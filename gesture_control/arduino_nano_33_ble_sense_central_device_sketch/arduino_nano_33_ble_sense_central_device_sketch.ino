#include <ArduinoBLE.h>
#include <Arduino_APDS9960.h>

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";
  
int gesture = -1; 
int oldGestureValue = -1;   

void setup() {
  
  Serial.begin(9600);
  while (!Serial);

  if (!APDS.begin()) {
    Serial.println("Error initializing APDS9960 sensor!");
  }

   APDS.setGestureSensitivity(80); // [1..100]
  
  // begin ble initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  Serial.println("BLE Central - gesture control");

}

void loop() {
  
     connectToPeripheral();
}


void connectToPeripheral(){

  BLEDevice peripheral;

  do
  {
     // start scanning for peripherals
    BLE.scanForUuid(deviceServiceUuid);
    peripheral = BLE.available();
    
  } while (!peripheral);

  
  if (peripheral) {
    // discovered a peripheral, print out address, local name, and advertised service
    Serial.print("Found  ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();
  
    // stop scanning
    BLE.stopScan();
  
    controlPeripheral(peripheral);
   
  }
  
}

void controlPeripheral(BLEDevice peripheral) {

  
  // connect to the peripheral
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

  // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

  BLECharacteristic gestureCharacteristic = peripheral.characteristic(deviceServiceCharacteristicUuid);
    
  if (!gestureCharacteristic) {
    Serial.println("Peripheral does not have gesture characteristic!");
    peripheral.disconnect();
    return;
  } else if (!gestureCharacteristic.canWrite()) {
    Serial.println("Peripheral does not have a writable gesture characteristic!");
    peripheral.disconnect();
    return;
  }

  
  while (peripheral.connected()) {

    gesture = gestureDetectection();

    if (oldGestureValue != gesture) {
      
      // gesture value changed
      oldGestureValue = gesture;
          
      Serial.print("writing value: ");
      Serial.println(gesture);
      gestureCharacteristic.writeValue((byte)gesture);
      Serial.println("done!");

    }
  
  }

  Serial.println("Peripheral disconnected!");

}
  
int gestureDetectection(){

  if (APDS.gestureAvailable()) {
    
    // a gesture was detected
    gesture = APDS.readGesture();

    switch (gesture) {
      case GESTURE_UP:
        Serial.println("Detected UP gesture");
        break;
      case GESTURE_DOWN:
        Serial.println("Detected DOWN gesture");
        break;
      case GESTURE_LEFT:
        Serial.println("Detected LEFT gesture");
        break;
      case GESTURE_RIGHT:
        Serial.println("Detected RIGHT gesture");
        break;
      default:
        Serial.println("NO gesture detected!");
        break;
      }
    
    }

    return gesture;
    
}
