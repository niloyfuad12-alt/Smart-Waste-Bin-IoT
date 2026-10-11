#include <NewPing.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
#include <Servo.h>

Servo myservo;
int dispUpTime = 0;
int pos = 0;    // variable to store the servo position

float smokepin = A0;  //somke sensor pin
float gaspin = A1;  // gas sensor pin
float smokevalue = 0;
float gasvalue = 0;

//int pirpin = 10;    //PIR sensor PIN
int lidFeedbackPin = 12;
int trig1 = 5;  //sonar disposal paper
int echo1 = 4;

int trig2 = 7;  // sonar food
int echo2 = 6;
unsigned long lastUp = 0;
unsigned long lastBatUp = 0;

int trig3 = 8;  // outside
int echo3 = 9;
NewPing sonar1(trig1, echo1, 200);
NewPing sonar2(trig2, echo2, 200);
NewPing sonar3(trig3, echo3, 200);

float distance1, duration1, distance2, duration2, distance3, duration3;

int analogPin = A2; // Current sensor output

const int averageValue = 120;
long int sensorValue = 0;  // variable to store the sensor value read
float v = 0;
float voltage = 0, current = 0;
float binP1 = 0, binP2 = 0;
//MILLIS funcrtion
unsigned long prevTime_T1 = millis();     //for LCD
long interval_T1 = 15000;                 // every 15 sec

unsigned long prevTime_T2 = millis();     //for actuator
long interval_T2 = 10000;                  // for 10 sec
void stopSystem() {
  // Stop low voltage
  if (lastBatUp == 0 || millis() - lastBatUp > 10000)
    while (voltage < 12.8) {
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.println("  Battery too Low!");
      lcd.println("  Recharge Battery");
      lastBatUp = millis();
      delay(10000);
      updateVI();
    }
}
void updateFill() {
  if (digitalRead(12) == 0) {
    binP1 = sonar1.ping_cm();
    binP2 = sonar2.ping_cm();

 

    if (binP1 > 57) {
      binP1 = 0;
    }
    else {
      binP1 = ((57 - binP1) / 57.0) * 100.0;
    }
    if (binP2 > 51) {
      binP2 = 0;
    }
    else {
      binP2 = ((51 - binP2) / 51.0) * 100.0;
    }

  }
}
void disp1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DIS:" + String(binP1, 0) + "%  | NDIS:" + String(binP2, 0) + "%");
  lcd.setCursor(0, 1);

  if (gasvalue > 150) {                 //Smoke trip
    lcd.print("GAS: Yes");
  }
  else {
    lcd.print("GAS: No ");
  }
  if (smokevalue > 150) {
    lcd.print(" | SMOKE: Yes");
  }
  else {
    lcd.print(" | SMOKE: No");
  }
  lcd.setCursor(0, 2);
  lcd.print("V:" + String(voltage, 2) + "V | I:" + String(current, 3) + "A");
  lcd.setCursor(0, 3);
  lcd.print("P:" + String(voltage * current, 2) + "W| B:" + String((voltage / 16.8) * 100, 0) + "%"); //battary
}
void setup()
{
  updateVI();
  stopSystem();
  Serial.begin(9600);
  // lcd.init();
  lcd.begin();
  lcd.backlight();
  pinMode(smokepin, INPUT);   //somke pin as input
  pinMode(gaspin, INPUT);     // gas pin as input
  //  pinMode(pirpin, INPUT);
  pinMode(lidFeedbackPin, INPUT_PULLUP);
  /*
    sonar1();
    sonar2();
    sonar3();
  */
  //RELAY ACTUALTOR
  pinMode(2, OUTPUT);       // top relay
  pinMode(3, OUTPUT);       // 2nd relay
  digitalWrite(2, HIGH);    // off
  digitalWrite(3, HIGH);    // off
  myservo.attach(11);
  myservo.write(0);
  while (digitalRead(12) != 0) {
    closeLid();
  }
  updateFill();
  digitalWrite(3, HIGH); // EXTEND actuator
  digitalWrite(2, HIGH);
  disp1();

}
void openLid() {
  digitalWrite(3, LOW); // EXTEND actuator
  digitalWrite(2, HIGH);
  for (int m = 0; m < 40; m++) {
    delay(200);
  }
  digitalWrite(3, HIGH); // EXTEND actuator
  digitalWrite(2, HIGH);
}
void closeLid() {
  digitalWrite(2, LOW); //Close
  digitalWrite(3, HIGH);
  updateVI();
}
void handle2ndLid() {
  //Serial.println("Servo open");
  for (pos = 0; pos <= 90; pos++) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
  int i = 0;
  while (i < 10) {
    delay(1000);
    //int sensorValue = digitalRead(pirpin);  //PIR
    //if (sensorValue == 1) {
    // i=5;
    //}
    i++;
  }
  //Serial.println("Servo close");
  for (pos = 90; pos >= 1; pos--) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
}
void updateVI() {
  if (lastUp == 0 || millis() - lastUp > 1000) {
    //measure voltage
    voltage = 0;
    for (int l = 0; l < 30; l++) {
      voltage = voltage + analogRead(A3);
      delay(2);
    }
    voltage = voltage / 30;
    voltage = voltage * (5.0 / 1024.0) * ((2.62 + 9.65) / 2.62);
    voltage = voltage - 0.25;
    stopSystem();
    for (int i = 0; i < averageValue; i++)
    {
      sensorValue += analogRead(analogPin);

      // wait 2 milliseconds before the next loop
      delay(2);
    }

    sensorValue = sensorValue / averageValue;
    v = sensorValue * 5.0 / 1024.0;
    current = (v - 2.5) / (-0.185);
    //current= (-1.00)*current;
    //Serial.println("V:" + String(voltage, 3) + "   I:" + String(current, 3) + "mA");
    smokevalue = analogRead(smokepin);      //SMOKE SENSOR CODE IS HERE
    gasvalue = analogRead(gaspin);
    if (smokevalue > 150) {                 //Smoke trip
     // Serial.println("Smoke detected");
    }


    if (gasvalue > 150) {                 //GAS trip
      //Serial.println("Gas detected");
      //delay (5000);
    }
    //binp1, binp2, smokevalue, gasvalue, battary String((voltage / 16.8) * 100, 0) 

Serial.println(String(binP1,0)+"p"+String(binP2,0)+"s"+String(smokevalue,0)+"g"+String(gasvalue,0)+"b"+String((voltage/16.8)*100,0));
    
    lastUp = millis();
  }
  dispUpTime++;
  if (dispUpTime == 10) {
    disp1();
    dispUpTime = 0;
  }
}
void loop()
{
  if (sonar3.ping_cm() < 30) {
    //Serial.println("In detect");
    unsigned long detectTime = millis();
    int detect = 0;
    while (sonar3.ping_cm() < 30 && millis() - detectTime < 5000) {
      detect++;
      if (detect > 9) {
        break;
      }
      delay(500);
    }
    if (detect > 9) {
      openLid();
      for (int j = 0; j < 150; j++) {
      //  Serial.println("Loop:" + String(j) + " Dis:" + String(sonar1.ping_cm()));

        if (sonar1.ping_cm() > 3 && sonar1.ping_cm() < 10) {
          handle2ndLid();
        }
        else {
          myservo.write(pos == 0);
        }
        delay(100);
      }
      //Lid close
      while (digitalRead(12) != 0) {
        closeLid();
      }
      digitalWrite(3, HIGH); //
      digitalWrite(2, HIGH);
      updateFill();
    }

  }

  updateVI();
  stopSystem();
  /*
    unsigned long currentTime = millis();   // MILLIS

    int sensorValue = digitalRead(pirpin);  //PIR

    smokevalue = analogRead(smokepin);      //SMOKE SENSOR CODE IS HERE
    gasvalue = analogRead(gaspin);
    //Serial.println(smokevalue);           //PRINTING SMOKE VALUE IN SERIAL MONITOR

    digitalWrite(trig1, LOW);                  //SONAR disposal paper
    delay(2);
    digitalWrite(trig1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig1, LOW);
    duration1 = pulseIn(echo1, HIGH);
    distance1 = (duration1 * 0.01338582677) / 2;

    digitalWrite(trig2, LOW);                  //SONAR food
    delay(2);
    digitalWrite(trig2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig2, LOW);
    duration2 = pulseIn(echo2, HIGH);
    distance2 = (duration2 * 0.01338582677) / 2;

    digitalWrite(trig3, LOW);                  //SONAR outside
    delay(2);
    digitalWrite(trig3, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig3, LOW);
    duration3 = pulseIn(echo3, HIGH);
    distance3 = (duration3 * 0.01338582677) / 2;

    if (currentTime - prevTime_T2 > interval_T2) {      //10 sec interval
    if (distance3 < 10) {
      digitalWrite(3, LOW); // EXTEND actuator
      digitalWrite(2, HIGH);
      delay(5000);

      if (sensorValue == 1) {
        for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15 ms for the servo to reach the position
        }
      }

      else {
        //for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
        myservo.write(pos == 0);            // tell servo to go to position in variable 'pos'
        //delay(15);                       // waits 15 ms for the servo to reach the position
      }
    }
    prevTime_T2 = currentTime;
    }
    else {                                        //**** PROBLEM******
    digitalWrite(3, HIGH); // Retract actuator
    digitalWrite(2, LOW);
    delay(5000);
    }

    if (currentTime - prevTime_T1 > interval_T1) {
    lcd.setCursor(0, 1);                    // Smoke LCD  //2nd line
    lcd.print("Smoke=");
    lcd.print(smokevalue);
    if (smokevalue > 150) {                 //Smoke trip
      lcd.setCursor(0, 2);                                  //3rd line
      lcd.print("Smoke is Detected");
      //delay (5000);
    }

    lcd.setCursor(11, 1);                    // GAS LCD  //2nd line
    lcd.print("Gas=");
    lcd.print(gasvalue);
    if (gasvalue > 150) {                 //GAS trip
      lcd.setCursor(0, 2);                                  //3rd line
      lcd.print("Gas is Detected");
      //delay (5000);
    }

    lcd.setCursor(0, 0);                //lcd input for Disposal paper  //1st line
    lcd.print("Dis:");
    lcd.print(distance1);
    //lcd.println("INCH");
    if (distance1 < 2 ) {               //Disposal trip
      lcd.setCursor(0, 3);                                             //4th line
      lcd.print("DIS BIN OVER LOADED");
    }

    lcd.setCursor(9, 0);                //lcd input for non-disposal food  //1st line
    lcd.print("N-Dis:");
    lcd.print(distance2);
    //lcd.println("INCH");
    if (distance2 < 2 ) {               //non - Disposal trip
      lcd.setCursor(0, 3);
      lcd.print("NDIS BIN OVER LOADED");
    }
    //lcd.clear();
    prevTime_T1 = currentTime;
    }
    Serial.print(smokevalue);
    Serial.print(distance1);
    Serial.print(distance2);
    Serial.print(gasvalue);
    //open time
  */

}
