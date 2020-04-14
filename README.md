# Arduino Sampling


**By:** Andrew Tuline

**Email:** atuline@gmail.com

**Date:** March, 2019

**Updated:** April, 2020

This repository contains several examples of A/D conversion of audio input using:

- Arduino Nano
- Sparkfun ADMP401 MEMS microphone


In April, 2020, I added a .pdf file containing the results of my testing of various microphones.


Various routines also utilize the following libraries:

- http://wiki.openmusiclabs.com/wiki/ArduinoFFT
- http://wiki.openmusiclabs.com/wiki/ArduinoFHT
- https://github.com/FastLED/FastLED


Examples include:

10 bit ADC port sampling for use with the FHT calculations.
8 bit ADCH port sampling.
analogRead for use with general sound routines.

For general sampling, averaging and peak detection, we seem to be OK with analogRead.


Here's some cool references:

https://www.instructables.com/id/Arduino-Frequency-Detection/
https://sites.google.com/site/qeewiki/books/avr-guide/analog-input
http://yaab-arduino.blogspot.com/2015/02/fast-sampling-from-analog-input.html
https://www.instructables.com/id/Girino-Fast-Arduino-Oscilloscope/
https://www.gammon.com.au/adc
http://www.robotplatform.com/knowledge/ADC/adc_tutorial.html
https://www.newbiehack.com/MicrocontrollersADC10Bits.aspx


-----------------------------------------------------------------------------------

This repository:						https://github.com/atuline/Arduino-Sampling

My Youtube channel is at:               https://www.youtube.com/user/atuline/videos
My Github account is at:                https://github.com/atuline
My Gists are at:                        https://gist.github.com/atuline
My Pastebins are at:                    https://pastebin.com/u/atuline
FastLED support forum:					https://www.reddit.com/r/fastled