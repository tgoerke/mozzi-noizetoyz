/*  Example of playing a sampled sound,
    using Mozzi sonification library.

    Demonstrates one-shot samples scheduled
    with EventDelay.

    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.io/Mozzi/

		Mozzi documentation/API
		https://sensorium.github.io/Mozzi/doc/html/index.html

		Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2012, CC by-nc-sa.
*/

/* Mozzi noize toys Workshop
 *  record a short (up to 1.5 seconds) mono sample 
 *  with 16384 sample rate in Audacity
 *  save as "raw" audio file, 8-bit, signed PCM, Mono
 *  convert to mozzi table format:
 *    python3 ~/src/Mozzi/extras/python/char2mozzi.py truck.raw truck.h TRUCK 16384
 *  
 *  see comments in char2mozzi.py for details
 *  https://sensorium.github.io/Mozzi/doc/html/char2mozzi_8py.html
 *  
 *  
    

 */
#include <MozziGuts.h>
#include <Sample.h> // Sample template
#include "/home/torsten/src/mozzi-noizetoyz/truck.h"
#include <EventDelay.h>

// use: Sample <table_size, update_rate> SampleName (wavetable)
Sample <TRUCK_NUM_CELLS, AUDIO_RATE> aSample(TRUCK_DATA);

// for scheduling sample start
EventDelay kTriggerDelay;

void setup(){
  startMozzi();
  aSample.setFreq((float) TRUCK_SAMPLERATE / (float) TRUCK_NUM_CELLS); // play at the speed it was recorded
  kTriggerDelay.set(1500); // 1500 msec countdown, within resolution of CONTROL_RATE
}


void updateControl(){
  if(kTriggerDelay.ready()){
    aSample.start();
    kTriggerDelay.start();
  }
}


AudioOutput_t updateAudio(){
  return MonoOutput::from8Bit((int) aSample.next());
}


void loop(){
  audioHook();
}
