
/************************************************functions*************************************************************************/

void horizontalMove(long x ,long y , unsigned long dly, unsigned long ny, unsigned long nx, unsigned long *yp, unsigned long *xp, unsigned int spu) // if slope of line is greater than 1 or less than - 1 this function is used
{
    int ii,aa,bb;
    float accel = 3;
    int count  = 0;
    unsigned long posx,posy;
    float lineDist = 0;
    float m = (float)y/(float)x;

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
    while(*yp < ny||*yp > ny)
    {
        digitalWrite(Y_step,HIGH);
        delayMicroseconds((dly/1.2)*accel);
        digitalWrite(Y_step, LOW);
        *yp += bb;
        lineDist = sqrt((pow(((0-nx*m+ny)+m*(*xp)-(*yp)),2 ))/((m*m)+1));//calculate current distance from ideal line
        while(lineDist > 1)
        {
            digitalWrite(X_step,HIGH);
            delayMicroseconds((dly/1.2)*accel);
            digitalWrite(X_step, LOW);
            *xp += aa;
            lineDist = sqrt((pow(((0-nx*m+ny)+m* (*xp) - (*yp)),2 ))/((m*m)+1));//calculate current distance from ideal line
            delayMicroseconds(10);
             if(count<120 && accel > 1){
              accel -= 0.0175;
              count ++;
              }
        }
        if(count<120 && accel > 1){
          accel -= 0.0175;
          count ++;
        }
        
    }
}
void flash(int down, int up, long *pz, unsigned long dly)
{
    long posz = *pz;
    zMove(down - posz, dly, &posz);
    delay(200);
    zMove(up - posz, dly, &posz);
    *pz = posz;
}
/**********************************************************************************************************************
==========================================  move x axis  ==============================================================
***********************************************************************************************************************/
void xMove(long dist, unsigned long dly, unsigned long *xp, unsigned int spu)//move x axis
{

    int ii;
    int absDist;
    float accel  = 3;
    absDist = sqrt(dist*dist);
    
    if(dist>=1)
    {
        digitalWrite(X_DIRECTIONPIN,HIGH);// counter clockwise
    }
    if(dist<=0)
    {
        digitalWrite(X_DIRECTIONPIN,LOW);// clockwise
    }

    for(ii=0;ii<absDist;ii++)
    {
       digitalWrite(X_step, HIGH);
       delayMicroseconds(dly*accel);
       digitalWrite(X_step,LOW);
       delayMicroseconds(10);
       if(ii<120 && accel > 1){
         accel-=0.025;
       }
       if(ii > absDist-120 && ii > 120){
         accel+=0.025;
       }
    }
    *xp=*xp+dist;
}

/***********************************************************************************************************************
                                                move y axis
***********************************************************************************************************************/
void yMove(long dist, unsigned long dly, unsigned long *yp, unsigned int spu)//move y axis
{

    int ii;
    int absDist;
    float accel = 3;
    absDist = sqrt(dist*dist);//make an absolute value for distance (no negative values)

    if(dist>=1)
    {
        digitalWrite(Y_DIRECTIONPIN,HIGH);// counter clockwise
    }
    if(dist<=0)
    {
        digitalWrite(Y_DIRECTIONPIN,LOW);// clockwise
    }

    for(ii=0;ii<absDist;ii++)
    {
        //create square wave
       digitalWrite(Y_step, HIGH);
       delayMicroseconds(dly*accel);
       digitalWrite(Y_step,LOW);
       delayMicroseconds(10);
       if(ii<120 && accel > 1){
         accel-=0.025;
       }
       if(ii > absDist-120 && ii > 120){
         accel+=0.025;
       }

    }
    *yp = *yp+dist;
}
/***********************************************************************************************************************
                                                move z axis
***********************************************************************************************************************/
void zMove(long dist, unsigned long dly, long *zp)//move y axis
{

    int ii;
    int absDist;
    absDist = sqrt(dist*dist);//make an absolute value for distance (no negative values)
    float accel = 3;


    if(dist>=1)
    {
        digitalWrite(Z_DIRECTIONPIN,HIGH);// counter clockwise
    }
    if(dist<=0)
    {
        digitalWrite(Z_DIRECTIONPIN,LOW);// clockwise
    }

    for(ii=0;ii<absDist;ii++)
    {
        //create square wave
       digitalWrite(Z_step, HIGH);
       delayMicroseconds(dly*accel);
       digitalWrite(Z_step,LOW);
       delayMicroseconds(10);
       if(ii<120 && accel > 1){
         accel-=0.025;
       }
       if(ii > absDist-120 && ii > 120){
         accel+=0.025;
       }

    }
    *zp = *zp+dist;
}

/*==================================================================================================================
========================================== functions for manual movement  ==========================================//
====================================================================================================================*/
void jogX(int dist, unsigned long dly)// manual x move 
{
  int ii;
  int absDist;
  absDist = sqrt(dist*dist);
  if(dist>=1)
  {
      digitalWrite(X_DIRECTIONPIN,HIGH);// counter clockwise
  }
  if(dist<=0)
  {
      digitalWrite(X_DIRECTIONPIN,LOW);// clockwise
  }

  for(ii=0;ii<absDist;ii++)
  {
     digitalWrite(X_step, HIGH);
     delayMicroseconds(dly);
     digitalWrite(X_step,LOW);
     delayMicroseconds(10);
  }
}
void jogY(int dist, unsigned long dly)
{
  int ii;
  int absDist;
  absDist = sqrt(dist*dist);//make an absolute value for distance (no negative values)

  if(dist>=1)
  {
      digitalWrite(Y_DIRECTIONPIN,HIGH);// counter clockwise
  }
  if(dist<=0)
  {
      digitalWrite(Y_DIRECTIONPIN,LOW);// clockwise
  }

  for(ii=0;ii<absDist;ii++)
  {
      //create square wave
     digitalWrite(Y_step, HIGH);
     delayMicroseconds(dly);
     digitalWrite(Y_step,LOW);
     delayMicroseconds(10);

  }
}
void jogZ(int dist, unsigned long dly)
{
    int ii;
    int absDist;
    absDist = sqrt(dist*dist);//make an absolute value for distance (no negative values)


    if(dist>=1)
    {
        digitalWrite(Z_DIRECTIONPIN,HIGH);// counter clockwise
    }
    if(dist<=0)
    {
        digitalWrite(Z_DIRECTIONPIN,LOW);// clockwise
    }


    for(ii=0;ii<absDist;ii++)
    {
        //create square wave
       digitalWrite(Z_step, HIGH);
       delayMicroseconds(dly/0.8);
       digitalWrite(Z_step,LOW);
       delayMicroseconds(10);

    }
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
void parseString(struct apeture *D, char *str, unsigned int *dps, unsigned int *spu,unsigned int *intpl, unsigned long *x, unsigned long *y, unsigned int *z, unsigned long dly, unsigned long *xo, unsigned long *yo, unsigned int *rpm)
{
    char * pch; //temperary variable for holding string
    char tmp[25]; // temp varible for float conversion
    float tempf;  //
    float xf, yf;
    unsigned int G, num = 0;
    int dist;
    
    //FOR G54 apeture change CODES
    if(str[0] == 'G'&&str[3]=='D'){
        pch = strtok(str,"G D *");
        G = atoi(pch);

        pch = strtok(NULL,"D * ");
        *z = atoi(pch);
        Serial.print("D = "),Serial.println(*z);
    }
    //for G codes
    else if(str[0]=='G'){
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
                while(pch != NULL){
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
                Serial.println("units inches (deprecated command)");
                break;
            case 71:
                Serial.println("units MM (deprecated command)");
                break;
            case 74:
                Serial.println("Single Quadrant mode enabled");
                break;
            case 75:
                Serial.println("Multi-Quadrant mode enabled");
            case 90:
                Serial.println("absolute mode (deprecated command)");
                break;
            case 91:
                printf("incremental mode (deprecated command)");
                break;

            default:
                Serial.println("non standard gerber definition");
                break;
        }
  }

  //FOR SETUP INSTRUCTIONS
  else if(str[0]=='%'){
      //for format specification (FS)
      if(str[1]=='F'&&str[2]=='S'){
          if(str[3] == 'L'){
              Serial.println("leading zeroes omitted\n");
          }
          else if(str[3] == 'T'){
              Serial.println("trailing zeroes omitted\n");
          }
          if(str[4]=='A'){
              Serial.println("absolute notation\n");
          }
          else if(str[4]=='I'){
              Serial.println("incremental notation\n");
          }
          pch = strtok(str,"%FSLAX  Y");
          *dps = pch[1]-48;
          Serial.print("format "),Serial.print(pch[0]),Serial.print(":"),Serial.println(pch[1]);
      }
      //unit select
      if(str[1]=='M'&&str[2]=='O'){
         if(strcmp("%MOIN*%",str)==0){
             delay(100);
              Serial.println("inches\n");
              *spu = 6350;
          }
          else if(strcmp("%MOMM*%",str)==0){
            delay(100);
              Serial.println("millimetres\n");
              *spu = 250;
          }
           else{
             Serial.println("STRING ERROR");
           }
      }
      //apeture select
      if(str[1]=='A'&&str[2]=='D'){
          char shape = str[6];
          pch = strtok(str,"%AD  C R , *%");
          num = atoi(pch)-10;
          D[num].location = atoi(pch);
          if(shape=='C'){
              strcpy(D[num].type,"circlular");
          }
          else if(shape=='R'){
              strcpy(D[num].type, "rectangular");
          }
          D[num].location = atoi(pch);
          // get diameter of apeture
          pch = strtok(NULL,", *%");
          printf("\n%s\n",pch);
          strcpy(tmp, pch);
          sscanf(tmp,"%f", &tempf);
          D[num].diam = tempf;
         // Serial.print("apeture D"),Serial.print(num),Serial.print(" "),Serial.print(D[num].type),Serial.print(" size "),Serial.print(D[num].diam),Serial.println("MM");
      }
  }
  else if (str[0]=='X'|| str[0] == 'Y'){
    int yy = 0;
    int dd = 0;
    if (strpbrk(str, "Y")!= NULL){
      yy = 1; 
    }
    if(strpbrk(str,"D")!= NULL){
      dd = 1;
    }
    if(str[0] == 'X'){
        pch = strtok (str,"X Y- D * ");
        *x = atol(pch);
        xf = *x;
        xf/=1*pow(10,*dps);
        *x = (xf*(*spu));

        if(yy == 1){
            pch = strtok (NULL, "Y- D");
            *y = atol(pch);
            yf = *y;
            yf/=1*pow(10,*dps);
            *y = (yf*(*spu));
        }
    }
    else if(str[0] == 'Y'){
        pch = strtok (str, "Y- D *");
        *y = atol(pch);
        yf = *y;
        yf/=1*pow(10,*dps);
        *y = (yf*(*spu));
    }
    if(dd == 1){
      pch = strtok (NULL, "D *");
      *z = atoi(pch);
    }
  }
  else if(str[0] == 'D')
  {
    pch = strtok (str, "D *");
    *z = atoi(pch);
  }
  else if(str[0] == 'M' && str[1] == '0')
  {
    *x = 0, *y = 0, *z = 2;
  }
  //CODES FOR MANUAL MOVEMENT
  else if(str[0] == 'b')
    {
       if(str[1] == 'o'){
                 pch = strtok(str,"box y"); // get x offset 
                 *xo = atol(pch);
                 xf = *xo;
                 xf/=1*pow(10,*dps);
                 *xo = (xf*(*spu));
                 
                 pch = strtok(NULL,"y *"); // get y offset 
                 *yo = atol(pch);
                 yf = *yo;
                 yf/=1*pow(10,*dps);
                 *yo = (yf*(*spu));
                 Serial.print("offsets set. x-"), Serial.print(xf), Serial.print("y: -"),Serial.println(yf);
       }
       
       else if(str[1] == 'f'){ // set stepper rpm for feed rate. feed rate = rpm/60 * 1.25 ( 1.25 is the pitch of the threaded bar)
         pch = strtok(str, "bf *");
         *rpm = atoi(pch);
         Serial.print("feedrate set to "),Serial.print(*rpm/48),Serial.println("mm//S");
       }
    
       else if(str[1] == 'Z'){
          pch = strtok(str,"bZ *");
          dist = atoi(pch);
          jogZ(dist, dly);
       }
       else if(str[1] == 'z'){
          pch = strtok(str,"bz *");
          dist = atoi(pch);
          jogZ(0-dist, dly);
       }
       else if(str[1] == 'Y'){
          pch = strtok(str,"bY *");
          dist = atoi(pch);
          jogY(dist, dly);
       }
       else if(str[1] == 'y'){
          pch = strtok(str,"by *");
          dist = atoi(pch);
          jogY(0-dist, dly);
       }
       else if(str[1] == 'X'){
          pch = strtok(str,"bX *");
          dist = atoi(pch);
          jogX(dist, dly);
       }
       else if(str[1] == 'x'){
          pch = strtok(str,"bx *");
          dist = atoi(pch);
          jogX(0-dist, dly);
       }     
      
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

