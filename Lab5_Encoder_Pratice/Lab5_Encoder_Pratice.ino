#include <math.h>
#include <LiquidCrystal.h>
// for the timer
#define CLOCK_FREQUENCY 16000000
// pulses per revolution of encoder
#define PPR 40
// LDC pins:
#define LCD_RS A0
#define LCD_EN A1
#define LCD_D4 A2
#define LCD_D5 A3
#define LCD_D6 A4
#define LCD_D7 A5

// encoder pins
#define ENCODER_A 2
#define ENCODER_B 3
#define SWITCH_PIN 8
// motor pins
#define MOTOR_ENABLE 5
#define DIRA 4
#define DIRB 7

// motor speed gain
#define KP 20

// direction sensing flags
volatile byte AfirstUp = 0; 
volatile byte BfirstUp = 0; 
volatile byte AfirstDown = 0; 
volatile byte BfirstDown = 0; 
// CW when it increases, CCW when it decreases
volatile int encoderPos = 0; //this variable stores our current value of encoder position. 
volatile float radPos = 0;
volatile int lastEncoderPos = 0; //this variable stores our last value of encoder position for speed calculation. 
int oldEncPos = 1; //this variable stores our last value of encoder position for lcd printing
volatile byte pinA_read = 0; // this variable is shared among interrupts
volatile byte pinB_read = 0; // this variable is shared among interrupts

// speed calculation peroid
int frequency = 10; // get speed update every 1/frequency second
volatile float speed;
float lastSpeed = 1; // for lcd printing

bool pressed = 0;
// motor speed
volatile int PWM; // maximum 255
volatile int motorSetSpeed; // maximum 255
// LCD screen initialize
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
int counts=0;
// set up timer given frequency, interval: 1/freq
// set up timer given frequency, interval: 1/freq
void setupTimer(int freq){
  // set up Timer
  noInterrupts(); // stop interrupts
  
  // TIMER 1 for interrupt frequency freq Hz:
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0
  // set compare match register for freq Hz increments
  OCR1A = CLOCK_FREQUENCY/(64*freq) - 1; 
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12, CS11 and CS10 bits for 64 prescaler
  TCCR1B |= (0 << CS12) | (1 << CS11) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  interrupts(); // allow interrupts
}

void setMotorCW(){
  digitalWrite(DIRA,LOW); //one way
  digitalWrite(DIRB,HIGH);
}

void setMotorCCW(){
  digitalWrite(DIRA,HIGH); //one way
  digitalWrite(DIRB,LOW);
}

void driveMotor(int motor_speed){
  motor_speed = min(motor_speed, 255);
  analogWrite(MOTOR_ENABLE, motor_speed);
}
// interrupt process functions
ISR(TIMER1_COMPA_vect) {
  speed = (encoderPos - lastEncoderPos)*frequency; // pulses/second
  speed = speed*2.0*PI/PPR;  // radians/second
  lastEncoderPos = encoderPos;
  if(speed > 0){
  // set CW
    setMotorCW();
    PWM = (int)KP*abs(speed);
    motorSetSpeed = PWM;
  // driveMotor(PWM);
  }
  if(speed < 0){
  // set CW
    setMotorCCW();
    PWM = (int)KP*abs(speed);
    motorSetSpeed = -PWM;
  // driveMotor(PWM);

  }
  if(abs(speed)<0.5){
   PWM = 0;
   motorSetSpeed = 0;
  // driveMotor(0);
  }
  // drive motor here
  driveMotor(PWM);

}

void setup() {
  // pin mode for encoder A
  pinMode(ENCODER_A, INPUT);
  // pin mode for encoder B 
  pinMode(ENCODER_B, INPUT);
  // pin mode for switch 
  pinMode(SWITCH_PIN, INPUT_PULLUP); 
  attachInterrupt(0, doEncoderA, CHANGE); 
  attachInterrupt(1, doEncoderB, CHANGE); 
  
  Serial.begin(115200); // start the serial monitor link
  // timer initialization
  setupTimer(frequency);

  // lcd initialization
  lcd.begin(16, 2); // 
  lcd.clear(); // clear the LCD screen

  //Motor pins
  pinMode(MOTOR_ENABLE,OUTPUT); // speed with PWM output
  pinMode(DIRA,OUTPUT);   // direction pin
  pinMode(DIRB,OUTPUT);   // direction pin
  Serial.print("Counts,");
  Serial.print("Encoder_Position(radians),");
  Serial.print("Speed(radians/sec),");
  Serial.println("Motor Set Speed(-255~255)");
}

void doEncoderA(){
  pinA_read = digitalRead(ENCODER_A);
  pinB_read = digitalRead(ENCODER_B);
  // for rising edge
  //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
  if((pinA_read==1) && (pinB_read==1) && BfirstUp) { 
    encoderPos ++; //decrement the encoder's position count
    AfirstUp = 0; //reset flags for the next turn
    BfirstUp = 0; //reset flags for the next turn
  }
  // Direction sensing
  else if ((pinA_read==1) && (pinB_read==0)) AfirstUp = 1; 

  // for falling edge
  //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
  if((pinA_read==0) && (pinB_read==0) && BfirstDown) { 
    encoderPos ++; //decrement the encoder's position count
    AfirstDown = 0; //reset flags for the next turn
    BfirstDown = 0; //reset flags for the next turn
  }
  // Direction sensing
  else if ((pinA_read==0) && (pinB_read==1)) AfirstDown = 1; 
}

void doEncoderB(){
  // reading = PIND & 0xC; //read all eight pin values then strip away all but pinA and pinB's values
  pinA_read = digitalRead(ENCODER_A);
  pinB_read = digitalRead(ENCODER_B);
  if ((pinA_read==1) && (pinB_read==1) && AfirstUp) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoderPos --; //increment the encoder's position count
    AfirstUp = 0; //reset flags for the next turn
    BfirstUp = 0; //reset flags for the next turn
  }
  // Direction sensing
  else if ((pinA_read==0) && (pinB_read==1)) BfirstUp = 1; 

  // for falling edge
  //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
  if((pinA_read==0) && (pinB_read==0) && AfirstDown) { 
    encoderPos --; //decrement the encoder's position count
    AfirstDown = 0; //reset flags for the next turn
    BfirstDown = 0; //reset flags for the next turn
  }
  // Direction sensing
  else if ((pinA_read==1) && (pinB_read==0)) BfirstDown = 1; 
}

void loop(){

  pressed = (digitalRead(SWITCH_PIN)==LOW);
  if(pressed){
    while(counts<500){
      counts++;
      Serial.print(counts); Serial.print(','); 
      // change the encode position in angles
      radPos = encoderPos*2*PI/40;
      Serial.print(radPos); Serial.print(',');
      Serial.print(speed);Serial.print(',');
      Serial.println(motorSetSpeed); 
      delay(20); 
    }
    while(1);
  }
}
