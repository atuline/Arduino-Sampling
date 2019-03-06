/* analog_sample
 *
 * By: Andrew Tuline
 *
 * Updated: Feb, 2019
 *
 * Basic code to read from the Sparkfun INMP401 microphone.
 * 
 * If your microphone uses 3.3V, then please read up on analogReference(EXTERNAL) in the setup().
 * 
 * You may also need to adjust the DC_OFFSET to be the value of the microphone when all is silent. Test and find out.
 *
 * Resultant fps is 8940 or about an 8KHz sampling rate, which is fine for basic volume sampling, but not for frequency sampling.
 * 
 */


#define MIC_PIN    5                                          // Analog port for microphone
  
// Global variable(s) used by other routines.



void setup() {

  analogReference(EXTERNAL);                                  //  Audio samples are 'cleaner' at 3.3V. COMMENT OUT THIS LINE FOR 3.3V ARDUINOS (FLORA, ETC.) or for 5V microphones.
  Serial.begin(57600);                                        // Initialize serial port for debugging.

} // setup()



void loop() {

  showfps();
  analog_sample();
  
} // loop()



void analog_sample() {

// Local definitions
#define DC_OFFSET  509                                        // DC offset in mic signal.
  
// Temporary local variables
  int micIn = 0;
  
  micIn = analogRead(MIC_PIN) - DC_OFFSET;                    // Sample the microphone. Range will result in -512 to 512.
//  Serial.println(micIn);

} // analog_sample()



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
