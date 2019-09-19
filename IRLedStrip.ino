#include <FastLED.h>
#include <IRremote.h>
int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);
decode_results results;
unsigned long key_value = 0;
int OnOFFInd = 0;
#define LED_PIN     13
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define SPARKING 120
#define COOLING  55
#define FRAMES_PER_SECOND 60

#define OCTAVE 1 //   // Group buckets into octaves  (use the log output function LOG_OUT 1)
#define OCT_NORM 0 // Don't normalise octave intensities by number of bins
#define FHT_N 256 // set to 256 point fht
#include <FHT.h> // include the library
int noise[] = {204,190,108,85,65,65,65,70};
float noise_fact[] = {2.1, 7, 1.67, 1.52, 1.45,1.49, 1.53,1.62  }; // noise level determined by playing pink noise and seeing levels [trial and error]{204,188,68,73,150,98,88,68}
float noise_fact_adj[] = {2.1, 7, 1.67, 1.52, 1.45,1.49, 1.53,1.62 }; // noise level determined by playing pink noise and seeing levels [trial and error]{204,188,68,73,150,98,88,68}
// Params for width and height
int kMatrixWidth = 5;
int kMatrixHeight = 26;
int NUM_LEDS = (kMatrixWidth * kMatrixHeight);
CRGB leds[130];
uint8_t Brightness = 80;
uint8_t Sarutation = 180;
static uint8_t hue = 150;
int lampModeInd = 0;
int scale = 230;
int counter2=0;
int modeCounter = 0;
int scaleInc = 1;
// Param for different pixel layouts
bool    kMatrixSerpentineLayout = false;

bool gReverseDirection = false;
CRGBPalette16 gPal;

void setup() {
  Serial.begin(9600);
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");
  random16_set_seed(8934);
  random16_add_entropy(analogRead(3));
  delay( 1000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  Brightness );
    
  gPal = HeatColors_p;
  fill_solid(leds, NUM_LEDS, CRGB::Black); 
  FastLED.show();  
// TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
//  DIDR0 = 0x01; // turn off the digital input for adc0
  
}

void loop() {
  if (irrecv.decode(&results)) {
    if (results.value == 0XFFFFFFFF && 
    (key_value == 0xFF5AA5 || key_value == 0xFF10EF|| key_value == 0xFF18E7|| key_value == 0xFF4AB5 || key_value == 0xFFB04F || key_value == 0xFF6897)) results.value = key_value;
//    if (lampModeInd == 3 ){
//      lampModeInd = 1;
//      changeHue(); 
//    }
    
    Serial.print("[");
    Serial.print(results.value, HEX);
    Serial.print("]  ");
    switch(results.value){
          case 0xFF4AB5:
          Serial.println("↓");
          Brightness -= 3;
            fillStrip(hue,Brightness,Sarutation); 
          Serial.println(Brightness);
          break ;  
          case 0xFF18E7:
          Serial.println("↑");
          Brightness += 3;
            fillStrip(hue,Brightness,Sarutation); 
          Serial.println(Brightness);
          break ;               
          case 0xFF10EF:
          Serial.println("<-"); 
            hue-=3;
            fillStrip(hue,Brightness,Sarutation); 
          Serial.println(hue);
          break ;  
          case 0xFF5AA5:
          Serial.println("->"); 
            hue+=3; 
            fillStrip(hue,Brightness,Sarutation); 
          Serial.println(hue);
          break ;  
          case 0xFF38C7:
          Serial.println("OK");
          if (OnOFFInd == 1) {
            OnOFFInd = 0;
            fillStrip(hue,Brightness,0); 
            FastLED.show();  
            lampModeInd = 1;
          }
          else {
            OnOFFInd = 1;
            fill_solid(leds, NUM_LEDS, CRGB::Black); 
            FastLED.show();  
          }
          break ;   
          case 0xFFB04F:
          Serial.println("#");
            Sarutation+=3; 
            fillStrip(hue,Brightness,Sarutation); 
          Serial.println(Sarutation);
          break ;
          case 0xFF6897:
          Serial.println("*");
            Sarutation-=3; 
            fillStrip(hue,Brightness,Sarutation); 
          Serial.println(Sarutation);
          
          break ;
          case 0xFF9867:
          Serial.println("0");
          break ;
          case 0xFFA25D:
          Serial.println("1");
          lampModeInd = 1;
          break ;
          case 0xFF629D:
          Serial.println("2");
          lampModeInd = 2;
          break ;
          case 0xFFE21D:
          Serial.println("3");
          lampModeInd = 3;
          break ;
          case 0xFF22DD:
          Serial.println("4");
          lampModeInd = 4;
          break ;
          case 0xFF02FD:
          Serial.println("5");
          lampModeInd = 5;
          break ;
          case 0xFFC23D:
          Serial.println("6");
          break ;
          case 0xFFE01F:
          Serial.println("7");
          break ;
          case 0xFFA857:
          Serial.println("8");
          break ;
          case 0xFF906F:
          Serial.println("9");
          break ;                   
        }
        key_value = results.value;
        irrecv.resume();
        
    Serial.print("Mode: ");
    Serial.println(lampModeInd);
  }
  switch(lampModeInd){
    case 1: 
    break;
    case 2: 
      MusicReactive();
//      lampModeInd = 1;
    break;
    case 3: 
      Cylon();
    break;
    case 4: 
     showFire();
    break;
    case 5: 
     NoisePlayground();
    break;
  }
}
void fillStrip(uint8_t hue, uint8_t sarutation, uint8_t brightness){
    fill_solid(leds, NUM_LEDS, CHSV(hue, brightness, sarutation));
    FastLED.show();  
}

void NoisePlayground(){
  
      Serial.println("NoisePlayground 5 activated ");
// x,y, & time values
uint32_t x,y,v_time,hue_time,hxy;

// Play with the values of the variables below and see what kinds of effects they
// have!  More octaves will make things slower.

// how many octaves to use for the brightness and hue functions
uint8_t octaves=1;
uint8_t hue_octaves=3;

// the 'distance' between points on the x and y axis
int xscale=57771;
int yscale=57771;

// the 'distance' between x/y points for the hue noise
int hue_scale=1;

// how fast we move through time & hue noise
int time_speed=1111;
int hue_speed=31;

// adjust these values to move along the x or y axis between frames
int x_speed=331;
int y_speed=1111;
  hxy = (uint32_t)((uint32_t)random16() << 16) + (uint32_t)random16();
  x = (uint32_t)((uint32_t)random16() << 16) + (uint32_t)random16();
  y = (uint32_t)((uint32_t)random16() << 16) + (uint32_t)random16();
  v_time = (uint32_t)((uint32_t)random16() << 16) + (uint32_t)random16();
  hue_time = (uint32_t)((uint32_t)random16() << 16) + (uint32_t)random16();
  while(1){
    
  
      Serial.println("NoisePlayground loop activated ");
    if (irrecv.decode(&results)) {
      Serial.print("[");
      Serial.print(results.value, HEX);
      Serial.println("]  ");
      if (results.value != 0XFFFFFFFF) {
            lampModeInd = 1;
      }
      break;
    }
    
  // fill the led array 2/16-bit noise values
  fill_2dnoise16(LEDS.leds(), 5, 26, true,
                octaves,x,xscale,y,yscale,v_time,
                hue_octaves,hxy,hue_scale,hxy,hue_scale,hue_time, false);

  LEDS.show();

  // adjust the intra-frame time values
  x += x_speed;
  y += y_speed;
  v_time += time_speed;
  hue_time += hue_speed;
      Serial.println("NoisePlayground loop ended ");
   delay(3);
  }
  
}

void Cylon() 
{ 
  while(1){
    if (irrecv.decode(&results)) {
      Serial.print("[");
      Serial.print(results.value, HEX);
      Serial.println("]  ");
      if (results.value != 0XFFFFFFFF) {
            lampModeInd = 1;
      }
      break;
    }
    static uint8_t hue = 0;
    // First slide the led in one direction
    for(int i = 0; i < NUM_LEDS; i++) {
      // Set the i'th led to red 
      leds[i] = CHSV(hue++, 255, 255);
      // Show the leds
      FastLED.show(); 
      // now that we've shown the leds, reset the i'th led to black
      // leds[i] = CRGB::Black;
      fadeall();
      // Wait a little bit before we loop around and do it again
      delay(3);
    }
    // Now go in the other direction.  
    for(int i = (NUM_LEDS)-1; i >= 0; i--) {
      // Set the i'th led to red 
      leds[i] = CHSV(hue++, 255, 255);
      // Show the leds
      FastLED.show();
      // now that we've shown the leds, reset the i'th led to black
      // leds[i] = CRGB::Black;
      fadeall();
      // Wait a little bit before we loop around and do it again
      delay(3);
    }
    
  if (scale < 255 && scaleInc) scale +=5;  
  else if (scale >= 255 || !scaleInc) { scaleInc = (scale <= 230); scale-=5; }
//   if (scale> 255) scale = 200;
//   else if (scale< 200) scale = 250;
//   else scale+=5;
//   if (scale == 255) lampModeInd = 1;
  }
}
void fadeall() 
{ 
  for(int i = 0; i < NUM_LEDS; i++) 
  { 
    leds[i].nscale8(scale); 
  } 
}

void MusicReactive(){
 
int bands[6];
int counter=0;
int prev_beat=0;
int led_index=0;
float master_volume = 0.0;
int bandWidth = 4;
int sum = 0;
int sampleBeat = 0;
uint8_t bandInnerIndex = 0;
int MaxSum =1400;
uint8_t const stepLimit = 83;
uint8_t const blurLimit = 95;
uint8_t const flashLimit = 90;
uint16_t hue = 0;
 while (1) { // reduces jitter
  
  if (irrecv.decode(&results)) {
    Serial.print("[");
    Serial.print(results.value, HEX);
    Serial.println("]  ");
    if (results.value != 0XFFFFFFFF) {
            kMatrixWidth = 5;
            kMatrixHeight = 26;
            modeCounter = 0;
            lampModeInd = 1;
    }
    break;
  }
  cli();  // UDRE interrupt slows this way down on arduino1.0
     
  for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
      while (!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fht_input[i] = k; // put real data into bins
    }
    fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fht
    fht_run(); // process the data in the fht
    fht_mag_octave(); // take the output of the fht  fht_mag_log()

//    every 50th loop, adjust the volume accourding to the value on A2 (Pot)
    if (counter >= 50) {
      ADMUX = 0x40 | (1 & 0x07); // set admux to look at Analogpin A1 - Master Volume
      
      while (!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc 
      delay(10);      
      while (!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc 
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      master_volume=map(k,0,1023,-100,100);
      
      ADMUX = 0x40 | (0 & 0x07); // set admux back to look at A0 analog pin (to read the microphone input
      counter = 0;
    }
        
    sei();
    counter++;
    for (int i = 2; i < 8; i++) {  // goes through each octave. skip the first 1, which is not useful
      int j;      
      j = (fht_oct_out[i] - noise[i]); // take the pink noise average level out, take the asbolute value to avoid negative numbers
      if (j<10) {j=0;}  
      j= j*noise_fact_adj[i];
      if (j<10) {j=0;}
      else j= j*noise_fact_adj[i];
      if(i!=1) sum+=j;
      bands[i-2]= j;
  }  
  if(MaxSum<=sum) MaxSum = sum;
  sampleBeat = map(sum,0,MaxSum + master_volume,0,100);;
  
  if (sampleBeat>=flashLimit) {
    fill_solid(leds, NUM_LEDS, CHSV(map(sampleBeat,flashLimit,100,255,0), 255, 255)); 
    FastLED.show();
  }                 
  else {
    if (prev_beat!=sampleBeat) {
      FastLED.setBrightness(map(sampleBeat,1,flashLimit-1,100,254));
      prev_beat=sampleBeat;
    }
  }
  if (sampleBeat) {      
    for( byte x = 0; x < 5; x++) {
      for(uint8_t bandIndex = 0; bandIndex<6; bandIndex++){
        CRGB Color = CHSV( bands[bandIndex]+ bandIndex*34, get_Sarutation(bands[bandIndex]), get_Brightness(bands[bandIndex]));
        CRGB ColorFade = CHSV( bands[bandIndex]+ bandIndex*34, get_Sarutation(bands[bandIndex]), get_Brightness(bands[bandIndex])/4);
        CRGB ColorFade2 = CHSV( bands[bandIndex]+ bandIndex*34, get_Sarutation(bands[bandIndex]), get_Brightness(bands[bandIndex])/2);
        leds[ XY(x, bandInnerIndex + ((bandIndex) * bandWidth))]  = Color;
        if(sampleBeat< flashLimit && sampleBeat<65){
          leds[ XY(x, bandInnerIndex - 1 + ((bandIndex) * bandWidth))]  = ColorFade2;
        }
        else if(sampleBeat<flashLimit){
          leds[ XY(x, bandInnerIndex - 1 + ((bandIndex) * bandWidth))]  = ColorFade;
          leds[ XY(x, bandInnerIndex + 1 + ((bandIndex) * bandWidth))]  = ColorFade;
        }
        if(sampleBeat>=flashLimit){
          leds[ XY(x, bandInnerIndex + ((bandIndex) * bandWidth))]  = CRGB::Black;
          leds[ XY(x, bandInnerIndex + 1 + ((bandIndex) * bandWidth))]  = ColorFade;
          leds[ XY(x, bandInnerIndex + 2 + ((bandIndex) * bandWidth))]  = ColorFade2;
          leds[ XY(x, bandInnerIndex + 3 + ((bandIndex) * bandWidth))]  = Color;
        }
        if(sampleBeat>=blurLimit && (bandIndex%5 == 0 )){
          FastLED.show();
        }
      }
      if(sampleBeat<flashLimit) fadeall(250);
      else fadeall(240);
      FastLED.show();
    }
    if(sampleBeat>=stepLimit && sampleBeat<flashLimit) counter2+=10;
//    else if(sampleBeat>=flashLimit)counter2-=10;
    if(counter2<0)counter2 = 10000;
    if(sampleBeat>=stepLimit )bandInnerIndex++;
//    if(sampleBeat>=blurLimit-2 || (sampleBeat<stepLimit-13) )bandInnerIndex--;
    if (bandInnerIndex> bandWidth ) bandInnerIndex=0;
    if ( bandInnerIndex<0) bandInnerIndex=bandWidth;
    
//  for(uint8_t index = 0; index < 6; index++) {
//    Serial.print(bands[index]);
//    Serial.print(",");
//  }
//  Serial.println("");
  Serial.print("   volume: ");
  Serial.print(master_volume);
  Serial.print("   Sbeat: ");
  Serial.print(sampleBeat);
   if(sampleBeat>=flashLimit) {
    Serial.print("   Flash ");
    if(sampleBeat>=blurLimit){
      Serial.print(sampleBeat);
      Serial.println(" Blur");
    }
    else Serial.println(sampleBeat);
   }
   else Serial.println("") ;
  }
  else fadeall(254);
  FastLED.show(); 
  if (sampleBeat) sampleBeat=0;
  sum = 0;
 }
}


uint8_t get_Sarutation(int j){ return map(j,0,305,100, 255);}
uint8_t get_Brightness(int j){ return map(j,0,305,10, 255);}
void fadeall(uint8_t scale) { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(scale); } }

uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  
  if( kMatrixSerpentineLayout == false) {
    i = (y * kMatrixWidth) + x;
  }

  if( kMatrixSerpentineLayout == true) {
    if( y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;

    } else {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;

    }
  }
  
  i=(i+counter2)%NUM_LEDS;  
  return i;
}

void showFire()
{
  while(1){
    if (irrecv.decode(&results)) {
      Serial.print("[");
      Serial.print(results.value, HEX);
      Serial.println("]  ");
      if (results.value != 0XFFFFFFFF) {
            lampModeInd = 1;
      }
      else{
      }
      break;
    }
    random16_add_entropy( random());
    FireWithPalette(); // run simulation frame, using palette colors
    
    FastLED.show(); // display this frame
    FastLED.delay(1000 / FRAMES_PER_SECOND);
  }
  
}

void FireWithPalette()
{
// Array of temperature readings at each simulation cell
  static byte heat[130];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}
