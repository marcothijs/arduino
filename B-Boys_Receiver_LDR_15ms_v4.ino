#include <Wire.h>
#include <LCD_I2C.h> //LCD library
String test = "t16";

#define ldrPin 8
#define period 15

const int buttonPin = 2;     // the number of the pushbutton pin
const int buttonLedPin = 4;  
const int lifiLedPin = 6;  


int buttonState;  // variable for reading the pushbutton status
bool previous_state;
bool current_state;

bool start;
bool cardScanned;

String text01 = "Scan Xtra-kaart";
String text02 = "Druk op de knop";

int inputLength = 0;

uint8_t turnLeft[8] = {0x00,0x04,0x08,0x1F,0x09,0x05,0x01,0x01};  
uint8_t turnRight[8] = {0x00,0x04,0x02,0x1F,0x12,0x14,0x10,0x10};
uint8_t up[8] = {0x00,0x04,0x0E,0x15,0x04,0x04,0x04,0x00};
uint8_t down[8] = {0x00,0x04,0x04,0x04,0x15,0x0E,0x04,0x00};
uint8_t toEndUp[8] = {0x1F,0x04,0x0E,0x15,0x04,0x04,0x04,0x00};
uint8_t turnRightDown[8] = {0x00,0x1C,0x04,0x04,0x15,0x0E,0x04,0x00};
uint8_t turnLeftDown[8] = {0x00,0x07,0x04,0x04,0x15,0x0E,0x04,0x00};
uint8_t button[8] = {0x00,0x00,0x0E,0x1F,0x1F,0x1F,0x0E,0x00};

typedef struct {
  char code;
  String locName;
} locator;

typedef struct {
  String buyStatus;
  String prod;
  int qty;
  String qUnit;
  String loc;
} shopItem;

shopItem list[] = {
  {"next","ProdA",4,"Stk","1-A-L"},
  {"wait","ProdB",16,"Stk","2-B-R"},
  {"wait","ProdC",8,"Stk","3-C-L"},
  {"wait","ProdD",1,"Kg","4-A-L"}
};

/*
shopItem list[] = {
  {"next","ProdA",8,"Stk","5-B-L"}
};
*/

locator loc[] = {
  {'0',"0-Y"},
  {'A',"1-A"},
  {'B',"1-B"},
  {'C',"1-C"},
  {'D',"1-Z"},
  {'E',"2-A"},
  {'F',"2-B"},
  {'G',"2-C"},
  {'H',"2-Y"},
  {'I',"3-A"},
  {'J',"3-B"},
  {'K',"3-C"},
  {'L',"3-Z"},
  {'M',"4-A"},
  {'N',"4-B"},
  {'O',"4-C"},
  {'P',"4-Y"},
  {'Q',"5-A"},
  {'R',"5-B"},
  {'S',"5-C"},
  {'T',"6-Z"}
};

LCD_I2C lcd(0x27);  // Set the LCD address to 0x27 for a 16 by 2 display
int items2buy = sizeof list/sizeof list[0];
int nrLoc = sizeof loc/sizeof loc[0];
char currentLocator = '0';
char lastLocator = '0';
String currentLocName;
String allLocators = "ABCDEFGHIJKLMNOPQRST";
char sideCode;
String locTempStr;
bool endShop = false;
bool leave = false;
bool readOK = false;
char locCode;
char buff[2];
  


void setup() {

  Serial.begin(9600);  // Initialize serial communication
  //Serial.println(test);
  //Serial.println(period);
  
  lcd.begin();
  lcd.backlight(); // turn on backlight
  lcd.clear(); //Clear the LCD screen, just in case!
  lcd.createChar(1, turnLeft);
  lcd.createChar(2, turnRight);
  lcd.createChar(3, up);
  lcd.createChar(4, down);
  lcd.createChar(5, toEndUp); 
  lcd.createChar(6, button);
  lcd.createChar(7, turnRightDown);
  lcd.createChar(8, turnLeftDown);

  pinMode(lifiLedPin, OUTPUT);
  pinMode(buttonLedPin, OUTPUT); // initialize the LED pin as an output (on board)
  pinMode(buttonPin, INPUT);  // initialize the pushbutton pin as an input
  pinMode(ldrPin, INPUT);
  cardScanned = true;
  start = true;

 
}

void loop() {
  
  while(start==true) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Goedendag!");
    delay(5000);
    //lcd.clear();
    start=false;
    cardScanned = false;
  }

  while(cardScanned==false) {
    lcd.clear(); //Clear the LCD screen of data from the last loop.
    while(buttonState==LOW) {
      lcd.setCursor(0,0);
      lcd.print("Scan Xtra-kaart");
      lcd.setCursor(0,1);
      lcd.print("Ja? Druk knop ");
      lcd.write(6);
      buttonState = digitalRead(buttonPin);
    }
    if (buttonState == HIGH) {
      digitalWrite(buttonLedPin, HIGH); // turn LED on
      cardScanned = true;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Klaar!");
      lcd.setCursor(0,1);
      lcd.print("Volg instructies");
      delay(1000);
      digitalWrite(buttonLedPin, LOW); // turn LED off
      buttonState = LOW;
      delay(2000);
    } 
  }

  while(endShop == false) {   
  for(int i=0; i<items2buy; i++) {
    if(list[i].buyStatus == "next") {
      currentLocName = locatorName(currentLocator);
      /*
      Serial.print(F("Prod: "));
      Serial.print(list[i].prod);
      Serial.print(F(" Qty: "));
      Serial.println(list[i].qty);
      Serial.print(currentLocName);
      Serial.print(F(" -> "));
      Serial.println(list[i].loc); */
      
      delay(100);
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(list[i].prod);
      lcd.print(F(" "));
      lcd.print(list[i].qty);
      lcd.print(F(" "));
      lcd.print(list[i].qUnit);
      lcd.setCursor(0,1);
      
      // at the bottom of row and next pickup 1 or 2 rows away -> turn left
      if(currentLocName.charAt(2) == 'Y' &&
        (int(currentLocName.charAt(0)) +1 == int(list[i].loc.charAt(0)) || 
        int(currentLocName.charAt(0)) +2 == int(list[i].loc.charAt(0)))) {
        //Serial.println("Draai links de gang in ");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(list[i].prod);
        lcd.print(F(" "));
        lcd.print(list[i].qty);
        lcd.print(F(" "));
        lcd.print(list[i].qUnit);
        lcd.setCursor(0,1);
        lcd.write(1);
        lcd.print(F(" Draai links"));
      }
      // at location for pickup
      else if(list[i].loc.substring(0,3) == currentLocName) { 
        sideCode = (list[i].loc.charAt(4));
        /*
        Serial.print(F("Neem je product -> "));
        Serial.println(leftOrRicht(sideCode));
        Serial.print(F("Prod: "));
        Serial.print(list[i].prod);
        Serial.print(F(" Qty: "));
        Serial.println(list[i].qty);
        Serial.print(list[i].qty);
        Serial.print(F("x "));
        Serial.print(list[i].prod);
        Serial.println(F(" in de winkelkar")); */
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(list[i].prod);
        lcd.print(F(" "));
        lcd.print(list[i].qty);
        lcd.print(F(" "));
        lcd.print(list[i].qUnit);
        lcd.setCursor(0,1);
        lcd.print(F("Stop! Pak "));
        lcd.print(leftOrRicht(sideCode));
        delay(7000);
        lcd.clear();
        while(buttonState==LOW) {
          lcd.setCursor(0,0);
          lcd.print(list[i].prod);
          lcd.print(F(" "));
          lcd.print(list[i].qty);
          lcd.print(F(" "));
          lcd.print(list[i].qUnit);
          lcd.setCursor(0,1);
          lcd.print(F("Klaar? Druk op "));
          lcd.write(6);
          buttonState = digitalRead(buttonPin);
        }
        
        if (buttonState == HIGH) {
          digitalWrite(buttonLedPin, HIGH); // turn LED on
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(list[i].prod);
          lcd.print(F(" "));
          lcd.print(list[i].qty);
          lcd.print(F(" "));
          lcd.print(list[i].qUnit);
          lcd.setCursor(0,1);
          lcd.print(F("Item in de kar!"));
          delay(1000);
          digitalWrite(buttonLedPin, LOW); // turn LED off
          buttonState = LOW;
          delay(2000);
        
          list[i].buyStatus = "bought";
          list[i+1].buyStatus = "next";
            if(i == items2buy-1) {
            endShop = true; 
            }
          }
        }
      // next pickup is in this row
      else if(list[i].loc.charAt(0) == currentLocName.charAt(0) &&
        list[i].loc.charAt(2) > currentLocName.charAt(2)) { 
        //Serial.println(F("Ga verder in deze gang"));
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(list[i].prod);
        lcd.print(F(" "));
        lcd.print(list[i].qty);
        lcd.print(F(" "));
        lcd.print(list[i].qUnit);
        lcd.setCursor(0,1);
        lcd.write(3);
        lcd.print(F(" Ga verder"));
      }  
      // in a row but next pickup is 1 or 2 rows further, turn right
      else if(currentLocName.charAt(2) != 'Y' &&
        currentLocName.charAt(2) != 'Z' &&
        currentLocName.charAt(0) % 2 != 0 && 
        (int(list[i].loc.charAt(0)) == int(currentLocName.charAt(0)) +1 ||
        int(list[i].loc.charAt(0)) == int(currentLocName.charAt(0))+2)) {
        //Serial.println(F("Ga tot het einde van de gang en sla de volgende gang in"));
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(list[i].prod);
        lcd.print(F(" "));
        lcd.print(list[i].qty);
        lcd.print(F(" "));
        lcd.print(list[i].qUnit);
        lcd.setCursor(0,1);
        lcd.write(5);
        lcd.write(7);
        lcd.print(F(" Volgende gang"));
      }  
      // in a row but next pickup is 1 or 2 rows further, turn left
      else if(currentLocName.charAt(2) != 'Y' &&
        currentLocName.charAt(2) != 'Z' &&
        currentLocName.charAt(0) % 2 == 0 && 
        (int(list[i].loc.charAt(0)) == int(currentLocName.charAt(0)) +1 ||
        int(list[i].loc.charAt(0)) == int(currentLocName.charAt(0))+2)) {
        //Serial.println(F("Ga tot het einde van de gang en sla de volgende gang in"));
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(list[i].prod);
        lcd.print(F(" "));
        lcd.print(list[i].qty);
        lcd.print(F(" "));
        lcd.print(list[i].qUnit);
        lcd.setCursor(0,1);
        lcd.write(5);
        lcd.write(8);
        lcd.print(F(" Volgend gang"));
      }  
      // at the end of row and next pickup more than 2 rows away
      else if((currentLocName.charAt(2) == 'Y' ||
        currentLocName.charAt(2) == 'Z') && 
        int(list[i].loc.charAt(0)) > int(currentLocName.charAt(0)) + 2) {
        //Serial.println(F("Sla de volgende 2 gangen over"));
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(list[i].prod);
        lcd.print(F(" "));
        lcd.print(list[i].qty);
        lcd.print(F(" "));
        lcd.print(list[i].qUnit);
        lcd.setCursor(0,1);
        lcd.write(3);
        lcd.write(3);
        lcd.print(F(" Sla 2x over"));
      }  
      // in a row and next pick up is more than 2 rows away
      else if(currentLocName.charAt(2) != 'Y' &&
        currentLocName.charAt(2) != 'Z' && 
        currentLocName != "" &&
        int(list[i].loc.charAt(0)) > int(currentLocName.charAt(0)) + 2) { 
        //Serial.println(currentLocName);
        //Serial.println(F("Ga tot aan het einde van de gang en sla de volgende 2 gangen over"));
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(list[i].prod);
        lcd.print(F(" "));
        lcd.print(list[i].qty);
        lcd.print(F(" "));
        lcd.print(list[i].qUnit);
        lcd.setCursor(0,1);
        lcd.write(5);
        lcd.write(0x7E); // right arrow sign
        lcd.write(0x7E); // right arrow sign
        lcd.print(F(" Sla 2x over"));
      }  
      // if you missed pickup location
      else if(int(list[i].loc.charAt(0)) < int(currentLocName.charAt(0)) ||
        (int(list[i].loc.charAt(0)) == int(currentLocName.charAt(0)) &&
        list[i].loc.charAt(2) < currentLocName.charAt(2))) { 
        //Serial.println(F("Ga terug, je bent er al voorbij!"));
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(list[i].prod);
        lcd.print(F(" "));
        lcd.print(list[i].qty);
        lcd.print(F(" "));
        lcd.print(list[i].qUnit);
        lcd.setCursor(0,1);
        lcd.write(4);
        lcd.print(F(" Ga terug"));
      }
      // at the end of row and next pickup 1 or 2 rows away -> turn right
      else if(currentLocName.charAt(2) == 'Z' && 
      (int(currentLocName.charAt(0)) +1 == int(list[i].loc.charAt(0)) || 
        int(currentLocName.charAt(0)) +2 == int(list[i].loc.charAt(0)))) {
        //Serial.println(F("Draai rechts de gang in "));
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(list[i].prod);
        lcd.print(F(" "));
        lcd.print(list[i].qty);
        lcd.print(F(" "));
        lcd.print(list[i].qUnit);
        lcd.setCursor(0,1);
        lcd.write(2);
        lcd.print(F(" Draai rechts"));
      }

      // Test with serial input
      //while (Serial.available() == 0) { }
      //currentLocator = Serial.readString().charAt(0); 
      
      readOK = false;
      while(!readOK) {
        current_state = get_ldr();
        if(!current_state && previous_state) {
          sprintf(buff, "%c", get_byte());
          //Serial.print("Buff: ");
          //Serial.println(buff);
          currentLocator = correctLocator(String(buff).charAt(0),lastLocator);
          delay(100);
          //Serial.print("Locator after correction: ");
          //Serial.println(currentLocator);
          Serial.print(currentLocator);
        }
        previous_state = current_state;
      }

      //Serial.print(F("Locator ontvangen: "));
      //Serial.println(currentLocator);
      
      delay(100);

      //Serial.print(F("Last: "));
      //Serial.print(lastLocator);
      //Serial.print(F(" - Current: "));
      //Serial.println(currentLocator);
      
      if(endShop) { 
        //Serial.println(F("Alles aangekocht"));
        //Serial.println(F("Ga naar de kassa"));
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(F("Alles aangekocht"));
        lcd.setCursor(0,1);
        lcd.print(F("Ga naar de kassa"));
      }  
      
     }
     
    }
    
  }
  while(endShop && !leave) {
    //while (Serial.available() == 0) { }
    //currentLocator = Serial.readString().charAt(0);
    
    readOK = false;
      while(!readOK) {
        current_state = get_ldr();
        if(!current_state && previous_state) {
          sprintf(buff, "%c", get_byte());
          //Serial.print("Buff: ");
          //Serial.println(buff);
          currentLocator = correctLocator(String(buff).charAt(0),lastLocator);
          delay(100);
          //Serial.print("Locator after correction: ");
          //Serial.println(currentLocator);
          Serial.print(currentLocator);
        }
        previous_state = current_state;
      }
    
    //Serial.print(F("Locator ontvangen: "));
    //Serial.println(currentLocator);
    if(currentLocator == 'T') {
      //Serial.println(F("Bedankt voor je bezoek"));
      //Serial.println(F("Tot gauw!")); 
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(F("Bedankt"));
      lcd.setCursor(0,1);
      lcd.print(F("Tot gauw!"));
      delay(10000);
      lcd.clear();
      leave = true;
    }
  } 
}  

// correct the location when not well read from the LDR module
char correctLocator(char code, char lastCode) {
  //Serial.print(F("Input corrector code: "));
  //Serial.println(code);
  //Serial.print(F("Input corrector lastCode: "));
  //Serial.println(lastCode);
  //Serial.print("Check conditie == \0: ");
  //Serial.println(code == '\0');
  char newCode;
 
  if(code == '\0') { // || String(code) == "" || code == '')  {
    code = '&';
  }
  //Serial.print(F("Status Check: "));
  //Serial.println(allLocators.indexOf(code));
  if(allLocators.indexOf(code) == -1)  {
    newCode = lastCode;
  }
  else {
    newCode = code;
    readOK = true;  // comment if you want to test LIFI signal
    lastLocator = currentLocator;
  }    
  return newCode;  
}

String locatorName(char code) {
  String locName = "";
  //Serial.println(code);
  for(int j=0;j<nrLoc; j++) {
    if(code == (loc[j].code)) {
      locName = loc[j].locName;
    } 
  }
  return locName;
}

String leftOrRicht(char sideCode) {
  String side;
  if(sideCode == 'L') {
    side = "links";
  }  
  else {
    side = "rechts";  
  }
  return side;
}

bool get_ldr() {
  int ldr = digitalRead(ldrPin);
  digitalWrite(lifiLedPin, ldr == 1 ? LOW : HIGH); // show Lifi signal on led
  return ldr == 1 ? false : true;
}

char get_byte() {
  char ret = 0;
  delay(period*1.5);
  for(int i = 0; i < 8; i++)
  {
   ret = ret | get_ldr() << i; 
   delay(period);
  }
  return ret;
}
