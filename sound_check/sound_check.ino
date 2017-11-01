void setup() {
  Serial.begin(9600);
}

const int sampleWindow = 25;
unsigned int sample;
unsigned long counter=1;
double volts = 0;
double totalVolts = 0;
unsigned long totalCycles = 0;
unsigned long seconds;

void loop() {
  seconds = millis();
  while (millis() - seconds <= 3000){ //run this loop for 3 seconds
    unsigned long startMillis = millis();
    unsigned int peakToPeak = 0;
    unsigned int signalMax = 0;
    unsigned int signalMin = 1024;
    
    while (millis() - startMillis < sampleWindow){
      
      sample = analogRead(0);
      if (sample < 1024){
        if (sample > signalMax){
          signalMax = sample;
        }
        else if (sample < signalMin){
          signalMin = sample;
        }
      }
    }
    peakToPeak = signalMax - signalMin;
    //volts += (peakToPeak * 5.0) / 1024;
    totalVolts += (peakToPeak * 5.0) / 1024;
    totalCycles++;
    //volts = volts / counter;
//    counter++;
//
//    if (counter == 5){
//      volts = 0;
//      counter = 1;
//    }
  }
  float averageVolts = totalVolts / totalCycles;
  //Serial.println(averageVolts);
  if (averageVolts <= 1.9){ Serial.print(0);}
  else if (averageVolts > 1.9 && averageVolts < 2.5){ Serial.print(1);}
  else if (averageVolts >= 2.5 && averageVolts < 3.1){ Serial.print(2);}
  else if (averageVolts >= 3.1){ Serial.print(3);}
  
  delay(10);
  Serial.flush();
  totalVolts = 0;
  totalCycles = 0;
}
