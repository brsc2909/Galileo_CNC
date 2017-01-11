/******************************************************************************************************
 _                     _ _ _                  ___  ___   ___ _____  ___ ____  _  _    ___   ___
| |__   ___  ___ _   _| | (_) ___  _ __      / _ \/ _ \ / _ \___ / / _ \___ \| || |  / _ \ / _ \
| '_ \ / __|/ __| | | | | | |/ _ \| '_ \    / /_\/ | | | | | ||_ \| | | |__) | || |_| | | | | | |
| |_) |\__ \ (__| |_| | | | | (_) | | | |  / /_\\| |_| | |_| |__) | |_| / __/|__   _| |_| | |_| |
|_.__(_)___/\___|\__,_|_|_|_|\___/|_| |_|  \____/ \___/ \___/____/ \___/_____|  |_|  \___/ \___/ \

programm: galileo cnc
    source for major functions which are called from main.
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

void heading()//print header logo and intro text
{
     printf("|=============================================================================|\n");
     printf("|\t                ___ __           _______   ________                   |");
     printf("\n|\t   ____ _____ _/ (_) /__  ____  / ____/ | / / ____/                   |\n");
     printf("|\t  / __ `/ __ `/ / / / _ \\/ __ \\/ /   /  |/ / /                        |\n");
     printf("|\t / /_/ / /_/ / / / /  __/ /_/ / /___/ /|  / /___                      |\n");
     printf("|\t \\__, /\\__,_/_/_/_/\\___/\\____/\\____/_/ |_/\\____/                      |\n");
     printf("|\t/____/                                                                |\n");
     printf("|===================================================by brendan scullion=======|\n");

    printf("\nWelcome, this is a program to to control a 3d printer/cnc machine using an \narduino or galileo micro controller.\n\n");
    printf("use the [help] command for a list of commands.\n");

}
/**================================================================================================
===== setup function is used to setup the machine prior to use
===== set up feed rate, position, etc...
==================================================================================================*/
void setup(char *devName)
{

    char ch = '0';
    char instruct[10];
    char serialIn[MAX_STRING_LENGHT]; // char  store string
    int dist, rpm;
    float feed; // float for storing feed rate

    printf("please select option from the list below\n 1\t adjust tool position.\n 2\t adjust feed rate(mm/s).\n");
    switch(_getch())
    {
        case '1':
            while(ch!='q')
            {
                printf("use the up and down numerical keys to move tool head up and down(press q to exit)\n");
                printf("enter jog distance ( 1000 = 1cm)(0 to exit):"), scanf("%i", &dist);
                if(dist == 0){
                    return;
                }
                ch = '0';
                while(ch!='e'&&ch!= 'q')
                {
                    ch = _getch();
                    if(ch == '7'){ // Z axis up
                        printf("Z up\n");
                        sprintf(instruct, "bZ%d*\n",dist); // creats string if format [function identifier][axis]<distance to move>
                    }
                    if(ch == '1'){ // Z-axis down
                        printf("Z down\n");
                        sprintf(instruct, "bz%d*\n",dist);
                    }
                    if(ch == '2'){ // y-axis back
                        printf(" y++ \n");
                        sprintf(instruct, "bY%d*\n",dist);
                    }
                    if(ch == '8'){ // Y-axis forward
                        printf("y--\n");
                        sprintf(instruct, "by%d*\n",dist);
                    }
                    if(ch == '6'){ // X-axis right
                        printf("X -- \n");
                        sprintf(instruct, "bX%d*\n",dist);
                    }
                    if(ch == '4'){ // X-axis left
                        printf("X ++\n");
                        sprintf(instruct, "bx%d*\n",dist);
                    }
                    sendSerial(instruct, devName); // send instruction to galileo
                    read_serial(serialIn, devName); // waits for reply from machine and adds it t char array (serialIn)
                    printf("%s", serialIn); // print reply
                }
            }
            break;
        case '2':
            printf("enter feed rate( < 6.25mm/s recomended):"),scanf("%f", &feed);
            if(feed > 6.25)
            {
                MessageBeep(0);
                if(MessageBoxA(0,"warning: feed rate greater than 6.25 can result in loss of position during process!\ndo u still want do use this speed?", "warning", MB_YESNO) == 7)
                {
                    printf("enter feed rate( < 6.25mm/s recomended):"),scanf("%f", &feed);
                }
            }
            rpm = feed*48; // feed rate is converted to motor rpm
            sprintf(instruct, "bf%d*", rpm);
            sendSerial(instruct, devName);
            Sleep(100);
            read_serial(serialIn, devName);
            break;
        default:
            printf("bad user input, please choose from options above");
            break;
    }
}
/**=======================================================================================================================
==================== help function opens a help file and displays it on the screen =======================================
=========================================================================================================================*/
int help()
{
    char ch[MAX_STRING_LENGHT]; // char array to store one line of the help file.

    file = fopen("helpfile.f","r"); // open help file
    if(file == NULL) // if the file cannot be opened or does not exist
    {
        perror("Error while opening the helpfile.\nmake sure help file exists!\n");
        return 1;
    }
    while(fgets(ch,100,file)!=NULL)// print out file line by line until end of file
    {
        printf("%s",ch);
    }
    fclose(file); // clode file
    return 0; // exit function
}
/**=====================================================================================================
============== gets g code calculates offsets and sends it to the galileo ==============================
=======================================================================================================*/
void GetGcode(char *devName)//read g code and set up for machine
{
    char gline[MAX_STRING_LENGHT];
    char serialIn[MAX_STRING_LENGHT];
    char file_name[20];
    char *pch;
    char message[20];
    unsigned int line_total = 1, current_line = 0;

   printf("Enter the name of file you wish to use\n");
   gets(file_name);
   //calculate image position and then calculate best offset for maximum space on board
   getOffset(file_name, // file to be calculated
            devName,   //device to send date to
             &line_total); // number of lines of code

   file = fopen(file_name,"r"); // read mode

   if( file == NULL )
   {
      perror("Error while opening the file.\n");
      return;
   }
   printf("The contents of %s file are :\n", file_name);

   while(1){
        while(fgets(gline,100,file)!=NULL){
            current_line++;
            printf("%s\n",gline);//print one line of the gcode
            Sleep(10);
            if(sendSerial(gline, devName) == 1){ // sent gcode to cnc machine
                return;
            }
            if(read_serial(serialIn, devName) == 1) // wait for response from machine and print
            {
                return;
            }
            printf("%s\n", serialIn);
            progressBar(current_line, line_total, 50);
            if(gline[0] == 'G'&& gline[1] == '5'&&gline[2] == '4'){
                pch = strtok(gline,"G54D * ");
                sprintf(message, "insert aperture no: %s", pch);
                messageup("aperture change", message);
            }
        }
        messageup("finished!","process has finished."); // when it reaches the end of the file
        break;
   }
   fclose(file); // close file
}
