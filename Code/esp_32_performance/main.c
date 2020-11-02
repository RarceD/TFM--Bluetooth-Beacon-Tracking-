#include <stdio.h>
#include <string.h>
int main()
{
    const char *hola = "";
    printf("Testing fast \n");

    if (strcmp(hola, "") == 0)
    {
        printf("SON IGUALES \n");
    }
    else
    {
        printf("NO IGUALEs \n");
    }
    return 0;
}