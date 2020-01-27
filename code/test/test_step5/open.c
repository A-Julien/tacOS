/**
 * @file open.c
 * @brief Open file and read nbByte
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#include "../../userprog/syscall.h"

int main()
{
    int file = fopen("cc");
    char content[10];
    fgets(file, content, 10);
    PutString(content);
    return 0;
}

