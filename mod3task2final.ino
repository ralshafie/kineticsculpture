/**********************************************************************
  Filename    : Drive Stepper Motor
  Description : Use ULN2003 to drive the stepper motor.
  Auther      : www.freenove.com
  Modification: 2020/07/11
**********************************************************************/
//for servo
#include <ESP32Servo.h>
#include <WiFi.h>
#include <HTTPClient.h>
#define USE_SERIAL Serial

Servo myservo1;  // create servo object to control a servo
Servo myservo2;

int pos = 0;    // variable to store the servo position
int servoPin1 = 15; // Servo motor pin
int servoPin2 = 32; // Servo motor pin

TaskHandle_t Task1; 
TaskHandle_t Task2; 


// Conncet the port of the stepper motor driver

const char *ssid_Router     = "Barnard Guest" ; //Enter the router name
const char *password_Router = ""; //Enter the router password

int outPorts[] = {33, 27, 26, 25};
String address= "http://134.122.113.13/rta2114/running";


void setup() {

  USE_SERIAL.begin(115200);

  WiFi.begin(ssid_Router, password_Router);
  USE_SERIAL.println(String("Connecting to ")+ssid_Router);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    USE_SERIAL.print(".");
  }
  USE_SERIAL.println("\nConnected, IP address: ");
  USE_SERIAL.println(WiFi.localIP());
  USE_SERIAL.println("Setup End");

  
  // set pins to output
  for (int i = 0; i < 4; i++) {
    pinMode(outPorts[i], OUTPUT);
  }
  myservo1.setPeriodHertz(50);           // standard 50 hz servo
  myservo1.attach(servoPin1, 500, 2500);  // attaches the servo on servoPin to the servo object

  myservo2.setPeriodHertz(50);           // standard 50 hz servo
  myservo2.attach(servoPin2, 500, 2500);

  xTaskCreatePinnedToCore(
    Task1code,
    "Task1",
    1000,
    NULL,
    1,
    &Task1,
    0
  ); 
  delay(500);

  xTaskCreatePinnedToCore(
    Task2code,
    "Task2",
    1000,
    NULL,
    1,
    &Task2,
    1
  ); 
  delay(500); 
}

void Task1code(void * pvParameters){
  Serial.println("Task1 is running on core ");
  while(true){
    for(pos = 0; pos < 180; pos += 1){
      myservo1.write(pos); 
      myservo2.write(pos);// tell servo to go to position in variable 'pos'
      delay(15); // waits 15ms for the servo to reach the position 
    }
    for(pos = 180; pos > 0; pos -= 1){
      myservo1.write(pos); 
      myservo2.write(pos);// tell servo to go to position in variable 'pos'
      delay(15); // waits 15ms for the servo to reach the position 
    }
  }
  
}

void moveSteps(bool dir, int steps, byte ms) {
  for (unsigned long i = 0; i < steps; i++) {
    moveOneStep(dir); // Rotate a step
    delay(constrain(ms,3,20));        // Control the speed
  }
}

void moveOneStep(bool dir) {
  // Define a variable, use four low bit to indicate the state of port
  static byte out = 0x01;
  // Decide the shift direction according to the rotation direction
  if (dir) {  // ring shift left
    out != 0x08 ? out = out << 1 : out = 0x01;
  }
  else {      // ring shift right
    out != 0x01 ? out = out >> 1 : out = 0x08;
  }
  // Output singal to each port
  for (int i = 0; i < 4; i++) {
    digitalWrite(outPorts[i], (out & (0x01 << i)) ? HIGH : LOW);
  }
}

void moveAround(bool dir, int turns, byte ms){
  for(int i=0;i<turns;i++)
    moveSteps(dir,32*64,ms);
}
void moveAngle(bool dir, int angle, byte ms){
  moveSteps(dir,(angle*32*64/360),ms);
}

void Task2code(void * pvParameters){
  Serial.println("Task1 is running on core ");
   while(true){
     moveAngle(true, 60, 3); 
     moveAngle(false, 120, 3);
     moveAngle(true, 160, 3); 
     moveAngle(false, 60, 3);
   }
   
}

void loop() {
  if((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;
    http.begin(address);

 
    int httpCode = http.GET(); // start connection and send HTTP header, FAILING!?
    if (httpCode == HTTP_CODE_OK) { 
        String response = http.getString();
        if (response.equals("false")) {
            // Do not run sculpture, perhaps sleep for a couple seconds
            delay(500);
        
       } else if(response.equals("true")) {
            // Run sculpture
//            Task1code(); 
//            Task2code(); 

//
//            moveAngle(true, 40, 3); 
//            moveAngle(false, 40, 3);
//            
//            //servo
//            for(pos = 0; pos < 180; pos += 1){
//              myservo1.write(pos); 
//              myservo2.write(pos);// tell servo to go to position in variable 'pos'
//              delay(15); // waits 15ms for the servo to reach the position 
//            }
//            for(pos = 180; pos > 0; pos -= 1){
//              myservo1.write(pos); 
//              myservo2.write(pos);// tell servo to go to position in variable 'pos'
//              delay(15); // waits 15ms for the servo to reach the position 
//            }
//
//           
//            // stepper
//            //moveSteps(true, 32*32, 3);
//            //moveSteps(false, 32*32, 3);
//            
//            
//
        }
        USE_SERIAL.println("Response was: " + response);
    } else {
        USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", "error"); //http.errorToString(httpCode).c_str()
    }
    http.end();
    //delay(500); // sleep for half of a second
  }
}



//20104rat
//Suggestion: the motor turns precisely when the ms range is between 3 and 20
//void moveSteps(bool dir, int steps, byte ms) {
//  for (unsigned long i = 0; i < steps; i++) {
//    moveOneStep(dir); // Rotate a step
//    delay(constrain(ms,3,20));        // Control the speed
//  }
//}
//
//void moveOneStep(bool dir) {
//  // Define a variable, use four low bit to indicate the state of port
//  static byte out = 0x01;
//  // Decide the shift direction according to the rotation direction
//  if (dir) {  // ring shift left
//    out != 0x08 ? out = out << 1 : out = 0x01;
//  }
//  else {      // ring shift right
//    out != 0x01 ? out = out >> 1 : out = 0x08;
//  }
//  // Output singal to each port
//  for (int i = 0; i < 4; i++) {
//    digitalWrite(outPorts[i], (out & (0x01 << i)) ? HIGH : LOW);
//  }
//}
//
//void moveAround(bool dir, int turns, byte ms){
//  for(int i=0;i<turns;i++)
//    moveSteps(dir,32*64,ms);
//}
//void moveAngle(bool dir, int angle, byte ms){
//  moveSteps(dir,(angle*32*64/360),ms);
//}
