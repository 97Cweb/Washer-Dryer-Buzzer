
const bool DEBUG=true;
const int lightPin = A0;
const int dryerSensorPin = A1;
const int brightnessThreshold=140;

bool dryerOn=false;
bool dryerWasOn=false;
bool washerDone=false;
bool washAlarm=false;

const int numVreadings=200;
int Vreadings[numVreadings];

  
void setup(){
  Serial.begin(9600);

  for(int i=0;i<numVreadings;i++){
    Vreadings[i]=0;
  }
  Serial.println("Start");

  tone(5,640,250);
  delay(500);
  tone(5,640,250);
  delay(500);

}
void loop(){
  updateDryer();
  updateWasher();  
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
  
  if (rmsAvg>200.0){
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
    tone(5,512,2000);
    delay(4000);
    tone(5,512,2000);
    delay(4000);
    tone(5,512,2000);
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
    //washAlarm=true;
    tone(5,640,2000);
    delay(4000);
    tone(5,640,2000);
    delay(4000);
    tone(5,640,2000);
    delay(2000);
  }
}
