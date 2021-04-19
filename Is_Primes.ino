// Global Variables:
  int prime[46]; // Aloting memory for any prime numbers
  int i;
  int j = 0;
  int x = 0; //i,j, and x are all just variables for "for" loops.
  int counter = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Initally make the prime array = 2 because 2 is first prime number
  for(j=0; j<46; j++) {
    prime[j] = 2;
  }
   
 for (i=2; i<=200; i++) { 

  // If i is not divisible by the first series of prime numbers in prime array,
  // add it to prime[] array at "counter" index.
  if(i%prime[0] != 0 && i%prime[1] != 0 && i%prime[2] != 0 && i%prime[3] != 0 && i%prime[4] != 0 && i%prime[5] != 0) {
    prime[counter] = i;
    counter++;

  }
 }
}


void loop() {
  // Here we will print prime numbers array
  
  delay(1000); //prevent Serial from half printing the void loop.
  Serial.print("\n");
  Serial.print("Here are our prime numbers!");
  Serial.print("\n");
  
  for (x=0; x<46; x++) { //Iterate through all enteries in prime[] to display
    Serial.print(prime[x]);
    Serial.print(", ");
    if(x == 25) { //inset a new line about half way through for Serial
      Serial.print("\n");
      }
  }

  // make it stop
  while(1){};
}

/* This code could potentially be done faster by not having as many for loops
 *  or just by optimizing the for loops. */
 
 */
