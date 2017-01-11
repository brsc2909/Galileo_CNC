#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#define MAX_STRING_LENGHT 80

#include <string.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <conio.h>


void heading();
void setup(char*);
void exit();
int help();
void messageup(char*, char*);
void GetGcode(char*);
int getOffset(char*, char*, unsigned int*);
void delay(unsigned int);
int sendSerial(char *,char *);
int read_serial(char*, char*);
void progressBar(unsigned int, unsigned int, unsigned int);

HANDLE hSerial;
HANDLE screen;
FILE *file;


#endif // FUNCTIONS_H_INCLUDED

