#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h>
#include <RollingAverage.h>
#include <IntMap.h>
#include <EventDelay.h>
#include <Sample.h> // Sample template
#include "/home/torsten/src/mozzi-noizetoyz/truck.h"
#include <mozzi_rand.h>

// use: Sample <table_size, update_rate> SampleName (wavetable)
Sample <TRUCK_NUM_CELLS, AUDIO_RATE> aSample(TRUCK_DATA);

// for scheduling changes
EventDelay  kTriggerDelay;
EventDelay  kSirenDelay;

#define CONTROL_RATE 128
Oscil <2048, AUDIO_RATE> aSin(SIN2048_DATA);
Oscil <2048, CONTROL_RATE> kVib(SIN2048_DATA);

const int KNOB_PIN = 0; // set the input for the knob to analog pin 0
// for smoothing the control signals
// use: RollingAverage <number_type, how_many_to_average> myThing
RollingAverage <int, 16> kAverageF;

// min and max values of parameters to map AutoRanged analog inputs to
const int MIN_F = 0.f;
const int MAX_F = 120.f;

// Intmap is a pre-calculated faster version of Arduino's map, OK for pots
IntMap kMapF(0,1023,MIN_F,MAX_F);


// siren
int flag; // switch between playing low of high siren tone
float centre_freq = 400.0;
float depth = 0.f;

// truck
const float playspeed = 0.4;
float playspeedmod = 0;
float speedchange = 0;

const unsigned int full = (int) (1000.f/playspeed) - 23; // adjustment approx for CONTROL_RATE difference

unsigned int chooseStart(){
  return rand((unsigned int)TRUCK_NUM_CELLS);
}


unsigned int chooseEnd(unsigned int startpos){
  return rand(startpos, (unsigned int) TRUCK_NUM_CELLS);
}


void chooseSpeedMod(){
  if (rand((byte)3) == 0){
    speedchange = (float)rand((char)-100,(char)100)/8000;
    float startspeed = (float)rand((char)-100,(char)100)/100;
    playspeedmod = playspeed + startspeed;
  }
  else{
    speedchange = 0;
    playspeedmod = playspeed;
  }
}


void chooseStartEnd(){
  unsigned int s,e;
  if (rand((byte)2) == 0){
    s = chooseStart();
    e = chooseEnd(s);
  }
  else{
    s = 0;
    e = TRUCK_NUM_CELLS;
  }
  aSample.setStart(s);
  aSample.setEnd(e);
}



void setup(){
  Serial.begin(115200);
  // wait before starting Mozzi to receive analog reads, so AutoRange will not get 0
  delay(200);

  // Truck engine
  randSeed(); // reseed the random generator for different results each time the sketch runs
  aSample.setFreq(playspeed*((float) TRUCK_SAMPLERATE / (float) TRUCK_NUM_CELLS));
  kTriggerDelay.set(full);
  aSample.setLoopingOn();

  // Siren
  kSirenDelay.set(1000);  
  kVib.setFreq(0.5f);
  
  startMozzi(CONTROL_RATE);
}

void updateControlTruck(){
  if(kTriggerDelay.ready()){
    chooseStartEnd();
    chooseSpeedMod();
    kTriggerDelay.start();
  }
  playspeedmod += speedchange;
  aSample.setFreq(playspeedmod);
}


void updateControlSiren(){

  int pot = mozziAnalogRead(KNOB_PIN)+1;

  if(kSirenDelay.ready()) {
    flag = 1 - flag;
    if (pot<10) aSample.setLoopingOn(); else aSample.setLoopingOff();
    Serial.println(flag);
    if (flag==0) centre_freq = 300.0; else centre_freq = 400.0;
    kSirenDelay.start();
  }
  pot = kMapF(pot);
  float vibrato = pot/55 * kVib.next();
  aSin.setFreq(centre_freq+vibrato+pot);
}



void updateControl(){
  updateControlTruck();
  updateControlSiren();
}


AudioOutput_t updateAudioTruck(){
  return MonoOutput::from8Bit(aSample.next());
}

int updateAudioSiren(){
  return aSin.next();
}

int updateAudio() {
    return MonoOutput::fromAlmostNBit(9, updateAudioTruck() + updateAudioSiren() );

}
void loop(){
  audioHook();
}
