
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
unsigned int X_DIRECTIONPIN = 10;
unsigned int Y_DIRECTIONPIN = 8;
unsigned int Z_DIRECTIONPIN = 12;
//pins to which a square wave is output to increment stepper motor
unsigned int X_step = 11;
unsigned int Y_step = 9;
unsigned int Z_step = 13;

unsigned int X_HOME = 7;
unsigned int Y_HOME = 6;

/****************************************************************************/

void xMove(double, unsigned long, unsigned long*);
void yMove(double, unsigned long, unsigned long*);
void zMove(double, unsigned long, unsigned long*);
void horizontalMove(double, double, float,unsigned long, unsigned long, unsigned long, unsigned long*, unsigned long*);
void calcDelay(unsigned int, unsigned long*);
void readserial(char*, int, int);
void parseString(struct apeture *D, char*, unsigned int, unsigned int*, unsigned int*,unsigned int*, unsigned long*, unsigned long*, unsigned int*);

//structure for storing apeture details and locstions
struct apeture {

    int location;
    char type[20];
    float diam;

};

void setup()
{

	Serial.begin(19200);

	pinMode(13, OUTPUT);
	pinMode(12, OUTPUT);
	pinMode(11, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(9, OUTPUT);
	pinMode(8, OUTPUT);
	pinMode(7, INPUT);
	pinMode(6, INPUT);

}

void loop()
{

    double moveX = 0;
    double moveY = 0;
    double moveZ = 0;
    unsigned long stepDelay = 1;
    unsigned int steppRpm = 150;

    float slope = 0;

    //keeping track of position
    unsigned long posX = 0;
    unsigned long newX = 0;
    unsigned long posY = 0;
    unsigned long newY = 0;
    unsigned long posZ = 0;
    unsigned int newZ = 0;

    float unitconverter = 1;
    unsigned int interpolation = 1; // 1 for linear, 2 for clockwise circular, 3 for counterclockwise circular
    unsigned int dps = 4;    // 4 for FSLAX34Y34 etc..
    unsigned int stepsPerUnit = 250;  //250 per mm or 6,350 per inch

    const int instructionsize = 20;
    unsigned int index = 0;

    struct apeture D[30];

    char *instruction;
    instruction = (char*)malloc(instructionsize*sizeof(char)); // Allocate some space for the string

    while(1)
    {

        //readserial(instruction, instructionsize, index); // read in one line of gerber file
        strcpy(instruction, "X5000Y-5622D01*");
        parseString(D, instruction, index, &dps, &stepsPerUnit, &interpolation, &newX, &newY, &newZ); //seperae string into seperate instructions and convert chars to integer value
        calcDelay(steppRpm, &stepDelay);
        //calculate distance to move
        moveX = newX - posX;
        moveY = newY - posY;
        moveZ = newZ - posZ;

        //decide which function to use dependng on slope of line and direction

        // for linear interpolation
       if(interpolation == 1)
         {
           if(moveX>0||moveY>0||moveZ>0)
            {
                if(moveZ>0||moveZ<0)
                {
                    zMove(moveZ,stepDelay, &posZ);
                }
                if(moveY == 0)
                {
                    xMove(moveX,stepDelay,&posX);
                }
                else if(moveX == 0)
                {
                    yMove(moveY,stepDelay,&posY);
                }
                else
                {
                    horizontalMove(moveX, moveY, slope, stepDelay, newY, newX, &posY, &posX);
                }
            }
          }

        if(posX==newX&&posY==newY&&posZ==newZ)
        {
            Serial.println("\nfinisInstruction");
            Serial.print(posX),Serial.print("  "), Serial.println(posY);
        }
    }
    free(instruction);
}

/************************************************functions*************************************************************************/

void horizontalMove(double x ,double y , float m, unsigned long dly, unsigned long ny, unsigned long nx, unsigned long *yp, unsigned long *xp) // if slope of line is greater than 1 or less than - 1 this function is used
{
    int ii,aa,bb;
    unsigned long posx,posy;
    float lineDist = 0;
    m = y/x;

    Serial.println(m);
    Serial.println(" ");
    if(y<=0)
    {
        digitalWrite(Y_DIRECTIONPIN,LOW);// counter clockwise
        bb = -1;
    }
    if(y>=0)
    {
        digitalWrite(Y_DIRECTIONPIN,HIGH);// clockwise
        bb = 1;
    }

    if(x<=0)
    {
        digitalWrite(X_DIRECTIONPIN,LOW);// counter clockwise
        aa = -1;
    }
    if(x>=0)
    {
        digitalWrite(X_DIRECTIONPIN,HIGH);// clockwise
        aa = 1;
    }
    while(*yp<ny)
    {
        digitalWrite(Y_step,HIGH);
        delayMicroseconds(dly);
        digitalWrite(Y_step, LOW);
        *yp += bb;
        lineDist = sqrt((pow(((0-nx*m+ny)+m*(*xp)-(*yp)),2 ))/((m*m)+1));//calculate current distance from ideal line
        delayMicroseconds(10);
        //Serial.print("x="),Serial.print(*xp),Serial.print(" y="),Serial.print(*yp),Serial.print(" vaiation="),Serial.println(lineDist);

        while(lineDist > 1)
        {
            digitalWrite(X_step,HIGH);
            delayMicroseconds(dly);
            digitalWrite(X_step, LOW);
            *xp += aa;
            lineDist = sqrt((pow(((0-nx*m+ny)+m* (*xp) - (*yp)),2 ))/((m*m)+1));//calculate current distance from ideal line
            delayMicroseconds(10);
        }
    }
    Serial.println("done!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    Serial.print("x="), Serial.print((float)*xp/250),Serial.print(" y="), Serial.println((float)*yp/250);

}

/***********************************************************************************************************************
                                                move x axis
***********************************************************************************************************************/
void xMove(double dist, unsigned long dly, unsigned long *xp)//move x axis
{

    int ii;

    int absDist;
    absDist = sqrt(dist*dist);
    Serial.println("\njust x");


    if(dist>=1)
    {
        digitalWrite(X_DIRECTIONPIN,LOW);// counter clockwise
    }
    if(dist<=0)
    {
        digitalWrite(X_DIRECTIONPIN,HIGH);// clockwise
    }



    for(ii=0;ii<absDist;ii++)
    {
       digitalWrite(X_step, HIGH);
       delayMicroseconds(dly);
       digitalWrite(X_step,LOW);
       delayMicroseconds(10);

    }
    *xp=*xp+dist;
    Serial.print("\nx position = ");
    Serial.print(*xp);
    delay(1);
}

/***********************************************************************************************************************
                                                move y axis
***********************************************************************************************************************/
void yMove(double dist, unsigned long dly, unsigned long *yp)//move y axis
{

    int ii;
    int absDist;
    absDist = sqrt(dist*dist);//make an absolute value for distance (no negative values)
    Serial.println("\njust y");

    if(dist>=1)
    {
        digitalWrite(Y_DIRECTIONPIN,LOW);// counter clockwise
    }
    if(dist<=0)
    {
        digitalWrite(Y_DIRECTIONPIN,HIGH);// clockwise
    }

    Serial.println();
    delay(1);

    for(ii=0;ii<absDist;ii++)
    {
        //create square wave
       digitalWrite(Y_step, HIGH);
       delayMicroseconds(dly);
       digitalWrite(Y_step,LOW);
       delayMicroseconds(10);

    }
    *yp = *yp+dist;
    Serial.print("\ny position = ");
    Serial.print(*yp);
    delay(1);
}
/***********************************************************************************************************************
                                                move z axis
***********************************************************************************************************************/
void zMove(double dist, unsigned long dly, unsigned long *zp)//move y axis
{

    int ii;
    int absDist;
    absDist = sqrt(dist*dist);//make an absolute value for distance (no negative values)


    if(dist>=1)
    {
        digitalWrite(Z_DIRECTIONPIN,LOW);// counter clockwise
    }
    if(dist<=0)
    {
        digitalWrite(Z_DIRECTIONPIN,HIGH);// clockwise
    }

    Serial.println();
    delay(1);

    for(ii=0;ii<absDist;ii++)
    {
        //create square wave
       digitalWrite(Z_step, HIGH);
       delayMicroseconds(dly);
       digitalWrite(Z_step,LOW);
       delayMicroseconds(10);

    }
    *zp = *zp+dist;
    Serial.print("\ny position = ");
    Serial.print(*zp);
    delay(1);
}
/************************************************************************************************************************
                                    read instruction string from serial
*************************************************************************************************************************/
void readserial(char *inC,int a, int n)
{
    char inChar; // Where to store the character read
    while(Serial.available() == 0);
    while(Serial.available() > 0) // Don't read unless    // there you know there is data
    {
      if(n < a) // One less than the size of the array
      {
          inChar = Serial.read(); // Read a character
          inC[n] = inChar; // Store it
          n++; // Increment where to write next
          inC[n] = '\0'; // Null terminate the string
      }
      delay(5);
    }
}
/************************************************************************************************************************
                                    read instructions from recieved string
*************************************************************************************************************************/
void parseString(struct apeture *D, char *str, unsigned int n, unsigned int *dps,unsigned int *spu,unsigned int *intpl, unsigned long *x, unsigned long *y, unsigned int *z)
{
    char * pch; //temperary variable for holding string
    char tmp[25]; // temp varible for float conversion
    float tempf;
    float xf, yf;
    unsigned int G, num = 0;

    //FOR G54 apeture change CODES
    if(str[0] == 'G'&&str[3]=='D')
    {
        pch = strtok(str,"G D *");
        G = atoi(pch);


        pch = strtok(NULL,"D * ");
        *z = atoi(pch);
        Serial.print("D = "),Serial.println(*z);
    }
    //for G codes
    else if(str[0]=='G')
    {
        pch = strtok(str,"G *");
        G = atoi(pch);
        switch(G)
        {
            case 1:
                Serial.println("linear interpolation");
                *intpl = 1;
                break;
            case 2:
                Serial.println("circular interpolation clockwise");
                *intpl = 2;
                break;
            case 3:
                Serial.println("circular interpolation counterclockwise");
                *intpl = 3;
                break;
            case 4:
                pch = strtok(NULL," ");
                Serial.print("\n");
                while(pch != NULL)
                {
                   Serial.println(pch);
                   pch = strtok(NULL," *");

                }
                break;
            case 36:
                Serial.println("Area fill ON");
                break;
            case 37:
                Serial.println("Area fill OFF");
                break;
            case 70:
                Serial.println("units inches");
                break;
            case 71:
                Serial.println("units MM");
                break;
            case 74:
                Serial.println("Single Quadrant mode enabled");
                break;
            case 75:
                Serial.println("Multi-Quadrant mode enabled");
            case 90:
                Serial.println("absolute mode");
                break;
            case 91:
                printf("incremental mode");
                break;

            default:
                Serial.println("non standard gerber definition");
                break;
        }
  }

  //FOR SETUP INSTRUCTIONS
  else if(str[0]=='%')
  {
      //for format specification (FS)
      if(str[1]=='F'&&str[2]=='S')
      {

          if(str[3] == 'L')
          {
              Serial.println("leading zeroes omitted\n");
          }
          else if(str[3] == 'T')
          {
              Serial.println("trailing zeroes omitted\n");
          }

          if(str[4]=='A')
          {
              Serial.println("absolute notation\n");
          }
          else if(str[4]=='I')
          {
              Serial.println("incremental notation\n");
          }
          pch = strtok(str,"%FSLAX  Y");
          *dps = pch[1]-48;
          Serial.print("format "),Serial.print(pch[0]),Serial.print(":"),Serial.println(pch[1]);
      }
      //unit select
      if(str[1]=='M'&&str[2]=='O')
      {


         if(strcmp("%MOIN*%",str)==0)
          {
              Serial.println("inches\n");
              *spu = 6350;
          }
          else if(strcmp("%MOMM*%",str)==0)
          {
              Serial.println("millimetres\n");
              *spu = 250;
          }
      }

      //apeture select
      if(str[1]=='A'&&str[2]=='D')
      {
          char shape = str[6];
          pch = strtok(str,"%AD  C R , *%");
          num = atoi(pch)-10;
          D[num].location = atoi(pch);
          if(shape=='C')
          {
              strcpy(D[num].type,"circlular");
          }
          else if(shape=='R')
          {
              strcpy(D[num].type, "rectangular");
          }

          D[num].location = atoi(pch);
          // get diameter of apeture
          pch = strtok(NULL,", *%");
          printf("\n%s\n",pch);
          strcpy(tmp, pch);
          sscanf(tmp,"%f", &tempf);
          D[num].diam = tempf;

      }


  }
  else if (str[0]=='X'){
        pch = strtok (str,"X Y- ");
        *x = atol(pch);
        xf = *x;
        xf/=1*pow(10,*dps);
        *x = xf*(*spu);

        Serial.print("x:"),Serial.println(*x);

        pch = strtok (NULL, "Y- D");
        *y = atol(pch);
        yf = *y;
        yf/=1*pow(10,*dps);
        *y = yf*(*spu);

        Serial.print("Y:"),Serial.println(*y);

        pch = strtok (NULL, "D *");
        *z = atoi(pch);
        Serial.print("D:"),Serial.println(*z);


  }
}
/************************************************************************************************************************
                                    calculate delay for rpm
*************************************************************************************************************************/
void calcDelay(unsigned int x, unsigned long *dly)
{
    float y=60;
    y = y/200;
    *dly = (y/x*1000000)-10;
}
