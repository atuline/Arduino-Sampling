/* poll_array
 *
 * By: Andrew Tuline
 *
 * Updated: Feb, 2019
 *
 * High speed polled code Basic code to read from the Sparkfun ADMP401 microphone and provide average as well as peak detection.
 * 
 * If your microphone uses 3.3V, then please read up on analogReference(EXTERNAL) in the setup().
 * 
 * You may also need to adjust the DC_OFFSET to be the value of the microphone when all is silent. Test and find out.
 *
 * 
 */

#define FHT_N 256 // set to 64 elements

#define DC_OFFSET  509                                        // DC offset in mic signal. Should probably be about 512.
#define MIC_PIN 5

// Global variable(s) used by other routines.
bool   samplepeak = 0;                                        // We'll call this our 'peak' flag.
uint8_t sampleavg = 0;                                        // Average of the last NSAMPLES samples.
uint8_t   sample = 10;                                        // Dampened 'sample' value from our twitchy microphone.



void setup() {
  Serial.begin(57600); // use the serial port

// Setup the ADC for polled 10 bit sampling on analog pin 5 at 38.5KHz.
  cli();                                  // Disable interrupts.
  ADCSRA = 0;                             // Clear this register.
  ADCSRB = 0;                             // Ditto.
  ADMUX = 0;                              // Ditto.
  ADMUX |= (MIC_PIN & 0x07);              // Set A5 analog input pin.
  ADMUX |= (0 << REFS0);                  // Set reference voltage  (analog reference(external), or using 3.3V microphone on 5V Arduino.
                                          // Set that to 1 if using 5V microphone or 3.3V Arduino.
//  ADMUX |= (1 << ADLAR);                  // Left justify to get 8 bits of data.                                          
  ADMUX |= (0 << ADLAR);                  // Right justify to get full 10 A/D bits.

//  ADCSRA |= bit (ADPS0) | bit (ADPS2);                //  32 scaling or 38.5 KHz sampling
//  ADCSRA |= bit (ADPS1) | bit (ADPS2);                //  Set ADC clock with 64 prescaler where 16mHz/64=250kHz and 250khz/13 instruction cycles = 19.2khz sampling.
  ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);    // 128 prescaler with 9.6 KHz sampling
  
  ADCSRA |= (1 << ADATE);                 // Enable auto trigger.
//  ADCSRA |= (1 << ADIE);                  // Enable interrupts when measurement complete (if using ISR method). Sorry, we're using polling here.
  ADCSRA |= (1 << ADEN);                  // Enable ADC.
  ADCSRA |= (1 << ADSC);                  // Start ADC measurements.
  sei();                                  // Re-enable interrupts.
  
}


void loop() {

//  showfps();
  poll_array();

} // loop()





void poll_array() {

// Local definitions
  #define NSAMPLES 32                                           // Creating an array with lots of samples for decent averaging.
  #define SQUELCH 10                                            // Noise squelching.
  #define DC_OFFSET  509                                        // DC offset in mic signal. Should be about 512.
  #define MAXVOL 7                                              // Amount higher than sampleavg is what we predict is max volume. Higher than that is a peak.
  
// Persistent local variables
  static uint8_t samplearray[NSAMPLES];                         // Array of samples for general sampling average.
  static uint16_t samplesum = 0;                                // Sum of the last 64 samples. This had better be positive.
  static uint8_t samplecount = 0;                               // A rollover counter to cycle through the circular buffer of samples.
  static long peaktime;                                         // Time of last peak, so that they're not too close together.

// Temporary local variables
  int16_t    micIn = 0;                                         // Current sample starts with negative values and large values, which is why it's 16 bit signed.



  while(!(ADCSRA & 0x10));                                    // wait for adc to be ready
  ADCSRA = 0xf5;                                              // restart adc
  
  micIn = ADC - DC_OFFSET;                                    // Get the data from the ADC and subtract the DC Offset.
  micIn = abs(micIn);                                         // Get the absolute value of that.
  micIn   = (micIn <= SQUELCH) ? 0 : (micIn - SQUELCH);       // Remove noise/hum.
    

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
 
}


void showfps() {                                                // Show rames per seocond on the serial monitor.

  long currentMillis = 0;                                       // Variables used by our fps counter.
  static long lastMillis = 0;
  static long loops = 0;

  currentMillis=millis();
  
  loops++;
  if(currentMillis - lastMillis >1000) {
    Serial.println(loops);                                      // Print it once a second.
    lastMillis = currentMillis;
    loops = 0;
  }
} // showfps()
