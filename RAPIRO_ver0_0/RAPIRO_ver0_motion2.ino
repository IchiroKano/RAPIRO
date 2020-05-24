// by ShotaIshiwatari is licensed under the Creative Commons - Public Domain Dedication license.
// Modified by IchiroKANO2; Changed all of sample commands to aggressive motions.

#include 

#define SHIFT 7
#define R 0          // Red LED
#define G 1          // Green LED
#define B 2          // Blue LED
#define TIME 15      // Column of Time
#define MAXSN 12     // Max Number of Servos, Kano changed from 10 to 12, add 2 motions
#define MAXMN 11     // Max Number of Motions
#define MAXFN 8      // Max Number of Frames 
#define POWER 17     // Servo power supply control pin
#define ERR -1       // Error

int i = 0;
int t = 1;
Servo servo[MAXSN];
uint8_t eyes[3] = { 0, 0, 0};

// Please adjust your initial position, 
int trim[MAXSN] = { 0,  // Head yaw
                    0,  // Waist yaw
                    0,  // R Sholder roll
                    0,  // R Sholder pitch
                    0,  // R Hand grip
                    0,  // L Sholder roll
                    0,  // L Sholder pitch
                    0,  // L Hand grip
                    0,  // R Foot yaw
                    0,  // R Foot pitch
                    0,  // L Foot yaw
                    0}; // L Foot pitch

int nowAngle[MAXSN] =        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  // Initialize array to 0
int targetAngle[MAXSN] =     { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  // Initialize array to 0
int deltaAngle[MAXSN] =      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  // Initialize array to 0
uint8_t bufferAngle[MAXSN] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  // Initialize array to 0
uint8_t tempAngle[MAXSN] =   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  // Initialize array to 0

int nowBright[3] =        { 0, 0, 0};  // Initialize array to 0
int targetBright[3] =     { 0, 0, 0};  // Initialize array to 0
int deltaBright[3] =      { 0, 0, 0};  // Initialize array to 0
uint8_t bufferBright[3] = { 0, 0, 0};  // Initialize array to 0
uint8_t tempBright[3] =   { 0, 0, 0};  // Initialize array to 0

double startTime =   0;                // Motion start time(msec)
double endTime =     0;                // Motion end time(msec)
int remainingTime =  0;                // Motion remaining time(msec)
uint8_t bufferTime = 0;                // Motion buffer time (0.1sec)

uint8_t motionNumber = 0;
uint8_t frameNumber = 0;
char mode = 'M';

uint8_t motion[MAXMN][MAXFN][16]={
{  // #M0 Stop
  { 90, 90,  0,130, 90,180, 50, 90, 90, 90, 90, 90,  0,  0,255, 10},
  { 90, 90,  0,130, 90,180, 50, 90, 90, 90, 90, 90,  0,  0,  0,  0},
  { 90, 90,  0,130, 90,180, 50, 90, 90, 90, 90, 90,  0,  0,  0,  0},
  { 90, 90,  0,130, 90,180, 50, 90, 90, 90, 90, 90,  0,  0,  0,  0},
  { 90, 90,  0,130, 90,180, 50, 90, 90, 90, 90, 90,  0,  0,  0,  0},
  { 90, 90,  0,130, 90,180, 50, 90, 90, 90, 90, 90,  0,  0,  0,  0},
  { 90, 90,  0,130, 90,180, 50, 90, 90, 90, 90, 90,  0,  0,  0,  0},
  { 90, 90,  0,130, 90,180, 50, 90, 90, 90, 90, 90,  0,  0,  0,  0}
},
{  // #M1 Forward aggresive
  { 90, 90,  0, 90, 90,180, 90, 90, 80,100, 80,120,  0,255,  0,  3},
  { 90, 90, 30, 90, 90,180, 90, 90, 70, 90, 60, 80,  0,  0,255,  5},
  { 70,110, 50, 90, 90,180, 90, 90, 70, 70, 60, 80,  0,  0,255,  3},
  { 90, 90,  0, 90, 90,180, 90, 90,100, 60, 92, 70,  0,255,  0,  3},
  { 90, 90,  0, 90, 90,120, 90, 90,110, 90,102, 90,  0,  0,255,  5},
  {100, 70,  0, 90, 90,100, 90, 90,110,100,110,110,  0,  0,255,  3},
  { 90, 90,  0, 90, 90,180, 90, 90, 80,100, 80,120,255,  0,  0,  0},
  { 90, 90,  0, 90, 90,180, 90, 90, 80,100, 80,120,255,  0,  0,  0}
},
{  // #M2 Back aggresive
  { 90, 90,  0, 90, 90,180, 90, 90,100,100,100,120,  0,255,  0,  5},
  {100, 70,  0, 90, 90,140, 90, 90,110, 90,110, 90,  0,  0,255,  3},
  {100, 70,  0, 90, 90,160, 90, 90,110, 70,110, 80,  0,  0,255,  3},
  { 90, 90,  0, 90, 90,180, 90, 90, 80, 60, 80, 80,  0,255,  0,  5},
  { 80,110, 20, 90, 90,180, 90, 90, 70, 90, 70, 90,  0,  0,255,  3},
  { 80,110,  0, 90, 90,180, 90, 90, 70,100, 70,110,  0,  0,255,  3},
  { 90, 90,  0, 90, 90,180, 90, 90, 90, 90, 90, 90,  0,  0,  0,  0},
  { 90, 90,  0, 90, 90,180, 90, 90, 90, 90, 90, 90,  0,  0,  0,  0}
},
{  // #M3 Left
  { 90, 70,  0, 90, 90,180,110, 90, 95,100, 85,120,  0,255,  0,  5},
  { 90,130,  0, 90, 90,180,110, 90,100, 90, 80, 90,  0,  0,255,  3},
  { 90, 90,  0, 90, 90,180, 90, 90,100, 70, 80, 80,  0,  0,  0,  5},
  { 90,130, 30, 70, 90,180,110, 90, 85, 60, 95, 70,  0,  0,255,  3},
  { 90, 90,  0, 90, 90,180,130, 90, 80, 90,100, 90,  0,  0,  0,  5},
  { 90, 90,  0, 90, 90,180, 90, 90, 80,100,100,130,  0,  0,255,  3},
  { 90, 90,  0, 90, 90,180, 90, 90, 90, 90, 90, 90,255,  0,  0,  0},
  { 90, 90,  0, 90, 90,180, 90, 90, 90, 90, 90, 90,255,  0,  0,  0}
},
{  // #M4 Right
  { 90,110,  0, 70, 90,180, 90, 90, 95, 60, 85, 70,  0,255,  0,  5},
  { 90, 50,  0, 70, 90,180, 90, 90,100, 90, 80, 90,  0,  0,255,  3},
  { 90, 90,  0, 90, 90,180, 90, 90,100,100, 80,110,  0,  0,  0,  5},
  { 90, 50,  0, 70, 90,150,110, 90, 85,110, 95,120,  0,  0,255,  3},
  { 90, 90,  0, 50, 90,180, 90, 90, 80, 90,100, 90,  0,  0,  0,  5},
  { 90, 90,  0, 90, 90,180, 90, 90, 80, 70,100, 80,  0,  0,255,  3},
  { 90, 90,  0, 90, 90,180, 90, 90, 90, 90, 90, 90,255,  0,  0,  0},
  { 90, 90,  0, 90, 90,180, 90, 90, 90, 90, 90, 90,255,  0,  0,  0}
},
{  // #M5 RED, Left and Right, Hands
  { 60,120,  0, 45, 90,180, 90, 90, 90, 80, 90, 80,255,  0,255,  5},
  {120, 60,  0, 90, 90, 70,100, 90, 90, 90, 90, 90,255,  0,  0,  5},
  {120, 60,  0, 90, 90, 70,100,110, 90, 90, 90, 90,255,  0,  0,  1},
  {120, 60,  0, 90, 90, 70,100, 70, 90, 90, 90, 90,255,  0,  0,  1},
  {120, 60,  0, 90, 90,180,135, 90, 90,100, 90,100,255,  0,255,  5},
  { 60,120, 90, 80, 90,180, 90, 90, 90, 90, 90, 90,255,  0,  0,  5},
  { 60,120, 90, 80, 70,180, 90, 90, 90, 90, 90, 90,255,  0,  0,  1},
  { 60,120, 90, 80,110,180, 90, 90, 90, 90, 90, 90,255,  0,  0,  1}
},
{  // #M6 Left and Right, hands up and down
  { 90, 45,  0, 90, 90,180, 90, 90,110, 90,110, 90,  0,255,  0, 10},
  { 90, 45,160, 90, 90,  0, 90, 90,110, 90,110, 90,  0,255,255,  5},
  { 90, 45,  0, 90, 90,180, 90, 90,110, 90,110, 90,  0,255,255,  5},
  { 90, 45,160, 90, 90,  0, 90, 90,110, 90,110, 90,  0,255,255,  5},
  { 90,135,  0, 90, 90,180, 90, 90, 70, 90, 70, 90,  0,255,  0, 10},
  { 90,135,160, 90, 90,  0, 90, 90, 70, 90, 70, 90,  0,255,255,  5},
  { 90,135,  0, 90, 90,180, 90, 90, 70, 90, 70, 90,  0,255,255,  5},
  { 90,135,160, 90, 90,  0, 90, 90, 70, 90, 70, 90,  0,255,255,  5}
},
{  // #M7 PinK, Hans open and close quickly
  { 50,130, 90, 70,110,180, 90, 90, 90, 90, 90, 90,204,102,255,  3},
  { 50,130, 90, 70, 70,180, 90, 90, 90, 90, 90, 90,204,102,255,  2},
  { 50,130, 90, 70,110,180, 90, 90, 90, 90, 90, 90,204,102,255,  2},
  { 50,130, 90, 70, 70,180, 90, 90, 90, 90, 90, 90,204,102,255,  2},
  { 50,130, 90, 70,110,180, 90, 90, 90, 90, 90, 90,204,102,255,  2},
  { 50,130, 90, 70, 70,180, 90, 90, 90, 90, 90, 90,204,102,255,  2},
  { 90, 90,  0, 90, 90,180, 90, 90, 90, 90, 90, 90,204,102,255,  3},
  { 90, 90,  0, 90, 90,180, 90, 90, 90, 90, 90, 90,102,  0,153, 20}
},
{  // #M8 Red Bye-Bye quickly
  { 90, 60,  0, 90, 90, 60, 50, 90, 90, 90, 90, 90,255,  0,  0,  3},
  { 90, 60,  0, 90, 90, 60, 90, 90, 90, 90, 90, 90,255,  0,  0,  3},
  { 90, 90,  0, 90, 90,180, 90, 90, 90, 90, 90, 90,  0,  0,  0,  0},
  { 90, 90,  0, 90, 90,180, 90, 90, 90, 90, 90, 90,  0,  0,  0,  0},
  { 90, 90,  0, 90, 90,180, 90, 90, 90, 90, 90, 90,  0,  0,  0,  0},
  { 90, 90,  0, 90, 90,180, 90, 90, 90, 90, 90, 90,  0,  0,  0,  0},
  { 90, 90,  0, 90, 90,180, 90, 90, 90, 90, 90, 90,  0,  0,  0,  0},
  { 90, 90,  0, 90, 90,180, 90, 90, 90, 90, 90, 90,  0,  0,  0,  0}
},
{  // #M9 Right and Left
  { 90, 90,150, 70,110,  0,100, 70, 90, 85, 90, 85,255,151,111,  3},
  {160, 20,180,110,110,  0, 70, 70, 90, 85, 90, 85,255,151,111,  3},
  { 10, 20, 40,130,110,130, 50, 70, 90, 85, 90, 85,255,151,111,  3},
  { 10, 20, 40,130,110,130, 50, 70, 90, 85, 90, 85,255,151,111,  3},
  { 20,160,180, 70,110,  0,100, 70, 90, 85, 90, 85,255,151,111, 10},
  { 20,160, 40, 70,110,130,100, 70, 90, 85, 90, 85,255,151,111,  3},
  {160,160, 40, 70,110,130,100, 70, 90, 85, 90, 85,255,151,111,  3},
  { 90, 90,150, 70,110,  0,100, 70, 90, 85, 90, 85,255,151,111,  3}
},
{  // #MA, Eight colored Illumination
  { 90, 90,  0,130, 90,180, 50, 90, 90, 90, 90, 90,255,  0,  0, 10},
  { 90, 90,  0,130, 90,180, 50, 90, 90, 90, 90, 90,  0,255,  0, 10},
  { 90, 90,  0,130, 90,180, 50, 90, 90, 90, 90, 90,  0,  0,255, 10},
  { 90, 90,  0,130, 90,180, 50, 90, 90, 90, 90, 90,255,255,  0, 10},
  { 90, 90,  0,130, 90,180, 50, 90, 90, 90, 90, 90,  0,255,255, 10},
  { 90, 90,  0,130, 90,180, 50, 90, 90, 90, 90, 90,255,  0,255, 10},
  { 90, 90,  0,130, 90,180, 50, 90, 90, 90, 90, 90,255,255,255, 10},
  { 90, 90,  0,130, 90,180, 50, 90, 90, 90, 90, 90,  0,  0,  0, 10}
}
};

void setup()  {
  servo[0].attach(10);   // Head yaw
  servo[1].attach(11);   // Waist yaw
  servo[2].attach(9);    // R Sholder roll
  servo[3].attach(8);    // R Sholder pitch
  servo[4].attach(7);    // R Hand grip
  servo[5].attach(12);   // L Sholder roll
  servo[6].attach(13);   // L Sholder pitch
  servo[7].attach(14);   // L Hand grip
  servo[8].attach(4);    // R Foot yaw
  servo[9].attach(2);    // R Foot pitch
  servo[10].attach(15);  // L Foot yaw
  servo[11].attach(16);  // L Foot pitch
  eyes[R] = 6;           // Red LED of eyes
  eyes[G] = 5;           // Green LED of eyes
  eyes[B] = 3;           // Blue LED of eyes
  
  for( i = 0; i < MAXSN; i++) {
    targetAngle[i] = motion[0][0][i] << SHIFT;
    nowAngle[i] = targetAngle[i];
    servo[i].write((nowAngle[i] >> SHIFT) + trim[i]);
  }
  for(i = 0; i < 3; i++) {
    targetBright[i] = 0 << SHIFT;
    nowBright[i] = targetBright[i];
    analogWrite(eyes[i], nowBright[i] >> SHIFT);
  }
  
  Serial.begin(57600);
  
  delay(500);
  
  pinMode(POWER, OUTPUT);
  digitalWrite(POWER, HIGH);
} 

void loop()  {
  int buf = ERR;
  if(Serial.available()) {
    if(Serial.read() == '#') {
      while(!Serial.available()){}
      switch(Serial.read()) {
        case 'M':
          buf = readOneDigit();
          if(buf != ERR){
            motionNumber = buf;
            if(buf == 17) {
              motionNumber = 10;
            }
            mode = 'M';
            digitalWrite(POWER, HIGH);
            Serial.print("#M");
            Serial.print(motionNumber);
          } else {
            Serial.print("#EM");
          }
        break;
        case 'P':
          buf = getPose();
          if(buf != ERR) {
            mode = 'P';
            digitalWrite(POWER, HIGH);
            Serial.print("#PT");
            printThreeDigit(buf);
          } else {
            Serial.print("#EP");
          }
        break;
        case 'Q':
          Serial.print("#Q");
          if(mode == 'M') {
            Serial.print("M");
            Serial.print(motionNumber);
            Serial.print("T");
            buf = (endTime-millis()) /100;
            if(buf < 0) { buf = 0;}
            printThreeDigit(buf);
          }
          if(mode == 'P') {
            Serial.print("PT");
            buf = (endTime-millis()) /100;
            if(buf < 0) { buf = 0;}
            printThreeDigit(buf);
          }
        break;
        case 'C':
          Serial.print("#C");
          if(bufferTime > 0) {
            Serial.print("F");
          } else {
            Serial.print("0");
          }
        break;
        case 'X':
          Serial.print("#X");
          digitalWrite(POWER, LOW);
        break;
        default:
          Serial.print("#E");
        break;
      }
    }
  }
  if(endTime > millis()) {
    remainingTime = (endTime - millis()) / 10;
    for( i = 0; i < MAXSN; i++) {
      nowAngle[i] = targetAngle[i] - (deltaAngle[i] * remainingTime);
      servo[i].write((nowAngle[i] >> SHIFT) + trim[i]);
    }
    for( i = 0; i < 3; i++) {
      nowBright[i] = targetBright[i] - (deltaBright[i] * remainingTime);
      analogWrite(eyes[i], nowBright[i] >> SHIFT);
    }
  } else if(mode == 'M') {
    nextFrame();
  } else if(mode == 'P') {
    if(bufferTime > 0){
      nextPose();
    } else if(endTime + 500 < millis()){
      //digitalWrite(POWER, LOW);
    }
  }
}

//Motion Play
void nextFrame() {
  frameNumber++;
  if(frameNumber >= MAXFN) {
    frameNumber = 0;
  }
  for(i = 0; i < MAXSN; i++) {
    bufferAngle[i] = motion[motionNumber][frameNumber][i];
  }
  for( i = 0; i < 3; i++) {
    bufferBright[i] = motion[motionNumber][frameNumber][MAXSN+i];
  }
  bufferTime = motion[motionNumber][frameNumber][TIME];
  
  nextPose();
}

//Make a pose
int nextPose() {
  if(bufferTime > 0) {
    for(i = 0; i < MAXSN; i++) {
      targetAngle[i] = bufferAngle[i] << SHIFT;
      deltaAngle[i] = ((bufferAngle[i] << SHIFT) - nowAngle[i]) / (bufferTime * 10);
    }
    for( i = 0; i < 3; i++) {
      targetBright[i] = bufferBright[i] << SHIFT;
      deltaBright[i] = ((bufferBright[i] << SHIFT) - nowBright[i]) / (bufferTime * 10);
    }
  } else {
    for(i = 0; i < MAXSN; i++) {
      deltaAngle[i] = 0;
    }
    for(i = 0; i < 3; i++) {
      deltaBright[i] = 0;
    }
  }
  startTime = millis();
  endTime = startTime + (bufferTime * 100);
  bufferTime = 0;
}

//get buffer values of the next pose from serial data
int getPose() {
  int buf = 0;
  int value = 0;
  int maximum = 255;
  boolean readPose = true;
  if(bufferTime == 0) {
    //Initialize array to target angle
    for(i = 0; i < MAXSN; i++) {
      tempAngle[i] = bufferAngle[i];
    }
    for( i = 0; i < 3; i++) {
      tempBright[i] = bufferBright[i];
    }
  } else {
    buf = ERR;
    readPose = false;
  }
  //Read data
  while(readPose) {
    while(!Serial.available()) {}
    switch(Serial.read()) {
      case 'S':
        buf = readOneDigit();
        if(buf != ERR) {
          value = buf *10;
          buf = readOneDigit();
          if(buf != ERR) {
            value += buf;
            if(0 <= value && value < MAXSN) {
              while(!Serial.available()) {}
              if(Serial.read() == 'A') {
                maximum = 180;
                buf = readThreeDigit(maximum);
                if(buf != ERR) {
                  tempAngle[value] = buf;
                } else {
                  readPose = false;
                }
              } else {
                buf = ERR;
                readPose = false;
              }
            } else {
              buf = ERR;
              readPose = false;
            }
          }
        }
        break;
      case 'R':
        maximum = 255;
        buf = readThreeDigit(maximum);
        if(buf != ERR) {
          tempBright[R] = buf;
        } else {
          readPose = false;
        }
        break;
      case 'G':
        maximum = 255;
        buf = readThreeDigit(maximum);
        if(buf != ERR) {
          tempBright[G] = buf;
        } else {
          readPose = false;
        }
        break;
      case 'B':
        maximum = 255;
        buf = readThreeDigit(maximum);
        if(buf != ERR) {
          tempBright[B] = buf;
        } else {
          readPose = false;
        }
        break;
      case 'T':
        maximum = 255;
        buf = readThreeDigit(maximum);
        if(buf > 0) {
          bufferTime = buf;
          for(i = 0; i < MAXSN; i++){
            bufferAngle[i] = tempAngle[i];
          }
          for( i = 0; i < 3; i++) {
            bufferBright[i] = tempBright[i];
          }
        }
        readPose = false;
        break;
      default:
        buf = ERR;
        readPose = false;
        break;
    }
  }
  return buf;
}

int printThreeDigit(int buf) {
  String s = String(buf);
  if(s.length() == 2){
    Serial.print("0");
  } else if (s.length() == 1) {
    Serial.print("00");
  }
  Serial.print(s);
}

int digit;
//Read ASCII Three-digit
int readThreeDigit(int maximum) {
  int buf;
  buf = readOneDigit();
  if(buf != ERR) {
    digit = buf * 100;
    buf = readOneDigit();
    if(buf != ERR) {
      digit += buf * 10;
      buf = readOneDigit();
      if(buf != ERR) {
        digit += buf;
        if(digit <= maximum) {
          buf = digit;
        } else {
          buf = ERR;
        }
      }
    }
  }
  return buf;
}

//Read ASCII One-digit
int readOneDigit() {
  int buf;
  while(!Serial.available()) {}
  buf = Serial.read() - 48;
  if(buf < 0 || 9 < buf){
    if(buf!=17){
      buf = ERR;
    }
  }
  return buf;
}
