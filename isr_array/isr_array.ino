/* isr_array
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
 * 77 fps at 19KHz
 * 151 fps at 38KHz
 */

#define FHT_N 256 // set to 64 elements

#define DC_OFFSET  509                                        // DC offset in mic signal. Should probably be about 512.
#define MIC_PIN 5

// Global variable(s) used by other routines.
bool   samplepeak = 0;                                        // We'll call this our 'peak' flag.
uint8_t sampleavg = 0;                                        // Average of the last NSAMPLES samples.
uint8_t   sample = 10;                                        // Dampened 'sample' value from our twitchy microphone.



#define NSAMPLES 256

volatile int16_t samplearray [NSAMPLES];
volatile int16_t samplecount;




void setup() {
  
  Serial.begin(57600); // use the serial port

// Setup the ADC for ISR 10 bit sampling on analog pin 5 at 38.5KHz.
  cli();                                  // Disable interrupts.
  ADCSRA = 0;                             // Clear this register.
  ADCSRB = 0;                             // Ditto.
  ADMUX = 0;                              // Ditto.
  ADMUX |= (MIC_PIN & 0x07);              // Set A5 analog input pin.
  ADMUX |= (0 << REFS0);                  // Set reference voltage  (analog reference(external), or using 3.3V microphone on 5V Arduino.
                                          // Set that to 1 if using 5V microphone or 3.3V Arduino.
//  ADMUX |= (1 << ADLAR);                  // Left justify to get 8 bits of data.
  ADMUX |= (0 << ADLAR);                  // Right justify to get 10 bits of data.  
//  ADCSRA |= (1 << ADPS2) | (1 << ADPS1);  // Set ADC clock with 64 prescaler where 16mHz/64=250kHz and 250khz/13 instruction cycles = 19.2khz sampling.
 ADCSRA |= (1 << ADPS2) | (1 << ADPS0);   // Set ADC clock with 32 prescaler for 38.5 KHz sampling.
  ADCSRA |= (1 << ADATE);                 // Enable auto trigger.
  ADCSRA |= (1 << ADIE);                  // Enable interrupts when measurement complete (if using ISR method). Sorry, we're using polling here.
  ADCSRA |= (1 << ADEN);                  // Enable ADC.
  ADCSRA |= (1 << ADSC);                  // Start ADC measurements.
  sei(); 
  
}


void loop() {

 showfps();
  isr_array();

} // loop()


ISR(ADC_vect) {

  #define DC_OFFSET  509                                        // DC offset in mic signal. Should probably be about 512.

  if (samplecount >= NSAMPLES)
    ADCSRA |= (0 << ADEN);                                      // Disable ADC.    
  else
    samplearray[samplecount++] = ADC - DC_OFFSET;               // Taking in the raw data
 
} // ISR()



void isr_array() {

  while (samplecount < NSAMPLES);                                 // Wait for buffer to fill.

/*  Serial.println("Starting a new array: ");                       // Optionally print out the now filled array.
  for (int i = 0; i < NSAMPLES; i++)
    Serial.println (samplearray[i]);
*/

  samplecount = 0;                                                // Reset the counter.
  ADCSRA |= (1 << ADEN);                                          // Re-enable the ADC.

// Now we can process the array . . . functionality which we don't have at the moment.

} // isr_array()



void showfps() {                                                // Show rames per second on the serial monitor.

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
