#include "functions.h"
#include <string.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <conio.h>


FILE *file, *file2;
void heading()//print header logo and intro text
{
         printf("|-----------------------------------------------------------------------------|\n");
         printf("|\t                ___ __           _______   ________                   |");
         printf("\n|\t   ____ _____ _/ (_) /__  ____  / ____/ | / / ____/                   |\n");
         printf("|\t  / __ `/ __ `/ / / / _ \\/ __ \\/ /   /  |/ / /                        |\n");
         printf("|\t / /_/ / /_/ / / / /  __/ /_/ / /___/ /|  / /___                      |\n");
         printf("|\t \\__, /\\__,_/_/_/_/\\___/\\____/\\____/_/ |_/\\____/                      |\n");
         printf("|\t/____/                                                                |\n");
         printf("|---------------------------------------------------by brendan scullion-------|\n");

    printf("\nWelcome, this is a program to to controle a 3d printer/cnc machine using an \narduino or galileo micro controller.\n\n");
    printf("use the [help] command for a list of commands.\n");

}
void setup(HANDLE hs, unsigned char *TTS, char *devName)
{
    char xx;
    char ch;

    printf("please select option from the list below\n 0\tzero Z axis\n 1\tZero XY axis.\n 2\tset XY origin.\n ");
    scanf("%c", &xx);
    switch(xx)
    {
        case '0':
            printf("use the up and down keys to move tool head up and down\n");
            while(ch!='q')
            {
                ch = _getch();
                if(ch == '8')
                    printf("move up\n");
                if(ch == '2')
                    printf("move down\n");
            }
            break;
    }
}
void help()
{
    char ch[100];

    file = fopen("helpfile.f","r");
    if(file == NULL)
    {
        perror("Error while opening the helpfile.\nmake sure help file exists!\n");
        exit(EXIT_FAILURE);
    }
    while(fgets(ch,100,file)!=NULL)
    {
        printf("%s",ch);
    }
    fclose(file);
    return;

}
void setupComs(char *com, int *baud)
{
    printf("set up com port variables:");
    printf("\ncom port:");
    scanf("%s", com);
    printf("baud rate:");
    scanf("%i", baud);

}

void sendSerial(HANDLE hs,char instr, unsigned char *TTS, char *devName)
{
    /***********************send string**********************************************/
    char buffer[MAX_PATH];
    strcpy(buffer, ""); // empty buffer
    strcpy(buffer, instr);

    // Check that some text to send was provided
    if (strlen(buffer) == 0)
    {
        printf("Usage:\n\n\tSerialSend [/baudrate BAUDRATE] ");
        printf("[/devnum DEVICE_NUMBER] [/hex] \"TEXT_TO_SEND\"\n");
        return 1;
    }
    int n = 0, m = 0;
    while(n < strlen(buffer))
    {
        TTS[m] = buffer[n];
        m++; n++;
    }
    TTS[m] = '\0'; // Null character to terminate string

     // Send specified text
    DWORD bytes_written, total_bytes_written = 0;
    printf("Sending text... ");

    /************     error checking     ************/
    while(total_bytes_written < m)
    {
        if(!WriteFile(hs, TTS + total_bytes_written,
            m - total_bytes_written, &bytes_written, NULL))
        {
            fprintf(stderr, "Error writing text to %s\n", devName);
            CloseHandle(hs);
            return 1;
        }


        total_bytes_written += bytes_written;

    }
    fprintf(stderr, "\t\t%ld bytes written to %s\n", total_bytes_written, devName);

/******************************************************************************************************/

}
void myexit(int *keepgoing)
{
    keepgoing = 0;
}
void getOffset(HANDLE hs, unsigned char *TTS, char *devName)// find minimum x and y coordinates and set offset
{
    char gline[80];
    char file_name[20];
    char *pch;
    char select;
    char str[80];
    long xOffset, yOffset;
    int yy = 0, start = 0;
    unsigned long x = 0, y = 0, xMin, yMin;
    printf("Enter the name of file you wish to use\n");
    gets(file_name);

    file = fopen(file_name,"r"); // read mode

    if( file == NULL )
    {
      perror("Error while opening the file.\n");
      return 1;
    }
    printf("calculating offsets....\n");
    while(fgets(gline,100,file)!=NULL)
    {
        if (gline[0]=='X'||gline[0]=='Y')
        {
            if(strpbrk(gline,"Y")!= NULL)
            {
              yy = 1;
            }
           if(gline[0]=='X')
            {
                pch = strtok(gline,"X Y");
                x = atoi(pch);

               if(yy == 1)
               {
                    pch = strtok(NULL,"Y- D");
                    y = atoi(pch);
               }
            }
            else if(gline[0]=='Y')
            {
                pch = strtok (gline, "Y- D *");
                y = atoi(pch);
            }
            if(start == 0){
            xMin = x, yMin = y;
            }
            start = 1;
            if(x<xMin){
                xMin = x;
            }
            if(y<yMin){
                yMin = y;
            }
            yy = 0;
        }
    }
    xOffset = xMin-100000;
    yOffset = yMin-100000;
    printf("Min x position = %lu , Min y position = %lu\n", xMin, yMin);
    printf("Most appropriate offsets are x -%lu y -%lu\nwould you like to use these offsets?(y/n):",xOffset, yOffset);
    scanf("%c", &select);
    if(select == 'y')
    {
        sprintf(str, "box%lu", xOffset);
        printf("sending %s", str);

        sendSerial(hs, str, TTS, devName);
    }

}

void getgcode(HANDLE hs, unsigned char *TTS, char *devName)//read g code and set up for machine
{
    char gline[100];
    char serialIn[100];
    char file_name[25];
    char buffer[80];
   /*
    *******************************************************************************************************************/
   printf("Enter the name of file you wish to use\n");
   gets(file_name);

   file = fopen(file_name,"r"); // read mode
   file2 = fopen(devName, "r");

   if( file == NULL )
   {
      perror("Error while opening the file.\n");
      return 1;
   }
   if(file2 == NULL)
   {
       perror("error reading serial.\n");
   }
   printf("The contents of %s file are :\n", file_name);

   while(1){
        while(fgets(gline,100,file)!=NULL){
            if(_getch() == 'q'){
                return 0;
            }
            printf("%s",gline);//print one line of the gcode
            if(_getch()== 's'){
            sendSerial(hs,gline, TTS, devName);
            }


            /*if(fgets(serialIn, 80, file2)!=NULL){
            printf("hello");
            printf("\n%s\n",serialIn);
            }*/

        }
   }
   fclose(file);
   return 0;
}
