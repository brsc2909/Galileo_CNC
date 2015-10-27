
 /************************************************************************************************************
application for controling galileo or arduino cnc machine
by brendan scullion
10/11/2014
**********************************************************************************************************/
#include <string.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <conio.h>
#include "functions.h"

int main()
{
    system("COLOR 1F");

    // Declare variables and structures
    unsigned char text_to_send[MAX_PATH];
    unsigned char digits[MAX_PATH];
    int baudrate = 19200;
    int dev_num = 50;
    char dev_name[MAX_PATH];
<<<<<<< HEAD
    HANDLE hSerial;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};
=======
    DCB dcbSerialParams = {0};  // array for storing serial parameters
    COMMTIMEOUTS timeouts = {0}; // array for storing timout information
>>>>>>> origin/master

     printf("Searching serial ports...\n");
    while(dev_num >= 0)
    {
        printf("\r                        ");
        printf("\rTrying COM%d...", dev_num);
        sprintf(dev_name, "\\\\.\\COM%d", dev_num);
        hSerial = CreateFile(
        dev_name, GENERIC_READ|GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
        if (hSerial == INVALID_HANDLE_VALUE) dev_num--;
        else break;
    }

    if (dev_num < 0)
    {
        printf("No serial port available\n");
        return 1;
    }

    printf("OK\n");

    // Set device parameters (38400 baud, 1 start bit,
    // 1 stop bit, no parity)
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (GetCommState(hSerial, &dcbSerialParams) == 0)
    {
        printf("Error getting device state\n");
        CloseHandle(hSerial);
        return 1;
    }
    //dcbSerialParams.BaudRate = CBR_38400;
    dcbSerialParams.BaudRate = baudrate;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if(SetCommState(hSerial, &dcbSerialParams) == 0)
    {
        printf("Error setting device parameters\n");
        CloseHandle(hSerial);
        return 1;
    }
    // Set COM port timeout settings
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if(SetCommTimeouts(hSerial, &timeouts) == 0)
    {
        printf("Error setting timeouts\n");
        CloseHandle(hSerial);
        return 1;
    }

    char *cmd = NULL;
    char *para1 = NULL;
    char *para2 = NULL;
    char *para3 = NULL;

    char comPort[10];
    float baudRate;
    int keepGoing = 1;
    //*********************************************************************************************************************

    char cmdLine[200];
    heading();
    while(keepGoing == 1)
    {
        printf("->>");
        gets(cmdLine);
        cmd = strtok(cmdLine, " ");

        if(cmd!=false)
        {
            if(cmd != NULL)
            {
                para1 = strtok(NULL, " ");
            }
            else if(para1 != NULL)
            {
               para2 = strtok(NULL, " ");
            }
            else if(para2 != NULL)
            {
                para3 = strtok(NULL, " ");
            }
            else if(strcmp(cmd, "help")== 0)
            {
                help();
            }
            if(strcmp(cmd, "comset")== 0)
            {
                setupComs(comPort, baudRate);
            }
            else if(strcmp(cmd, "getg")== 0)
            {
                getgcode(hSerial,text_to_send,dev_name);
            }
            else if(strcmp(cmd, "offset")==0)
            {
                getOffset(hSerial, text_to_send, dev_name);
            }
            else if(strcmp(cmd, "setup") == 0)
            {
                setup(hSerial, text_to_send, dev_name);
            }

            else if(strcmp(cmd, "exit") == 0)
            {
                keepGoing = 0;
            }
            else
            {
                printf("Unknown command!\n");
            }
            printf("\n");
        }
    }
    // Close serial port
    printf("Closing serial port...");
    if (CloseHandle(hSerial) == 0)
    {
        printf("Error\n");
        return 1;
    }
    printf("OK\n");
    return 0;
}

