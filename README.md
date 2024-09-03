# mozzi-noizetoyz

See sample.ino for info to create wavetable data (truck.h)

Tldr;

  sudo apt install sox
  sox truck.wav -b 8 -r 16384 truck.raw
  python3 /home/torsten/src/Mozzi/extras/python/char2mozzi.py truck.raw truck.h TRUCK 16384

More inrpiration by Arnov Sharma and his Neko Punk synth based on Mozzi
lib
- https://create.arduino.cc/projecthub/Arnov_Sharma_makes/neko-punk-synth-v2-d06f3e
- https://diyelectromusic.wordpress.com/2021/06/22/arduino-mozzi-sample-drum-machine/



