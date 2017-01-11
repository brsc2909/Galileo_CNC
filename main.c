/******************************************************************************************************
 _                     _ _ _                  ___  ___   ___ _____  ___ ____  _  _    ___   ___
| |__   ___  ___ _   _| | (_) ___  _ __      / _ \/ _ \ / _ \___ / / _ \___ \| || |  / _ \ / _ \
| '_ \ / __|/ __| | | | | | |/ _ \| '_ \    / /_\/ | | | | | ||_ \| | | |__) | || |_| | | | | | |
| |_) |\__ \ (__| |_| | | | | (_) | | | |  / /_\\| |_| | |_| |__) | |_| / __/|__   _| |_| | |_| |
|_.__(_)___/\___|\__,_|_|_|_|\___/|_| |_|  \____/ \___/ \___/____/ \___/_____|  |_|  \___/ \___/ \

programm: galileo cnc
    main
******************************************************************************************************/
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
    heading();
    screen = GetStdHandle(STD_OUTPUT_HANDLE);
    system("COLOR 1F");

    // Declare variables and structures
    int baudrate = 9600;
    int dev_num = 50; // comm port to start search at
    char dev_name[MAX_PATH];
    DCB dcbSerialParams = {0};  // aray for storing serial parameters
    COMMTIMEOUTS timeouts = {0}; // array for storing timout information

     printf("Searching serial ports...\n");
    while(dev_num >= 0)
    {
        printf("\r                        ");
        printf("\rTrying COM%d...", dev_num);
        sprintf(dev_name, "\\\\.\\COM%d", dev_num);
        hSerial = CreateFile(
                        dev_name,
                        GENERIC_READ|GENERIC_WRITE,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL );
        if (hSerial == INVALID_HANDLE_VALUE) dev_num--;
        else break;
    }

    if (dev_num < 0)
    {
        printf("No serial port available\n");
        return 1;
    }

    printf("OK\n");

    // Set device parameters (9600 baud, 1 start bit,
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
    //*********************************************************************************************************************

    char cmdLine[200];
    while(1)
    {
        printf(">>");
        gets(cmdLine);
        cmd = strtok(cmdLine, " ");

        if(cmd!=false)
        {
            if(strcmp(cmd, "help")== 0)
            {
                help();
            }
            else if(strcmp(cmd, "getg")== 0)
            {
                GetGcode(dev_name);
            }
            else if(strcmp(cmd, "setup") == 0)
            {
                setup(dev_name);
            }

            else if(strcmp(cmd, "exit") == 0)
            {
                break;
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

