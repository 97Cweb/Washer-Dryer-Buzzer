//need to set serial to 1200 for upload to work

const bool DEBUG=true;
const int dryerSensorPin = A1;
const int washerNotHome1Pin = A2;
const int washerNotHome2Pin = A3;
const int resetPin = A4;
const int speakerPin = 5;

bool dryerOn=false;
bool washerOn = false;
bool dryerDone = false;
bool washerDone = false;


const int numVreadings=200;

int dryerReadingCount = 0;
float dryerSquareSum = 0;

bool dryerReadingReady = false;
bool latestDryerState = false;

unsigned long lastDryerSampleTime = 0;
const unsigned long dryerSampleInterval = 1;


unsigned long lastBeepTime = 0;
const unsigned long beepDuration = 2000;
const unsigned long beepInterval = 4000;

bool beepWasherNext = true;

  
void setup(){
  Serial.begin(9600);

  pinMode(washerNotHome1Pin, INPUT);
  pinMode(washerNotHome2Pin, INPUT);
  pinMode(resetPin,INPUT_PULLUP);

  Serial.println("Start");

}
void loop() {
  if (!digitalRead(resetPin)) {
    reset();
    return;
  }

  updateDryerSensor();
  updateDryer();
  updateWasher();
  updateBeeper();
}


void updateBeeper(){
  unsigned long now = millis();

  if (now - lastBeepTime < beepInterval){
    return;
  }

  if (washerDone && dryerDone){
    if(beepWasherNext){
      tone(speakerPin,640,beepDuration);
    }
    else{
      tone(speakerPin,512,beepDuration);
    }
    beepWasherNext = ! beepWasherNext;
    lastBeepTime = now;
  }
  else if (washerDone){
    tone(speakerPin,640,beepDuration);
    lastBeepTime = now;
  }
  else if (dryerDone){
    tone (speakerPin,512, beepDuration);
    lastBeepTime = now;
  }
}


void updateDryerSensor() {
  unsigned long now = millis();

  if (now - lastDryerSampleTime < dryerSampleInterval) {
    return;
  }

  lastDryerSampleTime = now;

  int reading = analogRead(dryerSensorPin);

  dryerSquareSum += sq((float)reading);
  dryerReadingCount++;

  if (dryerReadingCount >= numVreadings) {
    float rmsAvg = sqrt(dryerSquareSum / numVreadings);

    latestDryerState = rmsAvg > 200.0;
    dryerReadingReady = true;

    if (DEBUG) {
      Serial.println("RMS: " + String(rmsAvg));
    }

    dryerSquareSum = 0;
    dryerReadingCount = 0;
  }
}

void updateDryer() {
  if (!dryerReadingReady) {
    return;
  }

  dryerReadingReady = false;

  bool newDryerOn = latestDryerState;

  if (DEBUG) {
    Serial.println("Dryer " + String(newDryerOn));
  }

  if (dryerOn && !newDryerOn) {
    dryerDone = true;
    lastBeepTime = millis() - beepInterval;
  }

  dryerOn = newDryerOn;
}

void updateWasher(){
  bool washer1NotHome = digitalRead(washerNotHome1Pin); 
  bool washer2NotHome = digitalRead(washerNotHome2Pin);
  bool newWasherOn =  washer1NotHome && washer2NotHome;
  if (washerOn && !newWasherOn){
    washerDone = true;
    lastBeepTime = millis() - beepInterval;
    Serial.println("Washer done");
  }
  washerOn = newWasherOn;
}

void reset() {
  dryerDone = false;
  washerDone = false;

  noTone(speakerPin);
  Serial.println("reset");
  lastBeepTime = millis();
  beepWasherNext = true;
}
