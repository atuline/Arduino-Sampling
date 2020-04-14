/* analog_sample_average
 *
 * By: Andrew Tuline
 *
 * Updated: Feb, 2019
 *
 * Basic code to read from the Sparkfun ADMP401 microphone and provide average as well as peak detection.
 * 
 * If your microphone uses 3.3V, then please read up on analogReference(EXTERNAL) in the setup().
 * 
 * You may also need to adjust the DC_OFFSET to be the value of the microphone when all is silent. Test and find out.
 *
 * Resultant fps is 8300 or about a 8KHz sampling rate. Slow. . .
 * 
 */

#define MIC_PIN    5                                          // Analog port for microphone

// Global variable(s) used by other routines.
bool   samplepeak = 0;                                        // We'll call this our 'peak' flag.
uint8_t sampleavg = 0;                                        // Average of the last NSAMPLES samples.
uint8_t   sample = 10;                                        // Dampened 'sample' value from our twitchy microphone.


void setup() {

  analogReference(EXTERNAL);                                  //  Audio samples are 'cleaner' at 3.3V. COMMENT OUT THIS LINE FOR 3.3V ARDUINOS (FLORA, ETC.) or for 5V microphones.
  Serial.begin(115200);                                        // Initialize serial port for debugging.

} // setup()



void loop() {

  showfps();
  analog_sample_average();

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

  if (sample > (sampleavg+MAXVOL)                                                     &&      // Keep above a floor value. 
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



void showfps() {                                              // Show rames per seocond on the serial monitor.

  long currentMillis = 0;                                       // Variables used by our fps counter.
  static long lastMillis = 0;
  static long loops = 0;

  currentMillis=millis();
  
  loops++;
  if(currentMillis - lastMillis >1000) {
    Serial.println(loops);                                    // Print it once a second.
    lastMillis = currentMillis;
    loops = 0;
  }
} // showfps()
