/**
 * @file exception.cc
 * @brief Entry point into the Nachos kernel from user programs.
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 *
 * Copyright (c) 1992-1993 The Regents of the University of California.
 * All rights reserved.  See copyright.h for copyright notice and limitation
 * of liability and disclaimer of warranty provisions.
 * syscall -- The user code explicitly requests to call a procedure
 *      Entry point into the Nachos kernel from user programs.
 *      There are two kinds of things that can cause control to
 *      transfer back to here from user code:
 *
 *      syscall -- The user code explicitly requests to call a procedure
 *      in the Nachos kernel.  Right now, the only function we support is
 *      "Halt".
 *
 *      exceptions -- The user code does something that the CPU can't handle.
 *      For instance, accessing memory that doesn't exist, arithmetic errors,
 *      etc.
 *    Interrupts (which can also cause control to transfer from user
 *    code into the Nachos kernel) are handled elsewhere.
 *
 *   For now, this only handles the Halt() system call.
 *   Everything else core dumps.
 *   Copyright (c) 1992-1993 The Regents of the University of California.
 */


#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "../threads/system.h"
#include "userthread.h"

#define MAX_FILENAME_BUFFER 200

extern void ExitThread(void * object);
/**
 * UpdatePC : Increments the Program Counter register in order to resume
 * the user program immediately after the "syscall" instruction.
 */
static void UpdatePC() {
    int pc = machine->ReadRegister(PCReg);
    machine->WriteRegister(PrevPCReg, pc);
    pc = machine->ReadRegister(NextPCReg);
    machine->WriteRegister(PCReg, pc);
    pc += 4;
    machine->WriteRegister(NextPCReg, pc);
}

/**
 * copyStringFromMachine
 * Copy a string from MIPS to Kernel 
 * @param from : the Machine adresse where the string is
 * @param to : the Kernel adresse where to copy the string
 * @param size : the string's adresse
 */


void copyStringFromMachine(int from, char *to, unsigned size) {
    int valChar;
    for (unsigned i = 0; i < size; i++) {
        if (machine->ReadMem(from + i, sizeof(char), &valChar)) {
            to[i] = valChar;
        } else {
            to[i] = '\0';
            return;
        }
    }
    to[size] = '\0';
    return;
}


/**
 * copyMachineFromString
 * Copy a string from Kernel to Mips 
 * @param[in/out] from : the Kernel adresse where the string is
 * @param to : the Machine adresse where to copy the string
 * @param size : the string's adresse
 */

void copyMachineFromString(char *from, int to, unsigned size) {
    for (unsigned i = 0; i < size; i++) {

        machine->WriteMem(to + i, 1, from[i]);
        if (from[i] == '\0') return;

    }
    machine->WriteMem(size, 1, '\0');


}

/**
 * ProcedurePutInt
 * Put an int in the SynchConsole current output stream
 * Be carefull the absolute value of the int must be lower than 10^10-1
 * Negative int is supported
 * @param n : The int to be printed
 */

void ProcedurePutInt(int n) {
    int stock = n;
    if (n > 999999999 || n < -999999999) {
        machine->RaiseException(IntOutOfBounds, 0);
    }
    if (n == 0) {
        synchConsole->SynchPutChar('0');
        return;
    }
    if (n < 0) {
        synchConsole->SynchPutChar('-');
        n = -n;
    }

    int reverse = 0;
    while (n != 0) {
        reverse = (reverse * 10) + n % 10;
        n = n / 10;
    }

    while (stock != 0) {
        synchConsole->SynchPutChar('0' + reverse % 10);
        reverse = reverse / 10;
        stock = stock / 10;
    }
}

/**
 * ProcedurePutInt
 * Get the int that have been written int the input stream of the SynchConsole
 * Be carefull the absolute value of the int must be lower than 10^10-1
 * Negative int is supported
 * @param n : The adress where the int will be written
 */

void ProcedureGetInt(int *n) {
    //un entier est inferieur à 10 digit
    int res = 0;
    int debut = 0;
    bool positif = TRUE;
    char number[12] = "00000000000";
    number[12] = '\0';
    // Get the string
    synchConsole->SynchGetString(number, 11);
    // Find where the '\0' is
    int index = 0;
    while (number[index] != '\0' && number[index] != '\n' && number[index] != EOF) {
        index++;
    }
    index--;

    // There, we know the characteres of the int are between nunmber[0] and number[index]
    // No need to dig further
    if (number[0] == '-') {
        // If the first charactere is '-' then the int is a negativ number. 
        positif = FALSE;
        // Thus, the int is writted betwen number[1] and number[index]
        debut = 1;
    }

    for (int i = debut; i <= index; i++) {
        if (number[i] > '9' || number[i] < '0') {
            machine->RaiseException(CharInsteadOfInt, 0);
            synchConsole->SynchPutChar('!');
        }
        res = res * 10 + (number[i] - '0');
    }
    if (!positif) {
        res = -res;
    }
    // Write the int into the expetected adresses
    *n = res;

}
///
/// SYScreateUserThread
/// Syscall function that create a UserThread (and the thread)
/// \param f  The function where the thread will begin
/// \param arg the adress of the argument needed by this function
/// \return The unsigned int TID
unsigned int  SYScreateUserThread(void * f,void * arg){
    // Get the parent's adress
    UserThread * parrent = (UserThread *) currentThread->getUserThreadAdress();
    // Create the child UserThread
    UserThread * child = new UserThread( f, arg, managerUserThreadID->GetNewId(), machine->ReadRegister(6));
    // If there is a parent, it adopt the child
    if(parrent != NULL){
        parrent->addChildren(child);
        fileSystem->registerOpenFileTable(child->getTableOfOpenfile(), child->getId());
    }

    // Assign the parent to the child
    child->setParrent(parrent);
    // Stock the pointer to the userThread on the thread
    ((Thread *) child->getThread())->setUserThread(child);
    child->Run();
    return child->getId();
}

///
/// SYSWaitForChildExited
/// \param CID
/// \return the adress of the object returned
void * SYSWaitForChildExited(unsigned int CID) {

    UserThread * currentUserThread = (UserThread * ) currentThread->getUserThreadAdress();
    void * res = currentUserThread->WaitForChildExited(CID);
    if(res != (void * ) -1){
        managerUserThreadID->addIdFreed(CID);
    }
    return res;
}
///
/// SYSWaitForAllChildExited
/// Call SYSWaitForChildExited on all child

void  SYSWaitForAllChildExited() {
    List * child = ((UserThread *)currentThread->getUserThreadAdress())->getChildList();
    while(!child->IsEmpty()){

        UserThreadData * WaitedChild = (UserThreadData *) child->get(0);
        ((UserThread *)currentThread->getUserThreadAdress())->DoneWithTheChildList();

        SYSWaitForChildExited(WaitedChild->getID());

        child = ((UserThread *)currentThread->getUserThreadAdress())->getChildList();

    }
    ((UserThread *)currentThread->getUserThreadAdress())->DoneWithTheChildList();
}

///
/// SYSExitThread Syscall function to exit a thread (Kill it)
/// Function called at end of all thread if no return and/or ExitThread()
/// \param object
void SYSExitThread(void * object){
    UserThread * userThread = (UserThread *) currentThread->getUserThreadAdress();
    List * l = userThread->getChildList();
    // For all the child
    while(!l->IsEmpty()){
        UserThreadData * enfantMeta = (UserThreadData *) l->get(0);
        UserThread * enfant = (UserThread *) enfantMeta->getUserThread();
        DEBUG('t', "Parrent no : %d is going to die, looking for child %d is isSurvivor\n", userThread->getId(), enfant->getId());
        // If they are survivor, put in the list of the nearest parent and delete his survivor mode.
        if(enfant->isSurvivor()){
            UserThread * Grandpa = userThread->getParrent();
            if(Grandpa != NULL){
                enfant->setParrent(Grandpa);
                Grandpa->getChildList()->Append(enfantMeta);
                Grandpa->DoneWithTheChildList();
            } else {
                // https://youtu.be/qiMaOmDtaYI?t=69
            }
            l->removeElement((void *) enfantMeta);
            userThread->DoneWithTheChildList();
            enfant->setSurvivor(false);
        // Else wait for his ending
        } else {
             DEBUG('t', "Child %d ins't a survivor\n", enfant->getId());
            userThread->DoneWithTheChildList();
            SYSWaitForChildExited(enfant->getId());
        }
        l = userThread->getChildList();
    }
    userThread->DoneWithTheChildList();

    fileSystem->unregisterOpenFileTable(userThread->getId());
    //userThread->exit(object);
    if(managerUserThreadID->lastAlive()){
        delete(userThread);
        interrupt->Halt();
    }
    userThread->exit(object);



}

///
/// StartUserThread Init the NachOS thread
/// \param data

void StartUserThread(int data) {
    thread_init * dataFork = (thread_init *) data;
    currentThread->space->InitRegisters();
    currentThread->space->RestoreState();
    machine->WriteRegister(PCReg, (int) dataFork->f);
    machine->WriteRegister(NextPCReg, ((int) dataFork->f) + 4);
    machine->WriteRegister(4, (int) dataFork->arg);
    //Adresse de retour ?

    machine->Run((void *) dataFork->exit);

    return;
}

///
/// UserThread::StopChild Put the thread in BLOCKED status
/// \param CID
/// \return 0 if the child have been stoped, 1 if the child is currently stop, 2 if it's not a child's TID

int SYSStopChild(unsigned int CID){

    UserThread * currentUserThread = (UserThread *) currentThread->getUserThreadAdress();
    UserThreadData * state = (UserThreadData *) currentUserThread->getUserThreadDataChild(CID);
    if(state == NULL){
        currentThread->Yield();
        state = (UserThreadData *) currentUserThread->getUserThreadDataChild(CID);
        if(state == NULL){
            return 2;
        }
    }
    Thread * childThread = ((UserThread *) state->getUserThread())->getThread();

    IntStatus oldLevel = interrupt->SetLevel (IntOff);

    if(childThread->stopped ){

       interrupt->SetLevel (oldLevel);
        return 1;
    }
    childThread->enterCritiqueExt();




   interrupt->SetLevel (oldLevel);

    return 0;
}

///
/// UserThread::WakeUpChild Put a thread in Ready status
/// \param CID
/// \return 0 if the child have been WakeUp, 1 if the child is currently Ready, 2 if it's not a child's TID
int SYSWakeUpChild(unsigned int CID){
    UserThreadData * state = (UserThreadData *) ((UserThread *)currentThread->getUserThreadAdress())->getUserThreadDataChild(CID);
    if(state == NULL){
        return 2;
    }
    Thread * childThread = ((UserThread *) state->getUserThread())->getThread();
    IntStatus oldLevel = interrupt->SetLevel (IntOff);
    if(!childThread->stopped) {
        interrupt->SetLevel(oldLevel);
        return 1;
    }
    childThread->exitCritique();
    scheduler->ReadyToRun(childThread);
    childThread->stopped = false;
    interrupt->SetLevel (oldLevel);
   //  currentThread->Yield();
    return 0;
}


///
/// ExceptionHandler
///      Entry point into the Nachos kernel.  Called when a user program
///      is executing, and either does a syscall, or generates an addressing
///      or arithmetic exception.
///
///      For system calls, the following is the calling convention:
///
///      system call code -- r2
///              arg1 -- r4
///              arg2 -- r5
///              arg3 -- r6
///              arg4 -- r7
///
///      The result of the system call, if any, must be put back into r2.
///
/// And don't forget to increment the pc before returning. (Or else you'll
/// loop making the same system call forever!
///
/// @param which is the kind of exception.  The list of possible exceptions are in machine.h.
///

void
ExceptionHandler(ExceptionType which) {
    int type = machine->ReadRegister(2);
    int size;
    int resultat;
    void *getString;
    IntStatus oldLevel;
    char* filename;
    char* into;
    char* from;
    DEBUG('m', "User mode exception %d %d\n", which, type);


    DEBUG('m', "Unexpected user mode exception %d %d\n", which, type);
    if (which == SyscallException) {
        switch (type) {
            case SC_PutChar:
                synchConsole->SynchPutChar(machine->ReadRegister(4));
                break;

            case SC_PutString:
                char string[MAX_STRING_SIZE + 1];
                currentThread->enterCritique();
                copyStringFromMachine(machine->ReadRegister(4), string, MAX_STRING_SIZE);
                synchConsole->SynchPutString(string);
                currentThread->exitCritique();
                break;

            case SC_GetChar:
                machine->WriteRegister(2, synchConsole->SynchGetChar());
                break;

            case SC_GetString:
                size = machine->ReadRegister(5); // size passed in GetString() in syscall.h
                getString = malloc(sizeof(char) * (size + 1)); //allocate the necessary size of the string
                synchConsole->SynchGetString((char *) getString, size); //
                copyMachineFromString((char *) getString, machine->ReadRegister(4), size + 1);
                free(getString);
                break;

            case SC_PutInt:
                ProcedurePutInt(machine->ReadRegister(4));
                DEBUG('t', "TestDebug\n");
                break;

            case SC_GetInt:
                resultat = 0;
                ProcedureGetInt(&resultat);
                machine->WriteMem(machine->ReadRegister(4), sizeof(int), resultat);
                break;

            case SC_Feof:
                if (synchConsole->Feof()) {
                    machine->WriteRegister(2, 1);
                } else {
                    machine->WriteRegister(2, 0);
                }
                break;

            case SC_Halt:
                DEBUG('a', "Shutdown, initiated by user program.\n");
                interrupt->Halt();
                break;

            case SC_Exit:
                char str[50];
                sprintf(str, "Return value : %d ", machine->ReadRegister(4));
                DEBUG('s', str);
               // currentThread->Finish();
                //currentThread->Yield();
                SYSExitThread( (void *)  0);



               break;

            case SC_createUserThread:
                resultat = (int)  SYScreateUserThread((void *) machine->ReadRegister(4), (void *) machine->ReadRegister(5));

                machine->WriteRegister(2,(int) resultat);

            break;

            case SC_WaitForChildExited:
                resultat = (int) SYSWaitForChildExited(machine->ReadRegister(4));
                machine->WriteRegister(2,(int) resultat);
            break;

            case SC_ExitThread:
                SYSExitThread( (void *)  machine->ReadRegister(4));
            break;

            case SC_WaitForAllChildExited:
                SYSWaitForAllChildExited();

            break;

            case SC_makeAllChildSurvive:
                ((UserThread *) currentThread->getUserThreadAdress())->makeAllChildSurvive();
            break;

            case SC_makeChildSurvive:
                resultat =  ((UserThread *) currentThread->getUserThreadAdress())->makeChildSurvive((unsigned int) machine->ReadRegister(4));
                machine->WriteRegister(2,resultat);
            break;

            case SC_WakeUpChild:
                resultat =  SYSWakeUpChild((unsigned int) machine->ReadRegister(4));
                machine->WriteRegister(2,resultat);
            break;

            case SC_StopChild:
                oldLevel = interrupt->SetLevel (IntOff);

                DEBUG('t', "Stopping child %d\n", machine->ReadRegister(4));
                resultat =  SYSStopChild(machine->ReadRegister(4));
                machine->WriteRegister(2,resultat);
                (void) interrupt->SetLevel (oldLevel);

                break;

            case SC_ThreadEndedWithoutExit:
                DEBUG('m', "Thread %s Exited without exit statement", currentThread->getName());
            break;

            case SC_Open:
                filename = (char* ) malloc(sizeof(char) * MAX_FILENAME_BUFFER);
                copyStringFromMachine(machine->ReadRegister(4),filename,MAX_FILENAME_BUFFER);
                machine->WriteRegister(2,
                        synchConsole->fopen(
                            filename,
                            ((UserThread *) currentThread->getUserThreadAdress())->getId()
                        )
                 );
                delete filename;
                break;

            case SC_Read:
                // int fgets(char* into, int FileDescriptor, int numBytes, int tid);
                //syscall -> int fgets(int fileDescriptor, char* into, int numBytes);
                into = (char* ) malloc(sizeof(char) * machine->ReadRegister(6));
                machine->WriteRegister(2,
                        synchConsole->fgets(
                                into,
                                machine->ReadRegister(4),
                                machine->ReadRegister(6),
                                ((UserThread *) currentThread->getUserThreadAdress())->getId()
                        )
                );
                copyMachineFromString(into,machine->ReadRegister(5), machine->ReadRegister(6));
                delete into;
            break;

            case SC_Write:

                //int fputs(int fileDescriptor, char* from, int numBytes);
                //int SynchConsole::fputs(char* from, int fileDescriptor, int numBytes, int tid);
                from = (char* ) malloc(sizeof(char) * machine->ReadRegister(6));
                copyStringFromMachine(machine->ReadRegister(5),from, machine->ReadRegister(6));
                machine->WriteRegister(2,
                                       synchConsole->fputs(
                                               from,
                                               machine->ReadRegister(4),
                                               machine->ReadRegister(6),
                                               ((UserThread *) currentThread->getUserThreadAdress())->getId()
                                       )
                );
                delete from;
                break;

            case SC_Seek:
                //void int SynchConsole::fseek(int fileDescriptor, int position, int tid){
                //void fseek(int fileDescriptor, int position);

               synchConsole->fseek(
                       machine->ReadRegister(4),
                       machine->ReadRegister(5),
                       ((UserThread *) currentThread->getUserThreadAdress())->getId()
               );

                break;

            case SC_Close:
                //bool fclose(int fileDescriptor);
                //void SynchConsole::fclose(int fileDescriptor, int* threadTableFileDescriptor, unsigned int tid){
                //bool FileSystem::UserCloseFile(int fileDescriptor, int* threadTableFileDescriptor, unsigned int tid) {

                machine->WriteRegister(2,
                   synchConsole->fclose(
                           machine->ReadRegister(4),
                           ((UserThread *) currentThread->getUserThreadAdress())->getTableOfOpenfile(),
                           ((UserThread *) currentThread->getUserThreadAdress())->getId()
                   )
                );

                break;

            default:
                printf("Unexpected user mode exception %d %d\n", which, type);
                ASSERT(FALSE);
                break;

        }
        UpdatePC();
        // POUR NE PAS GARDER LA MAIN APRES UN SYSCALL
        currentThread->Yield();

    } else if (which == CharInsteadOfInt) {
        puts("Il fallais rentrer un caractère..");
        interrupt->Halt();
    } else if (which == IntOutOfBounds) {
        puts("Dépassement de value d'un int..");
        interrupt->Halt();
    }

}

