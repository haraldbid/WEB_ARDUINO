/*
  Nano 33 BLE Sense Getting Started
  A BLE peripheral broadcasting temperatature and humidity readings that can be viewed
  on a mobile phone. On-board LED indicates a BLE connection.
  Adapted from Arduino BatteryMonitor example by Peter Milne
*/

#include <ArduinoBLE.h>
#include <Arduino_HTS221.h>

#define D2   2
#define D3   3
#define D4   4


const int UPDATE_FREQUENCY = 2000; // Update frequency in ms
const float CALIBRATION_FACTOR = -4.0; // Temperature calibration factor (celcius)

int previousTemperature = 0;
unsigned int previousHumidity = 0;
long previousMillis = 0;  // last time readings were checked, in ms

int oldReadVal = 0;
int readVal = 0;


BLEService environmentService("181A");  // Standard Environmental Sensing service

BLEIntCharacteristic tempCharacteristic("2A6E",  // Standard 16-bit Temperature characteristic
  BLERead | BLENotify); // Remote clients can read and get updates

BLEUnsignedIntCharacteristic humidCharacteristic("2A6F", // Unsigned 16-bit Humidity characteristic
  BLERead | BLENotify);

void setup() {
  Serial.begin(9600);    // Initialize serial communication
  while (!Serial);

  if (!HTS.begin()) {  // Initialize HTS22 sensor
    Serial.println("Failed to initialize humidity temperature sensor!");
    while (1);
  }

  pinMode(LED_BUILTIN, OUTPUT); // Initialize the built-in LED pin

  if (!BLE.begin()) {   // Initialize BLE
    Serial.println("starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("Nano33BLESENSE");  // Set name for connection
  BLE.setAdvertisedService(environmentService); // Advertise environment service
  environmentService.addCharacteristic(tempCharacteristic); // Add temperature characteristic
  environmentService.addCharacteristic(humidCharacteristic); // Add humidity chararacteristic
  BLE.addService(environmentService); // Add environment service
  tempCharacteristic.setValue(0); // Set initial temperature value
  humidCharacteristic.setValue(0); // Set initial humidity value

  BLE.advertise();  // Start advertising
  Serial.print("Peripheral device MAC: ");
  Serial.println(BLE.address());
  Serial.println("Waiting for connections...");

  pinMode(D2,INPUT);
  pinMode(D3,OUTPUT);
  pinMode(D4,OUTPUT);

  led_disconnected();
}

void loop() {
  BLEDevice central = BLE.central();  // Wait for a BLE central to connect

  // If central is connected to peripheral
  if (central) {
    Serial.print("Connected to central MAC: ");
    Serial.println(central.address());    // Central's BT address:
    // Turn on the LED to indicate the connection:
    digitalWrite(LED_BUILTIN, HIGH);
    led_connected();
    while (central.connected()) {
      long currentMillis = millis();
      // After UPDATE_FREQUENCY ms have passed, check temperature & humidity
      if (currentMillis - previousMillis >= UPDATE_FREQUENCY) {
        previousMillis = currentMillis;
        updateReadings();
      }
    }

    // When the central disconnects, turn off the LED
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Disconnected from central MAC: ");
    Serial.println(central.address());
  }
  led_disconnected();
    /*
   * Detect Forward edge (low -> High)
   */
  readVal = digitalRead(D2);
  if(oldReadVal == 0 && readVal == 1){
    Serial.println("Forward Edge");
    
  }
  oldReadVal = readVal; 
}

int getTemperature(float calibration) {
  // Get calibrated temperature as signed 16-bit int for BLE characteristic
  return (int) (HTS.readTemperature() * 100) + (int) (calibration * 100);
}

unsigned int getHumidity() {
  // Get humidity as unsigned 16-bit int for BLE characteristic
  return (unsigned int) (HTS.readHumidity() * 100);
}

void updateReadings() {
  // Read the HTS22 temperature and humidity
  int temperature = getTemperature(CALIBRATION_FACTOR);
  unsigned int humidity = getHumidity();

  if (temperature != previousTemperature) {  // If reading has changed
    Serial.print("Temperature: ");
    Serial.println(temperature);
    tempCharacteristic.writeValue(temperature);  // Update characteristic
    previousTemperature = temperature;           // Save value
  }
  
  if (humidity != previousHumidity) {
    Serial.print("Humidity: ");
    Serial.println(humidity);
    humidCharacteristic.writeValue(humidity);
    previousHumidity = humidity;
  }
}

void led_connected(){
  digitalWrite(D3,LOW);
  digitalWrite(D4,HIGH);
}
void led_disconnected(){
  digitalWrite(D3,HIGH);
  digitalWrite(D4,LOW);  
}
