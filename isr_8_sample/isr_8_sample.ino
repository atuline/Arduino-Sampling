
#define MIC_PIN 5                                             // Using A5. Change as required.


volatile int numSamples=0;
long t, t0;
volatile int16_t sample;


void setup() {
  
  Serial.begin(57600);

  cli();
  
  ADCSRA = 0;             // clear ADCSRA register
  ADCSRB = 0;             // clear ADCSRB register
  ADMUX |= (MIC_PIN & 0x07);    // set A5 analog input pin
  ADMUX |= (0 << REFS0);  // set reference voltage, which is same as analog reference(external)
  ADMUX |= (1 << ADLAR);  // left align ADC value to 8 bits from ADCH register

//  ADCSRA |= bit (ADPS0) | bit (ADPS2);                //  32 scaling or 38.5 KHz sampling
  ADCSRA |= bit (ADPS1) | bit (ADPS2);                //  Set ADC clock with 64 prescaler where 16mHz/64=250kHz and 250khz/13 instruction cycles = 19.2khz sampling.
//  ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);    // 128 prescaler with 9.6 KHz sampling

  ADCSRA |= (1 << ADATE); // enable auto trigger
  ADCSRA |= (1 << ADIE);  // enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN);  // enable ADC
  ADCSRA |= (1 << ADSC);  // start ADC measurements

  sei();
  
} // setup()



ISR(ADC_vect) {

  #define DC_OFFSET  127                                        // DC offset in mic signal. Should probably be about 512.

  sample = (ADCH - DC_OFFSET);              // An 8 bit A/D, combined with ADLAR of 1, so ADCH has top 8 bits of the sample.
  
  Serial.print(abs(sample));
  Serial.print(" ");
  Serial.print(0);
  Serial.print(" ");
  Serial.println(127);
  Serial.println(" ");
 
  numSamples++;

} // ISR()


  
void loop() {

  showfps();
  
  if (numSamples>=1000) {
    
    t = micros()-t0;  // calculate elapsed time

/*    Serial.print("Sampling frequency: ");
    Serial.print((float)1000000/t);
    Serial.println(" KHz");

    delay(2000);
*/
    
    // restart
    t0 = micros();
    numSamples=0;
  }
} // loop()


void showfps() {                                              // Show rames per seocond on the serial monitor.

  long currentMillis = 0;                                       // Variables used by our fps counter.
  
  static long lastMillis = 0;
  static long loops = 0;

  currentMillis=millis();
  
  loops++;
  if(currentMillis - lastMillis >1000) {
//    Serial.println(loops);                                    // Print it once a second.
    lastMillis = currentMillis;
    loops = 0;
  }
} // showfps()


