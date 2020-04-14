/* analog_sample_average_led
 *
 * By: Andrew Tuline
 *
 * Updated: Feb, 2019
 *
 * Basic code to read from the Sparkfun 3.3V ADMP401 microphone and provide average as well as peak detection.
 * 
 * If your microphone also uses 3.3V, then please read up on analogReference(EXTERNAL) in setup().
 * 
 * You may also need to adjust the DC_OFFSET to be the value of the microphone when all is silent.
 * This is the place to test and find out.
 *
 * Resultant fps is 8300 or about a 8KHz sampling rate. Slow, but adequate for general use. . .
 * 
 * I should really learn some math and take a signal processing course.
 * 
 */


#include "FastLED.h"                                          // FastLED library.

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define MIC_PIN    5                                          // Analog port for microphone

// Fixed definitions cannot change on the fly.
#define LED_DT 12                                             // Data pin to connect to the strip.
#define LED_CK 11                                             // Clock pin for WS2801 or APA102.
#define COLOR_ORDER BGR                                       // It's GRB for WS2812 and BGR for APA102.
#define LED_TYPE APA102                                       // Using APA102, WS2812, WS2801. Don't forget to modify LEDS.addLeds to suit.
#define NUM_LEDS 40                                           // Number of LED's.

// Initialize changeable global variables.
uint8_t max_bright = 128;                                     // Overall brightness definition. It can be changed on the fly.

struct CRGB leds[NUM_LEDS];                                   // Initialize our LED array.


// Global variable(s) used by other routines.
bool   samplepeak = 0;                                        // We'll call this our 'peak' flag.
uint8_t sampleavg = 0;                                        // Average of the last NSAMPLES samples.
uint8_t   sample = 10;                                        // Dampened 'sample' value from our twitchy microphone.



void setup() {

  analogReference(EXTERNAL);                                  //  Audio samples are 'cleaner' at 3.3V. COMMENT OUT THIS LINE FOR 3.3V ARDUINOS (FLORA, ETC.) or for 5V microphones.
  Serial.begin(115200);                                        // Initialize serial port for debugging.

//  LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(leds, NUM_LEDS);        // Use this for WS2812B.
  LEDS.addLeds<LED_TYPE, LED_DT, LED_CK, COLOR_ORDER>(leds, NUM_LEDS);  // Use this for WS2801 or APA102.

  FastLED.setBrightness(max_bright);                          // Setting our global maxiumum brightness. We can change this on the fly if we want.
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);               // Power managed display so that we don't draw too much current.

} // setup()



void loop() {

//  showfps();
  analog_sample_average();
  crappy_display();
  FastLED.show();

} // loop()



void analog_sample_average() {
  
// Local definitions
  #define NSAMPLES 32                                           // Creating an array with lots of samples for decent averaging.
  #define SQUELCH 10                                            // Noise squelching.
  #define DC_OFFSET  509                                        // DC offset in mic signal. Should be about 512.
  #define MAXVOL 7                                              // Amount higher than sampleavg is what we predict is max volume. Higher is a peak.
  
// Persistent local variables
  static uint8_t samplearray[NSAMPLES];                         // Array of samples for general sampling average.
  static uint16_t samplesum = 0;                                // Sum of the last 64 samples. This had better be positive.
  static uint8_t samplecount = 0;                               // A rollover counter to cycle through the circular buffer of samples.
  static long peaktime;                                         // Time of last peak, so that they're not too close together.

// Temporary local variables
  int16_t    micIn = 0;                                         // Current sample starts with negative values and large values, which is why it's 16 bit signed.


  micIn = analogRead(MIC_PIN) - DC_OFFSET;                      // Sample the microphone. Range will result in -512 to 512. Boy, it sure is twitchy.
  micIn = abs(micIn);                                           // Get the absolute value of that.
  micIn   = (micIn <= SQUELCH) ? 0 : (micIn - SQUELCH);         // Remove noise/hum.

//  sample = ((sample * 7) + micIn) >> 3;                       // Very dampened reading.
  sample = ((sample * 3) + micIn) >> 2;                         // Somewhat dampened reading, which is good enough for us.

  if (sample < sampleavg+MAXVOL) samplepeak = 0;                // Reset the global sample peak only if we're below MAXVOL.

// Let's populate an array (circular buffer) in order to calculate an average value across NSAMPLES.
  samplesum += sample - samplearray[samplecount];               // Add the new sample and remove the oldest sample in the array. No 'for' loops required here for extra speed.
  sampleavg = samplesum / NSAMPLES;                             // Get an average.
  samplearray[samplecount] = sample;                            // Update oldest sample in the array with new sample. By Andrew Tuline.
  samplecount = (samplecount + 1) % NSAMPLES;                   // Update the counter for the array and rollover if we hit the max.

  if (  sample > (sampleavg+MAXVOL)                                                     &&      // Keep above a floor value. 
        sample < samplearray[(samplecount+30)%NSAMPLES]                                 &&      // Is it < previous sample.
        samplearray[(samplecount+30)%NSAMPLES] > samplearray[(samplecount+29)%NSAMPLES] &&      // Is previous sample > sample before that.
        millis() > (peaktime + 50)                                                      &&      // Wait at least 200ms for another peak.
        samplepeak == 0                                                                         // and there wasn't a recent peak.
        ) {samplepeak = 1;peaktime=millis();} else {samplepeak = 0;}                            // Then we got a peak, else we don't.

/*
  Serial.print(micIn);
  Serial.print(" ");
  Serial.print(sample);
  Serial.print(" ");
  Serial.print(sampleavg+MAXVOL);
  Serial.print(" ");
  Serial.print(samplepeak*128);
  Serial.print(" ");
  Serial.print(0);
  Serial.print(" ");
  Serial.print(128);
  Serial.println(" ");
*/

} // analog_sample_average()



void crappy_display() {

  if (samplepeak) leds[random8(NUM_LEDS)] = CHSV(0,0,255);      // Add a randomly located twinkle if we have a peak.
  
  leds[0] = CHSV(sampleavg*3, 255, sample*3); 
  nblend(leds[0], CHSV(sample,255,sample), 128);                // Don't change the colour of led[0] too fast.

  EVERY_N_MILLIS(20) {
    fadeToBlackBy(leds, NUM_LEDS, 2);                           // Fade the whole strand.
    for (int i = NUM_LEDS-1; i>=1; i--) {                       // Now, propagate it down the line like in 'The Matrix', peaks and all.
      leds[i] = leds[i-1];
    }
  }

} // crappy_display()



void showfps() {                                                // Show rames per seocond on the serial monitor.

  long currentMillis = 0;                                       // Variables used by our fps counter.
  static long lastMillis = 0;
  static long loops = 0;

  currentMillis=millis();
  
  loops++;
  if(currentMillis - lastMillis >1000) {                        // OK, we've waited a second, so . . 
    Serial.println(loops);                                      // Print it once a second.
    lastMillis = currentMillis;                                 // And reset our time counter.
    loops = 0;                                                  // And reset the number of loops counter.
  }
  
} // showfps()
