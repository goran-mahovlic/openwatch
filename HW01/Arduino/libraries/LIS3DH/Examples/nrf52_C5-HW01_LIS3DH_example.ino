/*
 * LIS3DH example for C5-HW01 nrf52832 based smartwatch using Sparkfun's LIS3DH library and Sandeep's Arduino Core
 * I modified Sandeep's Wire.h to make assigning I2C pins on the fly a little easier
 * Curt White
 */

/********************************************************************************************************/
/************************ INCLUDES **********************************************************************/
/********************************************************************************************************/
#include <SPI.h>
#include <Wire.h>
#include <BLEPeripheral.h>
#include <BLEUtil.h>
#include "LIS3DH.h"  //Sparkfun library

/********************************************************************************************************/
/************************ CONSTANTS / SYSTEM VAR ********************************************************/
/********************************************************************************************************/
bool  debug = true;        //turn serial on/off to get data or turn up sample rate

/********************************************************************************************************/
/************************ DEFINITIONS *******************************************************************/
/********************************************************************************************************/

#define PIN_SERIAL_RX             21
#define PIN_SERIAL_TX             23

#define VIBRATE_PIN               25

//LIS3DH Accelerometer
#define LIS3DH_SCL_PIN            13
#define LIS3DH_SDA_PIN            14
#define LIS3DH_SDO_PIN            11
#define LIS3DH_CS_PIN             12
#define LIS3DH_INT1_PIN           15
#define LIS3DH_INT2_PIN           16

//dummy LED pin for BLE
#define LED_PIN                   3

/********************************************************************************************************/
/************************ VARIABLES *********************************************************************/
/********************************************************************************************************/
  //Time
  int   speedHz = 15;                 //default loop speed - native max loop speed is 62.5 ms or 16Hz
  float speedMs = (1/speedHz)*1000;   //native max loop speed is 62.5 ms or 16Hz
  
  //Timestamp
   float clocktime;
    
  //Bluetooth
   unsigned long microsPerReading, microsPrevious;
   float accelScal;

  //LIS3DH
   float           acc[3];
   double          pitch;
   double          roll;

  //System
  int varState = 0; //variable state controlled in app and appended to data stream
 
/********************************************************************************************************/
/************************ DECLARATIONS ******************************************************************/
/********************************************************************************************************/
//LIS3DH Accelerometer
LIS3DH IMU; //Default constructor is I2C, addr 0x19.

//Bluetooth
// create peripheral instance, see pinouts above
BLEPeripheral blePeripheral = BLEPeripheral();

// create service
//BLEService customService =    BLEService("FFFF");
BLEService customService =    BLEService("a000");

// create switch characteristic
BLECharCharacteristic    ReadOnlyArrayGattCharacteristic  = BLECharCharacteristic("a001", BLERead);
BLECharCharacteristic    WriteOnlyArrayGattCharacteristic = BLECharCharacteristic("a002", BLEWrite);

BLECharacteristic DataCharacteristic("a003", BLERead | BLENotify, 12);  //@param data - an Uint8Array.

/********************************************************************************************************/
/************************ UTILITY FUNCTIONS *************************************************/
/********************************************************************************************************/
float differenceBetweenAngles(float firstAngle, float secondAngle)
  {
        double difference = secondAngle - firstAngle;
        while (difference < -180) difference += 360;
        while (difference > 180) difference -= 360;
        return difference;
 }

/********************************************************************************************************/
/************************ BLUETOOTH BLE FUNCTIONS *************************************************/
/********************************************************************************************************/
void blePeripheralConnectHandler(BLECentral& central) {
  // central connected event handler
if(debug){
  Serial.print(F("Connected event, central: "));
  Serial.println(central.address());
}
}

void blePeripheralDisconnectHandler(BLECentral& central) {
  // central disconnected event handler
if(debug){
  Serial.print(F("Disconnected event, central: "));
  Serial.println(central.address());
}
}

void blePeripheralServicesDiscoveredHandler(BLECentral& central) {
  // central  services discovered event handler
if(debug){
  Serial.print(F(" services discovered event, central: "));
  Serial.println(central.address());
}

  delay(2000);
}

void bleCharacteristicValueUpdatedHandle(BLECentral& central, BLECharacteristic& characteristic) {
  const unsigned char* the_buffer = characteristic.value();
  unsigned char the_length = characteristic.valueLength();
 // char char_buf[2]={0,0};
  int command_value;
  
  String CardID = "";
  for (byte i = 0; i < the_length; i++){ CardID += String(the_buffer[i], HEX); }

  char *char_buf = const_cast<char*>(CardID.c_str());
  
  command_value = (int)strtol(char_buf, NULL, 16);
  
  if(debug) Serial.print("App command: "); Serial.println( command_value );

  //Process commands from app
  if(command_value < 9 && command_value != 0){
    varState = 1; //ACTIVATE COMMAND
  } else if(command_value > 10 || command_value < 20){
    varState = 0; //DEACTIVATE COMMAND
  }

  BLEUtil::printBuffer(characteristic.value(), characteristic.valueLength());
  if(debug) delay(1000);
  delay(100);
}

void switchCharacteristicWritten(BLECentral& central, BLECharacteristic& characteristic) {
  // central wrote new value to characteristic, update LED
  Serial.print(F("Characteristic event, writen: "));

  if (ReadOnlyArrayGattCharacteristic.value()) {
    if(debug) Serial.println(F("on"));
  } else {
    if(debug) Serial.println(F("off"));
  }
  delay(2000);
}

/********************************************************************************************************/
/************************ SETUP *************************************************************************/
/********************************************************************************************************/

void setup() 
{
    Serial.begin(115200);
    if(debug) Serial.print("starting\t");
    delay(50);
    
    //configure haptic feedback pin
    pinMode(VIBRATE_PIN, OUTPUT);  digitalWrite(VIBRATE_PIN, 0);

    delay(5);
    Wire.begin(LIS3DH_SDA_PIN, LIS3DH_SCL_PIN); //sda,scl
    delay(50);
  
  /************ INIT LIS3DH ACCELEROMETER *****************************/
    Serial.println("Initialize LIS3DH");
    //Call .begin() to configure the IMU
    IMU.begin();

  /************** INIT BLUETOOTH BLE instantiate BLE peripheral *********/
    // set LED pin to output mode
   // pinMode(LED_PIN, OUTPUT);
    // set advertised local name and service UUID
    blePeripheral.setLocalName("Example");
    blePeripheral.setDeviceName("Example");
    blePeripheral.setAdvertisedServiceUuid(customService.uuid());
    blePeripheral.setAppearance(0xFFFF);
  
    // add attributes (services, characteristics, descriptors) to peripheral
    blePeripheral.addAttribute(customService);
    blePeripheral.addAttribute(ReadOnlyArrayGattCharacteristic);
    blePeripheral.addAttribute(WriteOnlyArrayGattCharacteristic);
    blePeripheral.addAttribute(DataCharacteristic); //streaming data for app graph

    // assign event handlers for connected, disconnected to peripheral
    blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
    blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
  //blePeripheral.setEventHandler(BLEWritten, blePeripheralServicesDiscoveredHandler);

    // assign event handlers for characteristic
    ReadOnlyArrayGattCharacteristic.setEventHandler(BLEWritten /*BLEValueUpdated*/, bleCharacteristicValueUpdatedHandle);
    WriteOnlyArrayGattCharacteristic.setEventHandler(BLEWritten /*BLEValueUpdated*/, bleCharacteristicValueUpdatedHandle);

    // assign initial values
    char readValue[10] = {0,0,0,0,0,0,0,0,0,0};
    ReadOnlyArrayGattCharacteristic.setValue(0);
    char writeValue[10] = {0,0,0,0,0,0,0,0,0,0};
    WriteOnlyArrayGattCharacteristic.setValue(0);

    // initialize variables to pace updates to correct rate
    microsPerReading = 1000000 / 25;
    microsPrevious = micros();
  
    // begin initialization
    blePeripheral.begin();
  
    if(debug) Serial.println("BLE Mobile App Peripheral init");

    delay(500);   
}

/********************************************************************************************************/
/************************ LOOP **************************************************************************/
/********************************************************************************************************/

void loop()
{     
 /************************ LOOP SPEED CONTROL *************************/
 if(clocktime + speedMs < millis()){
    
    /*************************** Timestamp ****************************/
    clocktime = millis();
    if(debug){
        Serial.print("Time: "); Serial.print( clocktime/1000 ); Serial.println(" s"); 
    }

   /************************* Bluetooth App  **************************/
    blePeripheral.poll();

   /********************** READ LIS3DH ACCELEROMETER ***********************/
    acc[0] = IMU.readFloatAccelX();
    acc[1] = IMU.readFloatAccelY();
    acc[2] = IMU.readFloatAccelZ();

    pitch = (180/3.141592) * ( atan2( acc[0], sqrt( acc[1] * acc[1] + acc[2] * acc[2])) );
    roll = (180/3.141592) * ( atan2(-acc[1], -acc[2]) );

    if(debug){
      Serial.print("Pitch: "); 
      Serial.print(pitch); 
      Serial.print("  Roll: "); 
      Serial.println(roll);
      Serial.print("accX: "); Serial.println( acc[0] ); 
      Serial.print("accY: "); Serial.println( acc[1] ); 
      Serial.print("accZ: "); Serial.println( acc[2] );
    }

 
    /*********** Bluetooth App  *******************/
    unsigned long microsNow;
    roll = 5;
    pitch = 5;
    int roll_ble = roll;
    roll_ble = roll_ble + 180;
    
    int pitch_ble = pitch;
    pitch_ble = pitch_ble + 180;

    // check if it's time to read data and update the filter
    microsNow = micros();
    
    if(microsNow - microsPrevious >= microsPerReading){

          String strRoll = String(roll_ble);
          String strPitch = String(pitch_ble);
          String str = "9999999999999";

          if(debug){Serial.print("strPitch strRoll: "); Serial.print(strPitch); Serial.print(" "); Serial.println(strRoll);}
        
          BLECentral central = blePeripheral.central();
          
          if(central){ // if a central is connected to peripheral

              const unsigned char imuCharArray[12] = {
                  (uint8_t)roll_ble,
                  (uint8_t)pitch_ble,
                  (uint8_t)( (acc[0] + 1) * 100),
                  (uint8_t)( (acc[1] + 1) * 100),
                  (uint8_t)( (acc[2] + 1) * 100),
                  (uint8_t)(varState),   //send current device status to app
                  (uint8_t)str[6],
                  (uint8_t)str[7],
                  (uint8_t)str[8],
                  (uint8_t)str[9],
                  (uint8_t)str[10],
                  (uint8_t)str[11]
              };
 
              //send data over bluetooth
              DataCharacteristic.setValue(imuCharArray,12);
          }
  
          // increment previous time, so we keep proper pace
          microsPrevious = microsPrevious + microsPerReading;
        
     }

  } //end loop speed
} //end infinate loop
