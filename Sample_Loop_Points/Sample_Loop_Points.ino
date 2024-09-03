/*  Example playing a looped sampled sound while
    changing the start end end loop points,
    and sliding the frequency around,
    using Mozzi sonification library.

    Demonstrates Sample(), looping with random
    changes to loop and frequency parameters
    scheduled with EventDelay.

    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.io/Mozzi/

		Mozzi documentation/API
		https://sensorium.github.io/Mozzi/doc/html/index.html

		Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2012, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include <Sample.h> // Sample template
#include "/home/torsten/src/mozzi-noizetoyz/truck1.h"
#include <EventDelay.h>
#include <mozzi_rand.h>

#define CONTROL_RATE 128

// use: Sample <table_size, update_rate> SampleName (wavetable)
Sample <TRUCK_NUM_CELLS, AUDIO_RATE> aSample(TRUCK_DATA);

// for scheduling changes
EventDelay  kTriggerDelay;


const float playspeed = 0.4;
float playspeedmod = 0;
float speedchange = 0;

const unsigned int full = (int) (1000.f/playspeed) - 23; // adjustment approx for CONTROL_RATE difference

void setup(){
  randSeed(); // reseed the random generator for different results each time the sketch runs
  aSample.setFreq(playspeed*((float) TRUCK_SAMPLERATE / (float) TRUCK_NUM_CELLS));
  kTriggerDelay.set(full);
  aSample.setLoopingOn();
  startMozzi(CONTROL_RATE);
}


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


void updateControl(){
  if(kTriggerDelay.ready()){
    chooseStartEnd();
    chooseSpeedMod();
    kTriggerDelay.start();
  }
  playspeedmod += speedchange;
  aSample.setFreq(playspeedmod);
}


AudioOutput_t updateAudio(){
  return MonoOutput::from8Bit(aSample.next());
}


void loop(){
  audioHook();
}
