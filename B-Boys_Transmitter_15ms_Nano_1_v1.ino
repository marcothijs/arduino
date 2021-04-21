#define period 15
#define wait 1000

char * locString = "ABCDEFGHIJ";
int locations = strlen(locString);
int locPins[10] = {2,3,4,5,6,7,8,9,10,11};

void setup() 
{
  Serial.begin(9600);

  for(int j = 0; j < locations; j ++) {
    pinMode(locPins[j], OUTPUT);
    digitalWrite(locPins[j], HIGH);
  }

}

void loop() {
  for(int j = 0; j < locations; j ++) {
    digitalWrite(locPins[j], LOW);
  }
  delay(period);

  for(int i = 0; i < 8; i++) {
  
    for(int j = 0; j < locations; j ++) {
      send_loc_bit(i, locString[j], locPins[j]); // send bit i for loc j on pin j)
    }
  delay(period);
  }

  for(int j = 0; j < locations; j ++) {
    digitalWrite(locPins[j], HIGH);
  }
  delay(period);

delay(wait);
}

void send_loc_bit(int bitNr, char loc, int pin) {

  //transmission of bit i for location=loc and pinnumber=pin

  //  Serial.print(loc);
  //  Serial.print("-");
  //  Serial.print(pin);
  //  Serial.print("-");
  //  Serial.print((loc&(0x01 << i))!=0);
  //  Serial.print("*");

  digitalWrite(pin, (loc&(0x01 << bitNr))!=0);
}
