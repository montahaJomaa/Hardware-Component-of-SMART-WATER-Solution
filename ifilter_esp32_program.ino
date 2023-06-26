#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "xxxxx"
#define WIFI_PASSWORD "xxxxx"

// Insert Firebase project API Key
#define API_KEY "xxxxxxx"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "xxxxxxx"
#define USER_PASSWORD "xxxxxx"

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

unsigned long sendDataPrevMillis = 0;  // Previous millis value for sending data
const unsigned long timerDelay = 5000; // Delay between sending data (in milliseconds)

// Variables to save database paths
String databasePath;
String pHPath;
String CODPath;
String ConductivityPath;
String DOPath;
String TSSPath;
String AmmoniumPath;
String ChlorophyllPath;
String ChloridePath;
String ORPPath;


// Generate random pH value
float generateRandompH() {
  return random(0, 14); 
}

// Generate random COD value
float generateRandomCOD() {
  return random(0, 1000); 
}

// Generate random conductivity value
float generateRandomConductivity() {
  return random(0, 100); 
}

// Generate random dissolved oxygen value
float generateRandomDO() {
  return random(0, 20); 
}

// Generate random TSS value
float generateRandomTSS() {
  return random(0, 1000); 
}

// Generate random ammonium value
float generateRandomAmmonium() {
  return random(0, 10); 
}

// Generate random chlorophyll value
float generateRandomChlorophyll() {
  return random(0, 100); 
}

// Generate random chloride value
float generateRandomChloride() {
  return random(0, 100); 
}

// Generate random ORP value
float generateRandomORP() {
  return random(-1000, 1000); 
}

// Initialize random seed
void initRandom() {
  randomSeed(analogRead(0));
}

// Timer function to send new readings every three minutes
void sendRandomDataTimer() {
  unsigned long currentMillis = millis();
  if (currentMillis - sendDataPrevMillis >= timerDelay) {
    sendRandomData();
    sendDataPrevMillis = currentMillis;
  }
}

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
}

// Write float values to the database
void sendFloat(String path, float value){
  
  if (Firebase.RTDB.setFloat(&fbdo, path.c_str(), value)){
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

void setup(){
  Serial.begin(115200);
  
  // Initialize WiFi
  initWiFi();

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task
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
  Serial.print(uid);

  // Update database path
  databasePath = "/UsersData/" + uid;
  
  //Update database path for water parameters random values
  pHPath = databasePath + "/pH";
  CODPath = databasePath + "/COD";
  ConductivityPath = databasePath + "/Conductivity";
  DOPath = databasePath + "/DO";
  TSSPath = databasePath + "/TSS";
  AmmoniumPath = databasePath + "/Ammonium";
  ChlorophyllPath = databasePath + "/Chlorophyll";
  ChloridePath = databasePath + "/Chloride";
  ORPPath = databasePath + "/ORP";
}

void loop(){
  if (Firebase.isTokenExpired()){
    Firebase.refreshToken(&config);
    Serial.println("Refresh token");
  }
 
  // Send new readings to database
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

  
     // Generate random readings
  float pH = generateRandompH();
  float COD = generateRandomCOD();
  float conductivity = generateRandomConductivity();
  float dissolvedOxygen = generateRandomDO();
  float tss = generateRandomTSS();
  float ammonium = generateRandomAmmonium();
  float chlorophyll = generateRandomChlorophyll();
  float chloride = generateRandomChloride();
  float orp = generateRandomORP();
  
    // Send readings to the database
    sendFloat(pHPath, pH);
    sendFloat(CODPath, COD);
    sendFloat(ConductivityPath, conductivity);
    sendFloat(DOPath, dissolvedOxygen);
    sendFloat(TSSPath, tss);
    sendFloat(AmmoniumPath, ammonium);
    sendFloat(ChlorophyllPath, chlorophyll);
    sendFloat(ChloridePath, chloride);
    sendFloat(ORPPath, orp);
    
  }
}
