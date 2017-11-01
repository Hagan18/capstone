
/*  Assumes a 1s delay between individuals entering and leaving */

int leftFlag=0;
int rightFlag=0;
int completeFlag=0;
unsigned long leftMillis; //left sensor
unsigned long rightMillis; //right sensor
int left;
int right;

int totalEntered = 0;
int totalExited = 0;
unsigned int population=0;

void setup() {
  // put your setup code here, to run once:
  pinMode(7,INPUT);
  pinMode(4,INPUT);
  Serial.begin(9600);
}

void loop() {
  if(population<0){
    population=0;
  } //error in sensor reading
  // put your main code here, to run repeatedly:
  left = digitalRead(7);
  right = digitalRead(4);
  //Serial.println(right);

  if (((left == 0) && (right == 1)) && completeFlag == 0){
    leftMillis = millis();
    if (rightFlag == 1){
      completeFlag = 1;
      leftFlag = 1;
      //Serial.write("\ngoing left");
    }
    else {
      completeFlag = 0;
      leftFlag = 1;
    }
  }

  if (((right == 0) && (left ==1)) && completeFlag == 0){
    rightMillis = millis();
    if (leftFlag == 1){
      completeFlag = 1;
      rightFlag = 1;
      //Serial.write("\ngoing right");
    }
    else {
      completeFlag = 0;
      rightFlag = 1;
    }
  }
  if ((leftMillis < rightMillis) && completeFlag == 1) { //person goes left to right i.e. person enters
    //Serial.write("\nleft to right");
    population++;
    totalEntered++;
    leftFlag = 0;
    rightFlag = 0;
    leftMillis = 10000000;
    rightMillis = 10000000;
    completeFlag = 0;
    Serial.print("\ntoatl population: ");
    Serial.print(population);
    delay(1000);
  }
  else if ((leftMillis > rightMillis) && completeFlag == 1){ //person goes left to right i.e. person leaves
    //Serial.println("\nright to left");
    population--;
    totalExited++;
    leftFlag = 0;
    rightFlag = 0;
    leftMillis = 10000000;
    rightMillis = 10000000;
    completeFlag = 0;
    Serial.print("\ntoatl population: ");
    Serial.print(population);
    delay(1000);
  }


  Serial.flush();
  //Serial.println(left);
}


