#include <OneWire.h>

OneWire  ds(0);  // on pin 10
 int led = 1;
 // the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  pinMode(2, OUTPUT);
  digitalWrite(led, LOW);
  digitalWrite(2,HIGH);
  delay(1000);
  digitalWrite(2,LOW);
 
}
 
// the loop routine runs over and over again forever:
void loop() {
    byte i;
    byte type_s;
    byte present = 0;
    byte data[12];
    byte addr[8];
    float celsius;

   
    // turn off LED to indicate everything OK
    if ( !ds.search(addr) ) {
      ds.reset_search();
      return;
    }
    // the first ROM byte indicates which chip
    switch (addr[0]) {
    case 0x10:
      //Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      //Serial.println("  Chip = DS18B20");
      //indicate(2,2000);
      type_s = 0;
      break;
    case 0x22:
      //Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      while(1)  // loop forever if no DS182x found
        ;
  } 

    if ( addr[0] != 0x28 ) {

       return;
    }

    ds.reset();
    ds.select(addr);
    ds.write(0x44,1);
    delay(1000);

    present = ds.reset();
    ds.select(addr);
    ds.write(0xBE);
    for ( i = 0; i < 9; i++) { 
      data[i] = ds.read();
    }

    unsigned int raw = (data[1] << 8) | data[0];
    if (type_s) {
      raw = raw << 3; // 9 bit resolution default
      if (data[7] == 0x10) {
        // count remain gives full 12 bit resolution
        raw = (raw & 0xFFF0) + 12 - data[6];
      }
    } else {
      byte cfg = (data[4] & 0x60);
      if (cfg == 0x00) raw = raw << 3;  // 9 bit resolution, 93.75 ms
      else if (cfg == 0x20) raw = raw << 2; // 10 bit res, 187.5 ms
      else if (cfg == 0x40) raw = raw << 1; // 11 bit res, 375 ms
      // default is 12 bit resolution, 750 ms conversion time
    }
    celsius = (float)raw / 16.0;
    indicate(celsius,100);
    if ( celsius < 7.0 ) {
       digitalWrite(2,HIGH);
    } else {
      digitalWrite(2,LOW);
    }

    
}

void indicate( int howMany, int delayBetween ) {
 int m = 0;
 for ( m = 0; m < howMany; m++) {
      digitalWrite(led, HIGH);
      delay(delayBetween);
      digitalWrite(led, LOW);
      delay(delayBetween);
 }
      
 return;
}
