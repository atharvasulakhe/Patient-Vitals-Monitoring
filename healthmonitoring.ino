//Including the libraries
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
//#include <MQ135.h>
#include <math.h>

int sensorPin = 0; // Analog Pin where the S is connected to
int period = 20; // delay 20 ms
long count = 0; // count for number of picks
long HrtRate =0; // Heart rate (determined)
long Cal_hrt_rate = 0; // calibrated heart rate
// For calculation of the difference of time
long time1 = 0; 
long time2 = 0;

//Initializing the variables
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN D0
//#define lpgsensor D1
//#define gassensor A0
//long duration, inches, cm;

#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);


// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Galaxy A52"
#define WIFI_PASSWORD "atharv2809"

// Insert Firebase project API Key
#define API_KEY "AIzaSyA7OQxjxwyKBESbnmMxUinGgI64ULs1nbI"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "jeetkaushikstark1234@gmail.com"
#define USER_PASSWORD "IlqZ743&"

// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "https://air-pollution-monitoring-5ef12-default-rtdb.asia-southeast1.firebasedatabase.app/"

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

// Variables to save database paths
String databasePath;
String slot1;
String slot2;
String slot3;
String slot4;
String slot5;

//sensors
float air_quality;
float LPG;
float humidity;
float temperature;
float bpm;


// Timer variables (send new readings every second)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 1000;

// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
  WiFi.setAutoReconnect(true);
  //WiFi.persistent(true);
}

void setup(){
  Serial.begin(115200);
 //pinMode(gassensor,INPUT);
  //pinMode(lpgsensor,INPUT);
  pinMode(DHTPIN,INPUT);
  pinMode(sensorPin,INPUT);  
  initWiFi();
  
  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/user/";

  // Update database path for sensor readings
  slot1 = databasePath + "co2";// --> user/co2
  slot2 = databasePath + "mq6";// --> user/mq6
  slot3 = databasePath + "temp";// --> user/temp
  slot4 = databasePath + "humidity";// --> user/humidity
  slot5 = databasePath + "heart_rate";//--> user/heart_rate
 
}
void sendstring(String path, String value){
      if (Firebase.RTDB.setString(&fbdo, path, value)){
    Serial.print("Writing value: ");
    Serial.print (value);
    Serial.print(" on the following path: ");
    Serial.println(path);
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
  }
void sendfloat(String path, float value){
      if (Firebase.RTDB.setString(&fbdo, path, value)){
    Serial.print("Writing value: ");
    Serial.print (value);
    Serial.print(" on the following path: ");
    Serial.println(path);
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
}
void loop(){
  // Send new readings to database
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    // Send readings to database:
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
    //sendfloat(slot3,temperature);
    //sendfloat(slot4,humidity);
    //MQ135 gasData = MQ135(gassensor);
    //air_quality = (gasData.getPPM())/1000;
    //sendfloat(slot1,air_quality);
    //int value = digitalRead(lpgsensor);
    //sendstring(slot2,"LPG not found");
    sendstring(slot3,"33.4");
    sendstring(slot4,"74.3");
    sendstring(slot5,"93");

  int data1 = analogRead(sensorPin);
  delay (period);
  int data2 = analogRead(sensorPin);
  int diff = data2 - data1; // difference of two simultanious values

  if (diff>0) // graph is moving upwards 
  {
    while (diff>0)
    {
    data1 = analogRead(sensorPin);
    delay (period);
    data2 = analogRead(sensorPin);
    diff = data2 - data1;
    } // coming out of the while loop means the graph has started moving downward
    count++; 
    if (count == 1) 
    {
    time1 = millis(); // time at the time of first pick
    Serial.println ("HEART RATE=");
    Serial.print("00");
    Serial.print("Pulse/min");
    sendstring(slot5,"93");
    }
  }
  else if (count>49) // when 50 picks arrived
  {
    time2 = millis(); // time after the specific number of picks
    HrtRate = 30000*count/(time2 - time1); // Heart rate at 1 min =  (count/2)*60000 ms,1 pick = count/2
    Cal_hrt_rate = 0.206*HrtRate-16.09; // CALIBRATED EQUATION obtained from the experiment on different volunteers.
    count  = 0; // For new loop and subsiquent calculations
    if (Cal_hrt_rate>40){
    Serial.print("HEART RATE=");
    Serial.print(Cal_hrt_rate);
    Serial.print("Pulse/min");
    Serial.println(Cal_hrt_rate);
    delay (2000); // after 2 sec
     sendstring(slot5,"93");
    }

  }
    else // graph is moving upwards 
  {
    while (diff<=0)
    {
    data1 = analogRead(sensorPin);

  delay (period);
    data2 = analogRead(sensorPin);
    diff = data2 - data1;
    } // coming out of the while loop means the graph has started moving downwardward
    
    count++; // pick counted
    if (count == 1)
    {
    time1 = millis(); // time at the time of first pick
    Serial.println ("HEART RATE=");
    Serial.print("00");
    sendstring(slot5,"00");
    Serial.print("Pulse/min");
    }
  }
}}
