// exception.cc 
//      Entry point into the Nachos kernel from user programs.
//      There are two kinds of things that can cause control to
//      transfer back to here from user code:
//
//      syscall -- The user code explicitly requests to call a procedure
//      in the Nachos kernel.  Right now, the only function we support is
//      "Halt".
//
//      exceptions -- The user code does something that the CPU can't handle.
//      For instance, accessing memory that doesn't exist, arithmetic errors,
//      etc.  
//
//      Interrupts (which can also cause control to transfer from user
//      code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

//----------------------------------------------------------------------
// UpdatePC : Increments the Program Counter register in order to resume
// the user program immediately after the "syscall" instruction.
//----------------------------------------------------------------------
static void
UpdatePC() {
  int pc = machine->ReadRegister(PCReg);
  machine->WriteRegister(PrevPCReg, pc);
  pc = machine->ReadRegister(NextPCReg);
  machine->WriteRegister(PCReg, pc);
  pc += 4;
  machine->WriteRegister(NextPCReg, pc);
}

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

void copyMachineFromString(char *from, int to, unsigned size) {
  for (unsigned i = 0; i < size; i++) {

    machine->WriteMem(to + i, 1, from[i]);
    if (from[i] == '\0') return;

  }
  machine->WriteMem(size, 1, '\0');


}

void ProcedurePutInt(int n){
  if(n == 0){
     synchConsole->SynchPutChar('0');
     return;
  }
  if(n < 0){
    synchConsole->SynchPutChar('-');
    n = -n;
  }

  int reverse = 0;
  while(n != 0){
    reverse = (reverse * 10 ) + n%10;
    n = n/10;
  }


  while(reverse != 0){
    synchConsole->SynchPutChar('0'+reverse%10);
    reverse = reverse/10;
  }
}

void ProcedureGetInt(int * n){
  //un entier est inferieur à 10 digit
  int res = 0;
  int debut = 0;
  bool positif = TRUE;
  char number[11] = "0000000000";
  number[11] = '\0';
  synchConsole->SynchGetString(number,10);

  int index = 0;
  while(number[index] != '\0' && number[index] != '\n' && number[index] != EOF){
    index++;
  }
  index--;
  if(number[0] == '-'){
    positif = FALSE;
    debut = 1;
  }
  for(int i = debut ; i <= index; i++){
    if(number[i] > '9' || number[i] < '0' ){
      //RAISE EXCEPTION CE N'EST PAS UN CHIFFRE
    }
    res = res * 10 + (number[i] - '0'); 
  }
  if(!positif){
    res = -res;
  }
  *n = res;
  
}


//----------------------------------------------------------------------
// ExceptionHandler
//      Entry point into the Nachos kernel.  Called when a user program
//      is executing, and either does a syscall, or generates an addressing
//      or arithmetic exception.
//
//      For system calls, the following is the calling convention:
//
//      system call code -- r2
//              arg1 -- r4
//              arg2 -- r5
//              arg3 -- r6
//              arg4 -- r7
//
//      The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//      "which" is the kind of exception.  The list of possible exceptions 
//      are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which) {
  int type = machine->ReadRegister(2);
  int size;
  int resultat;
  void *getString;

  if (which == SyscallException) {
    switch (type) {


      case SC_PutChar:
      DEBUG('p', "Putting char\n");
      synchConsole->SynchPutChar(machine->ReadRegister(4));
      break;

      case SC_PutString:
      DEBUG('p', "Putting String\n");
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
      break;

      case SC_GetInt: 
        resultat = 0;
        ProcedureGetInt(&resultat);
        machine->WriteMem(machine->ReadRegister(4), sizeof(int), resultat);
      break; 

      case SC_Feof:
        if(synchConsole->Feof()){
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
      machine->WriteRegister(2, machine->ReadRegister(4));
      interrupt->Halt();
      break;

      default:
      printf("Unexpected user mode exception %d %d\n", which, type);
      ASSERT(FALSE);

              }
              UpdatePC();
            }

          }
