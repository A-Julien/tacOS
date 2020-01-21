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

unsigned int  SYScreateUserThread(void * f,void * arg){
    UserThread * parrent = (UserThread *) currentThread->getUserThreadAdress();
    UserThread * child = new UserThread( f, arg, managerUserThreadID->GetNewId());
    if(parrent != NULL){
        parrent->addChildren(child);
    }


    child->setParrent(parrent);
    ((Thread *) child->getThread())->setUserThread(child);
    child->Run();
    //child->getThread()->status;
    return child->getId();
}


void * SYSWaitForChildExited(unsigned int CID) {
    UserThread * currentUserThread = (UserThread * ) currentThread->getUserThreadAdress();
    void * res = currentUserThread->WaitForChildExited(CID);
    return res;
}


void SYSExitThread(void * object){
    UserThread * userThread = (UserThread *) currentThread->getUserThreadAdress();
    userThread->exit(object);
    bool haveChild = userThread->getChildList()->IsEmpty();
    if(haveChild){
        // Proccss with the child;
        // Kill it ? wait it ?
    }
    userThread->DoneWithTheChildList();
}


void StartUserThread(int data) {
    thread_init * dataFork = (thread_init *) data;
    currentThread->space->InitRegisters();
    currentThread->space->RestoreState();
    machine->WriteRegister(PCReg, (int) dataFork->f);
    machine->WriteRegister(NextPCReg, ((int) dataFork->f) + 4);
    machine->WriteRegister(4, (int) dataFork->arg);
    //Adresse de retour ?
    machine->Run();

    return;
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

    if (which == SyscallException) {
        switch (type) {
            case SC_PutChar:
                synchConsole->SynchPutChar(machine->ReadRegister(4));
                break;

            case SC_PutString:
                char string[MAX_STRING_SIZE + 1];
                copyStringFromMachine(machine->ReadRegister(4), string, MAX_STRING_SIZE);
                synchConsole->SynchPutString(string);
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
                interrupt->Halt();
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

            default:
                printf("Unexpected user mode exception %d %d\n", which, type);
                ASSERT(FALSE);

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
