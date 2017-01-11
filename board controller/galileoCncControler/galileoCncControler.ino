
/******************************************************************************************************
 _                     _ _ _                  ___  ___   ___ _____  ___ ____  _  _    ___   ___
| |__   ___  ___ _   _| | (_) ___  _ __      / _ \/ _ \ / _ \___ / / _ \___ \| || |  / _ \ / _ \
| '_ \ / __|/ __| | | | | | |/ _ \| '_ \    / /_\/ | | | | | ||_ \| | | |__) | || |_| | | | | | |
| |_) |\__ \ (__| |_| | | | | (_) | | | |  / /_\\| |_| | |_| |__) | |_| / __/|__   _| |_| | |_| |
|_.__(_)___/\___|\__,_|_|_|_|\___/|_| |_|  \____/ \___/ \___/____/ \___/_____|  |_|  \___/ \___/ \
* programm: Intel Galileo CNC cntroller
* date: 30/01/2015
* Galileo cnc controller
* description: program to cont x,y and z movements of a cnc machine or similer via serial usb
* works in conjunction with galileo cnc windows application
*
* by brendan scullion G00302400
*******************************************************************************************************************/
#include <Arduino.h>
#include <string.h>

/********************************declare variables*****************************************/
//pins to controle stepper direction
const int X_DIRECTIONPIN = 10;
const int Y_DIRECTIONPIN = 8;
const int Z_DIRECTIONPIN = 12;
//pins to which a square wave is output to increment stepper motor
const int X_step = 11;
const int Y_step = 9;
const int Z_step = 13;

const int X_HOME = 7;
const int Y_HOME = 6;

/****************************************************************************/

void xMove(long, unsigned long, unsigned long*, unsigned int);
void yMove(long, unsigned long, unsigned long*, unsigned int);
void zMove(long, unsigned long, long*);
void jogX(int, unsigned long);
void jogY(int, unsigned long);
void jogZ(int, unsigned long);
void horizontalMove(long, long, unsigned long, unsigned long, unsigned long, unsigned long*, unsigned long*, unsigned int);
void calcDelay(unsigned int, unsigned long*);
void readserial(char*, int, int);
void parseString(struct apeture *D, char*, unsigned int*, unsigned int*,unsigned int*, unsigned long*, unsigned long*, unsigned int*, unsigned long, unsigned long *, unsigned long *, unsigned int* );
void flash(int, int, long *, unsigned long);

//structure for storing apeture details and locstions
struct apeture {
    int location;
    char type[20];
    float diam;
};

void setup()
{
	Serial.begin(9600); // start serial with baudrate 19200
        // initialise pins 	
        pinMode(13, OUTPUT);
	pinMode(12, OUTPUT);
	pinMode(11, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(9, OUTPUT);
	pinMode(8, OUTPUT);
	pinMode(7, INPUT);
	pinMode(6, INPUT);

        Serial.println("\ngalileo cnc V1.0 by brendan scullion");
}

void loop()
{

    long moveX = 0;
    long moveY = 0;
    long moveZ = 0;
    unsigned long stepDelay = 1;
    unsigned int steppRpm = 240;

    //keeping track of position
    unsigned long posX = 0;  // current position (in steps)
    unsigned long newX = 0;  // destination position (in steps)
    unsigned long posY = 0;
    unsigned long newY = 0;
    unsigned long xOffset = 0;
    unsigned long yOffset = 0;
    long posZ = 0;
    int newZ = 0;
    float unitconverter = 1;
    unsigned int interpolation = 1; // 1 for linear, 2 for clockwise circular, 3 for counterclockwise circular
    unsigned int d = 1, dps = 4;    // 4 for FSLAX24Y24 etc..
    unsigned int stepsPerUnit = 160;  //160 per mm or 6,350 per inch
    const int instructionsize = 50;
    unsigned int index = 0;
    int draw = 0, lift = 300;
    struct apeture D[30];
    char *instruction;
    instruction = (char*)malloc(instructionsize*sizeof(char)); // Allocate some space for the string
    newZ = lift;

    while(1)
    {
        strcpy(instruction, ""); // clear instruction array
        readserial(instruction, instructionsize, index); // read in one line of gerber file
        //parse string into useable data and instructions and convert chars to integer value
        parseString(  
                    D,                // structure for storing apeture data
                    instruction,      // g code or gerber code    
                    &dps,             // decimal points in coordinates 
                    &stepsPerUnit,    // amount of steps it takes the motor to move one unit of measurement mm/inch
                    &interpolation,   // are u drawing a circle or a straight line
                    &newX, &newY,     // destination x,y coordinate
                    &d,                 // what do do with the z axis 
                    stepDelay,          // delay between steps to controle speed of movement
                    &xOffset, &yOffset, // offsets for x, y  
                    &steppRpm           // rpm of stepper motor   
                    );
                    
        //newX -= xOffset;
        //newY -= yOffset; 
        calcDelay(steppRpm, &stepDelay);
        
        if(d == 1)
          newZ = draw;
        if (d == 2)
          newZ = lift;
        if(d == 3)
          newZ = lift;
        
  while(posX != newX || posY != newY || posZ != newZ)
       {

        //calculate distance to move
        moveX = newX  - posX ;
        moveY = newY - posY ;
        moveZ = newZ - posZ;

        //decide which function to use dependng on slope of line and direction

        // for linear interpolation
       if(interpolation == 1)
         {
              if(moveZ>0||moveZ<0)
              {
                  zMove(moveZ,stepDelay, &posZ);
              }
              if(moveY == 0)
              {
                  xMove(moveX,stepDelay,&posX, stepsPerUnit);
              }
              else if(moveX == 0)
              {
                  yMove(moveY,stepDelay,&posY, stepsPerUnit);
              }
              else
              {
                  horizontalMove(moveX, moveY,  // x and y distance machine has to move 
                                 stepDelay,    // delay for speed control
                                 newY, newX,    // destination x, y coordinates
                                 &posY, &posX,  // current X,Y coordinates
                                 stepsPerUnit  //steps per unit of measurement
                                 );
              }
          }
        if(posX==newX&&posY==newY&&posZ==newZ)
        {
           if(d == 3)
          {
            flash(draw, lift, &posZ, stepDelay);
          }
          Serial.print((float)posX/stepsPerUnit),Serial.print(" y:"), Serial.println((float)posY/stepsPerUnit);  
        }  
      }
      
      Serial.print("Instruction finished X:"); 
         
    }
    free(instruction);
}
