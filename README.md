# kineticsculpture

For this project, Leah and I wanted to create a sculpture of how we looked doing our homework, studying, and working on projects together. It resulted in just our arms moving up and down, and our eyes spinning signifying our working together and looking for errors until our eyes spin, giving more of a Walking Dead vibes or starring in Michael Jackson’s “Thriller”. 

## Run a program

To run this program on your ESP32, download the file and click on the arrow `—>` icon at the top left corner. The program should compile and run immediately barring any errors that may occur regarding the set-up of the ESP32 in the Arduino IDE. Ensure that you have the correct port selected.

## API Usage

Part of the goal of this project was to utilize an API that will allow the whole class to start the movement of the sculpture at the same time. On an individual level, it will allow you to press a button that will have the sculpture run for 30 seconds at a time. 
To do so, you would have to enter `http://134.122.113.13/UNI`   into your browser, where `UNI` is your UNI. After compiling your code on Arduino, you would press the “Run Sculpture for 30s” button at this link for the sculpture to run. 

## The code

There are about 4 main sections to the code for this exhibition:

- The first section is dedicated to defining global variables, shortcuts, and invoking certain libraries
    - Here you would specify the pins that each of the motors are occupying and initialize any global variables used. For example
    
    ```arduino
    int outPorts[] = {33, 27, 26, 25}; //Step motor
    int servoPin2 = 35; // Servo motor 
    TaskHandle_t Task1; // Task initialization for Multicore Processing
    ```
    
    - This is also where you specify the WiFi network you will be using, and give the password to grant access to the program to connect to the WiFi so that the API can be utilized. In this example, the WiFi used is that of my dorm building.
        
        ```arduino
        const char *ssid_Router     = "Fios-P9AQG" ; //Enter the router name
        const char *password_Router = "mind86gym2680would"; //Enter the router password
        ```
        
- `void setup(void)`
    - Initializes the program and connects to the WiFi network provided above.
    
    ```arduino
    WiFi.begin(ssid_Router, password_Router);
      USE_SERIAL.println(String("Connecting to ")+ssid_Router);
      while (WiFi.status() != WL_CONNECTED){
        delay(500);
        USE_SERIAL.print(".");
      }
    ```
    
    - Attaches the motor pins used to the motor objects and sets the pins for the motors to the output
    
    ```arduino
    for (int i = 0; i < 4; i++) {
        pinMode(outPorts[i], OUTPUT);
      }
    myservo2.setPeriodHertz(50);           // standard 50 hz servo
    myservo2.attach(servoPin2, 500, 2500);
    ```
    
    - Utilizing Multicore Processing to allow for the motors to run in parallel
    
    ```arduino
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
    ```
    
- Defining  of Task Code
    - Here we are splitting the movement and code for the motors into 2 tasks so that both the servo and the stepper can function in parallel.  Because we already initialized, created the task object, and store the handle, we can then define what each task would be responsible for. Here is an example of the task that controls the servo motors.
    
    ```arduino
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
    ```
    
- `void loop()`    
    ```arduino
    if((WiFi.status() == WL_CONNECTED)) {
        HTTPClient http;
        http.begin(address);
        int httpCode = http.GET();
    ```
    

> Note that these code snippets do not represent the code for all the motors, as code was duplicated and changes were made accordingly. More details about the technical implementation can be found here


we had to connect 6 total wires for each of the stepper motors. These are the connections for one of the stepper motors. 

For the Stepper Motor:
- Power( + ) connects to the 5V pin
- Black wire connects to the Ground pin
- IN4 connects to pin 25
- IN3 connects to pin 26
- IN2 connects to pin 27
- IN1 connects to pin 33 (there is no 14 on our ESP32)

For the Servo Motors:
- Each of the servo motors connected to a Ground and Power (5V) pin
- The motors differed in the third pin used.
    - For the sculpture that represents Rania’s eyes, we used pins 15 and 35.

