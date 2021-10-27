/****** FREE-TIME SYSTEM ******/
/******* EME154 UCDAVIS *******/

/********** REMARKS SECTION **********
  INZ - Initialize Supervisor
  DIG - Diagnostics Supervisor
  ERT - Error Treatment Supervisor
  MSS - Machine Status Scan Supervisor
  MCS - Mode Control Supervisor 
  ACS - Automatic Control Supervisor
  MOS - Manual Operation Supervisor
  PGS - Programming Supervisor 
  MSD - Machine Setup Data Supervisor
  OCS - Output Control Supervisor
**************************************/

// include the LiquidCrystal library
#include <LiquidCrystal.h>
// initialize the class named lcd
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); // LiquidCrystal(RS, Enable, D4, D5, D6, D7)

// include the Keypad library
#include <Keypad.h>

const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns
// define the two-dimensional array on the buttons of the keypad
char hexaKeys[ROWS][COLS] = {'1','2','3','A',
                             '4','5','6','B',
                             '7','8','9','C',
                             '*','0','#','D'};
byte rowPins[ROWS] = {5, 4, 3, 2}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {A5, A4, A3, A2}; // connect to the column pinouts of the keypad

// initialize an instance of class NewKeypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

char *machineMessage; // used to save a message
byte OMD, pre_OMD; // used to save the operation mode
// used to save the time in the manual mode
unsigned int MD, MH, MM,   AD, AH, AM,   DD, DH, DM; 
volatile unsigned int MS, AS, DS, pre_AS, pre_DS, pre_MS;
bool ERR1; // a flag for error level 1 
bool FFRA; // a flag for the first running


// INZ Initialize Supervisor
void INZfunction() {
  // initailize parameters
  OMD = 0;
  pre_OMD = 0;
  MD = 0;
  MH = 0;
  MM = 0;
  MS = 0;
  pre_MS = 0;

  AD = 0;
  AH = 0;
  AM = 0;
  AS = 0;
  pre_AS = 0;  
  DD = 0;
  DH = 0;
  DM = 0;
  DS = 0;
  pre_DS = 0;
  
  ERR1 = false;
  FFRA = true;
}

// DIG  Diagnostics Supervisor
int DIGfunction() {
  // put the diagnostics program here
  if (ERR1) {
    return 0; // for error
  } else {
    return 1; // successful diagnostics
  }
}

// ERT   Error Treatment Supervisor
void ERTfunction() {
  // take actions here to treat detected errors
  ERR1 = false;
}

// MSS  Machine Status Scan Supervisor
void MSSfunction() {
  // scan the keypad to get the input
  char customKey = customKeypad.getKey(); // get a key from the keypad
  if (customKey) {
    switch (customKey) {
      case '1':
        OMD = 1;
        if (pre_OMD != OMD) {
          TCNT1  = 0; // initialize counter value in Timer1 to 0
        }
      break;
      
      case '2':
        OMD = 2;
        if (pre_OMD != OMD) {
          TCNT1  = 0; // initialize counter value in Timer1 to 0
        }
      break;
      case '3':
        OMD = 3;
        if (pre_OMD != OMD) {
          TCNT1  = 0; // initialize counter value in Timer1 to 0
        }
      break;
      
      case '5':
        OMD = 5;
      break;
      default:
        //Serial.println(customKey); // print the key we press for checking
      break;
    }
  }
}

// MCS   Mode Control Supervisor 
void MCSfunction() {
  switch (OMD) {
    case 1:
      machineMessage = "Manual Mode Accepted";
      MOSfunction();
    break;
    case 2:
      machineMessage = "Automatic Mode Accepted";
      ACSfunction();
    break;
    case 3:
      machineMessage = "Set-up Mode Accepted";
      MSDfunction();
    break;
    case 5:
      machineMessage = "Exit Command Accepted";
    break;
    default:
    break;
  }
}



// MOS   Manual Operation Supervisor
void MOSfunction() {
  // put the program of Manual Operation Supervisor here
  if (MS > 59){  // 60 sec in a minute
    MM++;
    MS = 0;
  }
  if (MM > 59){  // 60 minutes in an hour
    MH++;
    MM = 0;
  }
  if (MH > 23){  // 24 hours in a day
    MD++;
    MH = 0;
  }
}

// ACS    ACS - Automatic Control Supervisor
void ACSfunction(){
  // put the program of Automatic Control Supervisor here
    if (AS > 59){  // 60 sec in a minute
    AM++;
    AS = 0;
  }
  if (AM > 59){  // 60 minutes in an hour
    AH++;
    AM = 0;
  }
  if (AH > 23){  // 24 hours in a day
    AD++;
    AH = 0;
  }
}

// MSD    Machine Setup Data Supervisor
void MSDfunction(){
  // put the program of Machine Setup Data Supervisor here
    if (DS > 59){  // 60 sec in a minute
    DM++;
    DS = 0;
  }
  if (DM > 59){  // 60 minutes in an hour
    DH++;
    DM = 0;
  }
  if (DH > 23){  // 24 hours in a day
    DD++;
    DH = 0;
  }
}

char *getCurrentMode() {
  switch (OMD) {
    case 1:
      return "Manual";
    case 2:
      return "Auto";
    break;
    case 3:
      return "Set-up";
    break;
    case 5:
      return "System Turned Off";
    default:
      return "Idle Mode";
  }
}

// OCS   Output Control Supervisor
void OCSfunction() {
  if (pre_MS != MS || pre_AS != AS || pre_DS != DS || pre_OMD != OMD || FFRA == true) { // print info every 1 second on LCD
    lcd.setCursor(0, 0); // lcd.setCursor(col, row)
    if (OMD == 5) {
      lcd.print("Turned Off!     ");
    } else {
      lcd.print(getCurrentMode());
      lcd.print("     ");
    }
    lcd.setCursor(0, 1); // lcd.setCursor(col, row)
    switch (OMD) {
      case 1:
        lcd.print(MD);
        lcd.print("D ");
        lcd.print(MH);
        lcd.print("H ");
        lcd.print(MM);
        lcd.print("M ");
        lcd.print(MS);
        lcd.print("S     ");
      break;

      case 2:
        lcd.print(AD);
        lcd.print("D ");
        lcd.print(AH);
        lcd.print("H ");
        lcd.print(AM);
        lcd.print("M ");
        lcd.print(AS);
        lcd.print("S     ");
      break;
      case 3:
        lcd.print(DD);
        lcd.print("D ");
        lcd.print(DH);
        lcd.print("H ");
        lcd.print(DM);
        lcd.print("M ");
        lcd.print(DS);
        lcd.print("S     ");
      break;
      
      case 5:
        lcd.print("System Exit!    ");
      break;
      default:
        lcd.print("0D 0H 0M 0S     ");
      break;
    }
    
    if (pre_OMD != OMD || FFRA == true) { // print info on serial monitor when OMD is changed 
      Serial.println(machineMessage);
      Serial.print("Current Mode: ");
      Serial.println(getCurrentMode());
      Serial.println("");
      Serial.println("*** RUN TIME LOG ***");
      
      Serial.print("Manual ");
      Serial.print(MD);
      Serial.print(" D ");
      Serial.print(MH);
      Serial.print(" H ");
      Serial.print(MM);
      Serial.print(" M ");
      Serial.print(MS);
      Serial.println(" S");
      Serial.println("");

      Serial.print("Auto   ");
      Serial.print(AD);
      Serial.print(" D ");
      Serial.print(AH);
      Serial.print(" H ");
      Serial.print(AM);
      Serial.print(" M ");
      Serial.print(AS);
      Serial.println(" S");
      Serial.println("");
      
      Serial.print("Set-up ");
      Serial.print(DD);
      Serial.print(" D ");
      Serial.print(DH);
      Serial.print(" H ");
      Serial.print(DM);
      Serial.print(" M ");
      Serial.print(DS);
      Serial.println(" S");
      Serial.println("");
      pre_OMD = OMD; // save current mode to the previous mode
      FFRA = false; // not the first time running
      delay(10); // wait for printing
    }
    pre_MS = MS; // save current time to the previous time
    pre_AS = AS;
    pre_DS = DS;
  }
}

void printmenu() {
  Serial.println("EME-154 Mechatronics");
  Serial.println("Free Time System");
  Serial.println("Alex Lewis!");
  Serial.println("****************************************");
  Serial.println("            OPERATION MENU");
  Serial.println("****************************************");
  Serial.println("1. Manual Operation");
  Serial.println("2. Automatic Opration");
  Serial.println("3. Machine Data Set-up");
  Serial.println("5. Exit");
  Serial.println("");
  delay(10); // wait for printing
}

void settimer() {
  // set up Timer
  noInterrupts(); // stop interrupts
  
  //set Timer1 interrupt at 1 Hz = 1 time per second
  TCCR1A = 0; // initialize TCCR1A register
  TCCR1B = 0; // initialize TCCR1B register
  TCNT1  = 0; // initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624; // = 16,000,000 / 1 Hz / 1024 prescaler - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12); // set WGM12 = 1
  // set CS12 and CS10 equal to 1 for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // enable timer compare match A interrupt
  TIMSK1 |= (1 << OCIE1A); // set OCIE1A = 1
  
  interrupts(); // allow interrupts
}

// timer interrupt function with compare match A interrupt in Timer1
ISR(TIMER1_COMPA_vect){
  if (OMD == 1) {
    MS++;
  }
  if (OMD == 2) {
    AS++;
  }
  if (OMD == 3) {
    DS++;
  }
}

void setup() {
  // put your setup code here, to run once:
  // set up the serial communication
  Serial.begin(9600);
  // initialize lcd
  lcd.begin(16, 2); // lcd.begin(cols, rows)
  lcd.clear(); // clear the LCD screen
  // print the menu
  printmenu();
  // set the timer
  settimer();
  // the initialization
  INZfunction();
}

void loop() {
  // put your main code here, to run repeatedly:
  // the main FTS
  if (DIGfunction()){
    // if diagnostics succeed
    MSSfunction();
    MCSfunction();
    OCSfunction();
  } else {
    // go to ERT if diagnostics fail
    ERTfunction();
    OCSfunction();
  }
  // if press the button 5, stop the system
  if (OMD == 5) {
    while (1) {};
  }
}
