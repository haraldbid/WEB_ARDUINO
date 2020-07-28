#include <ArduinoBLE.h>
#include <Arduino_MKRRGB.h>

enum {
  GESTURE_NONE = -1,
  GESTURE_UP = 0,
  GESTURE_DOWN = 1,
  GESTURE_LEFT = 2,
  GESTURE_RIGHT = 3
};

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

int gesture = -1;

// BLE gesture Service
BLEService gestureService(deviceServiceUuid); 

// BLE gesture Switch Characteristic 
BLEByteCharacteristic gestureCharacteristic(deviceServiceCharacteristicUuid, BLERead | BLEWrite);


void setup() {
  
  //Serial.begin(9600);
  //while (!Serial);

  // initialize the display
  MATRIX.begin();
  // set the brightness, supported values are 0 - 255
  MATRIX.brightness(120);
  // configure the text scroll speed
  MATRIX.textScrollSpeed(50);
  MATRIX.clear();
  MATRIX.endDraw();

  
  // begin ble initialization
  if (!BLE.begin()) {
    //Serial.println("starting BLE failed!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Gesture peripheral");
  BLE.setAdvertisedService(gestureService);

  // add the characteristic to the service
  gestureService.addCharacteristic(gestureCharacteristic);

  // add service
  BLE.addService(gestureService);

  // set the initial value for the characeristic:
  gestureCharacteristic.writeValue(-1);

  // start advertising
  BLE.advertise();

  //Serial.println("BLE gesture Peripheral");
}

void loop() {
  
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    
    //Serial.print("Connected to central: ");
    // print the central's MAC address:
    //Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      
      // if the remote device wrote to the characteristic,
      if (gestureCharacteristic.written()) {
         gesture = gestureCharacteristic.value();
         writeGesture(gesture);
       }
      
    }

    // when the central disconnects, print it out:
    //Serial.print(F("Disconnected from central: "));
    //Serial.println(central.address());
  }
}

void writeGesture(int gesture)
{

  int R = 0;
  int G = 0;
  int B = 0;
  String text = "";
  
   switch (gesture) {
    
      case GESTURE_UP:
        text =" UP";
        R=0;
        G=255;
        B=0;
        break;
      case GESTURE_DOWN:
        text ="DOWN";
        R=255;
        G=0;
        B=0;
        break;
      case GESTURE_LEFT:
        text ="LEFT";
        R=0;
        G=0;
        B=255;
        break;
      case GESTURE_RIGHT:
        text ="RIGHT";
        R=207;
        G=9;
        B=227;
        break;
      default:
        text ="";
        R=0;
        G=0;
        B=0;
        break;
        
    }
    
    MATRIX.clear();
    MATRIX.endDraw();
    MATRIX.beginText(0, 0, R, G, B); // X, Y, then R, G, B
    MATRIX.print(text);
    MATRIX.endText(SCROLL_LEFT);
      
}