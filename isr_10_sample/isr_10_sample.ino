
#define MIC_PIN 5                                             // Using A5. Change as required.


volatile int numSamples=0;
long t, t0;
volatile int16_t sample;


void setup() {
  
  Serial.begin(57600);

// Setup the ADC for ISR 10 bit sampling on analog pin 5 at 19.2kHz.
  cli();                                  // Disable interrupts.
  ADCSRA = 0;                             // Clear this register.
  ADCSRB = 0;                             // Ditto.
  ADMUX = 0;                              // Ditto.
  ADMUX |= (MIC_PIN & 0x07);              // Set A5 analog input pin.
  ADMUX |= (0 << REFS0);                  // Set reference voltage  (analog reference(external), or using 3.3V microphone on 5V Arduino.
                                          // Set that to 1 if using 5V microphone or 3.3V Arduino.
//  ADMUX |= (1 << ADLAR);                  // Left justify to get 8 bits of data.
  ADMUX |= (0 << ADLAR);                  // Right justify to get 10 bits of data.  
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1);  // Set ADC clock with 64 prescaler where 16mHz/64=250kHz and 250khz/13 instruction cycles = 19.2khz sampling.
// ADCSRA |= (1 << ADPS2) | (1 << ADPS0);   // Set ADC clock with 32 prescaler for 38.5 KHz sampling.
  ADCSRA |= (1 << ADATE);                 // Enable auto trigger.
  ADCSRA |= (1 << ADIE);                  // Enable interrupts when measurement complete (if using ISR method). Sorry, we're using polling here.
  ADCSRA |= (1 << ADEN);                  // Enable ADC.
  ADCSRA |= (1 << ADSC);                  // Start ADC measurements.
  sei();                                  // Re-enable interrupts.
  
} // setup()



ISR(ADC_vect) {

  #define DC_OFFSET  509                                        // DC offset in mic signal. Should probably be about 512.

  sample = ADC - DC_OFFSET;                     // A 10 bit A/D, combined with ADLAR of 0 means we just subtract the DC_OFFSET.

/*
  Serial.print(sample);
  Serial.print(" ");
  Serial.print(0);
  Serial.print(" ");
  Serial.println(512);
  Serial.println(" ");
*/

  numSamples++;

} // ISR()


  
void loop() {

//  showfps();
  
  if (numSamples>=1000) {
    
    t = micros()-t0;  // calculate elapsed time

/*    Serial.print("Sampling frequency: ");
    Serial.print((float)1000000/t);
    Serial.println(" KHz");
    delay(2000);
*/
    
    t0 = micros();                                            // Restart the count.
    numSamples=0;
  }
} // loop()



void showfps() {                                              // Show rames per seocond on the serial monitor.

  long currentMillis = 0;                                     // Variables used by our fps counter.
  
  static long lastMillis = 0;
  static long loops = 0;

  currentMillis=millis();
  
  loops++;
  if(currentMillis - lastMillis >1000) {
    Serial.println(loops);                                  // Print it once a second.
    lastMillis = currentMillis;
    loops = 0;
  }
} // showfps()


