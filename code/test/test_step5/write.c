/**
 * @file open.c
 * @brief Open file and read nbByte
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#include "../../userprog/syscall.h"

int main()
{
    int file = fopen("cc");
    fseek(file, 0);
    char from[5] = "12345";
    fputs(file, from, 5);

    fseek(file, 0);
    char content[24];
    fgets(file, content, 24);
    PutString(content);
    fclose(file);
    //void fseek(int fileDescriptor, int position);
    //int fputs(int fileDescriptor, char* from, int numBytes);
    return 0;
}

