#include <LowPower.h>

// MPU-6050 Short Example Sketch
//www.elegoo.com
//2016.12.9

#include<Wire.h>

const bool DEBUG=true;
const int MPU_addr=0x68;  // I2C address of the MPU-6050
const int lightPin = A0;
const int dryerSensorPin = A1;
const int brightnessThreshold=250;

bool dryerOn=false;
bool dryerWasOn=false;
bool washerDone=false;
bool washAlarm=false;

const int numVreadings=200;
int Vreadings[numVreadings];

  
void setup(){
  //setup serial communication with gyroscope
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);

  for(int i=0;i<numVreadings;i++){
    Vreadings[i]=0;
  }
  Serial.println("Start");
}
void loop(){
  updateDryer();
  updateWasher();  
  Serial.flush();
  LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);  
}



bool isDryerOn(){
  float sqSum=0;
  float rmsAvg=0;
  for(int i=0;i<numVreadings;i++){
    Vreadings[i]=analogRead(dryerSensorPin);
    delay(1);
  }
  //calculate RMS
  for(int i=0;i<numVreadings;i++){
    sqSum+=sq((float)Vreadings[i])/(float)numVreadings;
  }

  rmsAvg=sqrt(sqSum);

  if(DEBUG){
    Serial.println("RMS: "+String(rmsAvg));
    }
  
  if (rmsAvg>550.0){
    return true;
  }
  else{
    return false;
  }
}

void updateDryer(){
  if(dryerOn){
    dryerWasOn=true;
  }
  dryerOn=isDryerOn();
  
  if(DEBUG)  Serial.println("Dryer "+dryerOn);

  if(!dryerOn && dryerWasOn){
    tone(8,512,2000);
    delay(4000);
    tone(8,512,2000);
    delay(4000);
    tone(8,512,2000);
    delay(2000);
    dryerWasOn=false;
  }
  
}


void updateWasher(){
  int brightness=analogRead(lightPin);
  if(DEBUG)Serial.println("bright "+String(brightness));

  if(brightness>brightnessThreshold){
    if(!washerDone){
      if(DEBUG)Serial.println("Washer is DONE");
      washerDone=true;
      washAlarm=false;
    }
  }    
  else{
    washerDone=false;
  }
  if(!washAlarm&&washerDone){
    washAlarm=true;
    tone(8,640,2000);
    delay(4000);
    tone(8,640,2000);
    delay(4000);
    tone(8,640,2000);
    delay(2000);
  }
}
