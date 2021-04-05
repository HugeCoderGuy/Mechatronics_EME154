/* User defined functions definition */
// The input is the empty char array for user to input
// After calling the function, the user's input will be stored into the char array
#define MAX_INPUT_NUM 50
void getString(char *a){
 String str;
 while(1){
  if(Serial.available() > 0) {
   str = Serial.readStringUntil('\n');
   str.toCharArray(a, MAX_INPUT_NUM);
   break;
  } 
 }
}
// array for storing the password
char str1[] = "SECRET";
// array for storing user's input
char str2[MAX_INPUT_NUM];
bool guessed = false;

void setup() {
 // put your setup code here, to run once:
 Serial.begin(9600);
 Serial.println("What is the password?");
}

// put your main code here, to run repeatedly
void loop() {

// get the user input string into the char array str2
 getString(str2);
 if(!strcmp(str1, str2)) {
  Serial.println("You Guessed It!");
  guessed = true;
 } 
else {
  Serial.print(str2);
  Serial.println("Wrong, Try Again");
}

// for this case, we don't need to do anything after inputting the password
 if (guessed) while(1);
}
