#include "functions.h"
#include <string.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <conio.h>

FILE *file;
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

    printf("\nWelcome, this is a program to to controle a 3d printer/cnc machine using an \narduino or galileo micro controller.\n\n");
    printf("use the [help] command for a list of commands.\n");

}
void messageup(char *title, char *body)
{
    MessageBeep(0);
    MessageBox(0,body,title, MB_OK );
}
void setup(char *devName)
{

    char ch = '0';
    char instruct[10];
    char serialIn[MAX_STRING_LENGHT];
    int dist, rpm;
    float feed;

    printf("please select option from the list below\n 1\t adjust tool position.\n 2\t adjust feed rate(mm/s).\n");
    switch(_getch())
    {
        case '1':
            while(ch!='q')
            {
                printf("use the up and down numerical keys to move tool head up and down(press q to exit)\n");
                printf("enter jog distance (10^-3):"), scanf("%i", &dist);
                ch = '0';
                while(ch!='e'&&ch!= 'q')
                {
                    ch = _getch();
                    if(ch == '7'){
                        printf("Z up\n");
                        sprintf(instruct, "bZ%d*\n",dist);
                        sendSerial(instruct, devName);
                    }
                    if(ch == '1'){
                        printf("Z down\n");
                        sprintf(instruct, "bz%d*\n",dist);
                        sendSerial(instruct, devName);
                    }
                    if(ch == '2'){
                        printf(" y++ \n");
                        sprintf(instruct, "bY%d*\n",dist);
                        sendSerial(instruct, devName);
                    }
                    if(ch == '8'){
                        printf("y--\n");
                        sprintf(instruct, "by%d*\n",dist);
                        sendSerial(instruct, devName);
                    }
                    if(ch == '6'){
                        printf("X -- \n");
                        sprintf(instruct, "bX%d*\n",dist);
                        sendSerial(instruct, devName);
                    }
                    if(ch == '4'){
                        printf("X ++\n");
                        sprintf(instruct, "bx%d*\n",dist);
                        sendSerial(instruct, devName);
                    }
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
            rpm = feed*48;
            sprintf(instruct, "bf%d*", rpm);
            sendSerial(instruct, devName);
            Sleep(100);
            read_serial(serialIn, devName);
            break;
    }
}
int help()
{
    char ch[MAX_STRING_LENGHT];

    file = fopen("helpfile.f","r");
    if(file == NULL)
    {
        perror("Error while opening the helpfile.\nmake sure help file exists!\n");
        return 1;
    }
    while(fgets(ch,100,file)!=NULL)
    {
        printf("%s",ch);
    }
    fclose(file);
    return 0;
}
/**=============================================================================================================================/
/============================ function to send message through serail com port =================================================/
/==============================================================================================================================*/
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
        printf("Usage:\n\n\tSerialSend [/baudrate BAUDRATE] ");
        printf("[/devnum DEVICE_NUMBER] [/hex] \"TEXT_TO_SEND\"\n");
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
    printf("Sending text... ");

    /************     error checking     ************/
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
    fprintf(stderr, "\t\t%ld bytes written to %s\n", total_bytes_written, devName);

/******************************************************************************************************/
    return 0;
}
int read_serial( char *message, char *devName )
{

    char buffer[MAX_STRING_LENGHT];
    DWORD bytes_recieved = MAX_STRING_LENGHT, written = 0;
    strcpy(buffer,""); //empty buffer

    while(buffer!=NULL){ // wait untill serail message revieved
        ReadFile(hSerial, buffer,sizeof(buffer), // read serial
                &bytes_recieved, NULL );
        if(bytes_recieved){                // if something to read
            WriteFile(screen, buffer, bytes_recieved, &written, NULL);

            strncpy(message, buffer, bytes_recieved);
            message[bytes_recieved] = '\0';
            return 0;
        }
        if(kbhit()){
            if(getch() == 'q')
            {
                printf("\nOPERATION ABORTED!!\n");
                return 1;
            }
        }
    }
}
static inline void progressBar(unsigned int i, unsigned int n, unsigned int width)
{
    //when 100% is reached fuction is exited
   if ((i != n) && (i % (n/100+1) != 0) )
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

    printf("]\b");

}

void myexit(int *keepgoing)
{
    keepgoing = 0;
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
    unsigned long x = 0, y = 0, xMin, yMin;
    unsigned int count = 0;
    printf("hello");

    file = fopen(filename,"r"); // read mode
    if( file == NULL )
    {
      perror("Error while opening the file.\n");
      return 1;
    }
    printf("\ncalculating offsets....\n");
    while(fgets(gline,100,file)!=NULL)
    {
        count++;
        if (gline[0]=='X'||gline[0]=='Y')
        {
            if(strpbrk(gline,"Y")!= NULL)
            {
              yy = TRUE;
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
    xMin = xMin - 1000;
    yMin = yMin - 1000;
    printf("Min x position = %lu , Min y position = %lu\n", xMin, yMin);
    printf("Most appropriate offsets are x -%lu y -%lu\nwould you like to use these offsets?(y/n):",xMin, yMin);
    scanf("%c", &select);
    if(select == 'y')
    {
        sprintf(str, "box%luy%lu*", xMin, yMin);
        sendSerial(str, devName);
        read_serial(serialIn, devName); // wait for response from machine and print
    }
    fclose(file);
}
void getgcode(char *devName)//read g code and set up for machine
{
    char gline[MAX_STRING_LENGHT];
    char serialIn[MAX_STRING_LENGHT];
    char file_name[20];
    char buffer[MAX_STRING_LENGHT];
    unsigned int line_total = 1, current_line = 0, percentComplete = 0;

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
            progressBar(current_line, line_total, 50);
            printf("%s\n",gline);//print one line of the gcode
            Sleep(10);
            sendSerial(gline, devName); // sent gcode to cnc machine
            if(read_serial(serialIn, devName) == 1) // wait for response from machine and print
            {
                printf("OPERATION ABORTED!\n");
                return;
            }
            printf("%s", serialIn);
            if(gline[0] == 'G'&& gline[1] == '5'&&gline[2] == '4'){
                messageup("apeture change", serialIn);
            }
           printf("\b");


        }
        messageup("finished!","process has finished.");
        break;
   }
   fclose(file);
}
