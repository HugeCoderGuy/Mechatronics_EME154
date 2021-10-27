// include the LiquidCrystal library
#include <LiquidCrystal.h>
// initialize the class named lcd
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); // LiquidCrystal(RS, Enable, D4, D5, D6, D7)

// set up global variables
unsigned int numstar;
volatile unsigned int secs = 0; // use volatile (changing frequently) variables in interrupt functions
volatile unsigned int fake_secs = 0; //.008s increases that will be used for 1s timing.
int pre_secs = secs;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // initialize lcd
  lcd.begin(16, 2); // lcd.begin(cols, rows)
  lcd.clear(); // clear the LCD screen 

  String str;

  // set the position of the cursor and print info.
  lcd.setCursor(0, 0); // lcd.setCursor(col, row)
  lcd.print("Number of Stars:");

  // get information from the serial monitor
  Serial.println("How many stars do you want to display?");
  while(1) {
    if(Serial.available() > 0) {
      str = Serial.readStringUntil('\n');
      numstar = str.toInt();
      Serial.println(numstar);
      lcd.setCursor(0, 1);
      lcd.print(numstar);
      delay(1000); // wait for printing
      break;
    }
  }

  lcd.clear(); // clear info. on LCD

  // set up Timer
  noInterrupts(); // stop interrupts

  //Initial Setup
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;
  //CTC Mode and 1024 prescaler
  TCCR2A |= (1<<WGM21);
  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
  //Compare match and allow compare match inturrupt 
  OCR2A = 124;
  TIMSK2 |= (1<<OCIE2A);
  
  interrupts(); // allow interrupts
}

// timer interrupt function with compare match A interrupt in Timer1
ISR(TIMER2_COMPA_vect) {
    fake_secs++;
    if (fake_secs == 125) {
      secs++;
      fake_secs = 0;
    }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (pre_secs != secs) { // when secs increases, print * or seconds
    if (secs <= numstar) {
      Serial.print("*");
      if (secs == numstar) {
        Serial.println("");
      }
      lcd.setCursor(secs-1, 0);
      lcd.print("*");
    } else {
      Serial.print("Second: ");
      Serial.println(secs);
      // for checking with internal clock
      Serial.print("Internal Clock: ");       
      Serial.println(float(millis())/1000.0); // millis() = milliseconds
    }

    lcd.setCursor(0, 1);
    lcd.print(secs, DEC);
    lcd.print("S");
    
    pre_secs = secs;
  }
}
