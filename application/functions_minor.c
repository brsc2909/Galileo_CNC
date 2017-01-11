/******************************************************************************************************
 _                     _ _ _                  ___  ___   ___ _____  ___ ____  _  _    ___   ___
| |__   ___  ___ _   _| | (_) ___  _ __      / _ \/ _ \ / _ \___ / / _ \___ \| || |  / _ \ / _ \
| '_ \ / __|/ __| | | | | | |/ _ \| '_ \    / /_\/ | | | | | ||_ \| | | |__) | || |_| | | | | | |
| |_) |\__ \ (__| |_| | | | | (_) | | | |  / /_\\| |_| | |_| |__) | |_| / __/|__   _| |_| | |_| |
|_.__(_)___/\___|\__,_|_|_|_|\___/|_| |_|  \____/ \___/ \___/____/ \___/_____|  |_|  \___/ \___/ \

programm: galileo cnc
        source for minor functions which are called from other functions.

date: 30/01/2015
******************************************************************************************************/
#include "functions.h"
#include <string.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <conio.h>
/**===========================================================================
=========================== pop up message ===================================
==============================================================================*/
void messageup(char *title, char *body)
{
    MessageBeep(0);
    MessageBox(0,
               body,    // messgeto display n body of box
               title, // title string
               MB_OK // box contains only 1 o button
                );
}

int sendSerial(char *instr, char *devName)
{
    /***********************send string**********************************************/
    unsigned char text_to_send[MAX_PATH];
    char buffer[MAX_STRING_LENGHT];
    strcpy(buffer, ""); // empty buffer
    strcpy(buffer, instr);

    // Check that some text to send was provided
    if (strlen(buffer) == 0)
    {
        return 1;
    }
    int n = 0, m = 0;
    while(n < strlen(buffer))
    {
        text_to_send[m] = buffer[n];
        m++; n++;
    }
    text_to_send[m] = '\0'; // Null character to terminate string

     // Send specified text
    DWORD bytes_written, total_bytes_written = 0;
   //printf("Sending text... ");
    while(total_bytes_written < m)
    {
        if(!WriteFile(hSerial, text_to_send + total_bytes_written,
            m - total_bytes_written, &bytes_written, NULL)){
            fprintf(stderr, "Error writing text to %s\n", devName);
            CloseHandle(hSerial);
            return 1;
        }
        total_bytes_written += bytes_written;
    }
    //fprintf(stderr, "\t\t%ld bytes written to %s\n", total_bytes_written, devName);
    return 0;
}

int read_serial( char *message, char *devName )
{
    char buffer[MAX_STRING_LENGHT];
    DWORD bytes_recieved = 40, written = 0;
    int index = 0;
    strcpy(buffer,""); //empty buffer

    while(buffer!=NULL){ // wait untill serail message recieved
        if(ReadFile(hSerial, buffer,sizeof(buffer), // read serial
                &bytes_recieved, NULL ) == FALSE){
                    perror("ERROR READING SERIAL");
                return 1;
                }
        if(bytes_recieved){                // if something to read
            WriteFile(screen, buffer, bytes_recieved, &written, NULL);
            for(index = 0; buffer[index] == '*'; index++){
                message[index] = buffer[index];
            }
            message[index] = '\0';
            return 0;
        }
        if(kbhit()){
            if(getch() == 'q')
            {
                perror("\nUSER ABORT!!\n");
                return 1;
            }
        }
    }
}

void progressBar(unsigned int i, unsigned int n, unsigned int width)
{
    //when 100% is reached fuction is exited
   if ((i == n) && (i % (n/100+1) != 0) )
        return;

    float ratio  =  i/(float)n;
    int   c      =  ratio * width;
     // Show the percentage complete.
    printf("%3d%% [", (int)(ratio*100) );

    // Show the load bar.
    for ( i=0; i<c; i++){ // progress so far
       printf("=");
    }

    for ( i=c; i<width; i++){ // remainder filled with spaces
       printf(" ");
    }

    printf("]\n");
}

int getOffset(char *filename,char *devName, unsigned int *totalLines)// find minimum x and y coordinates and set offset
{
    char gline[MAX_STRING_LENGHT];
    char serialIn[MAX_STRING_LENGHT];
    char *pch;
    char select;
    char str[MAX_STRING_LENGHT];
    bool yy = FALSE;
    bool start = FALSE;
    unsigned long x = 0, y = 0, xMin = 0 , yMin  = 0;
    unsigned int count = 0;

    file = fopen(filename,"r"); // read mode
    if(file == NULL ){
      perror("Error while opening the file.\n");
      return 1;
    }
    printf("\ncalculating offsets....\n");
    while(fgets(gline,100,file)!=NULL)
    {
        count++;
        if (gline[0]=='X'||gline[0]=='Y'){
            if(strpbrk(gline,"Y")!= NULL){
                yy = true;
            }
           if(gline[0]=='X'){
                pch = strtok(gline,"X Y");
                x = atol(pch);

                if(yy == true){
                    pch = strtok(NULL,"Y- D");
                    y = atol(pch);
               }
            }
            else if(gline[0]=='Y'){
                pch = strtok (gline, "Y- D *");
                y = atol(pch);
            }
            if(!start){
            xMin = x, yMin = y;
            }
            start = TRUE;
            if(x<xMin){
                xMin = x;
            }
            if(y<yMin){
                yMin = y;
            }
            yy = FALSE;
        }
    }
    *totalLines = count;
    printf("Min x position = %lu , Min y position = %lu\n", xMin, yMin);
    xMin = xMin - 1000;
    yMin = yMin - 1000;
    printf("Most appropriate offsets are x -%lu y -%lu\nwould you like to use these offsets?(y/n):",xMin, yMin);
    scanf("%c", &select);
    if(select == 'y')
    {
        sprintf(str, "box%luy%lu*", xMin, yMin);
        if(sendSerial(str, devName) == 1){
            return 1;
        }
        if(read_serial(serialIn, devName) == 1){ // wait for response from machine and print
            return 1;
        }
    }
    fclose(file);
    return 0;
}
