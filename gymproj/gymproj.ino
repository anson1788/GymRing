

/*
    This sketch demonstrates how to scan WiFi networks.
    The API is almost the same as with the WiFi Shield library,
    the most obvious difference being the different file you need to include:
*/
#define INTERRUPT_ATTR IRAM_ATTR 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <ArduinoJson.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

enum GameState { 
    WaitForBLEConnect, 
    BLEConnected, 
    RequestCalibration, 
    RequestCompleteAndStartMotionExchange
};
GameState state = GameState::WaitForBLEConnect;

String inputString = "";
String crtInput = "";
String role = "";
String roomId = "";
int timerCounter = 0;
String gameHostId ="";


class NetworkClass{
  public:
      String yawAngle="-999";
      String pitchAngle="-999";
      String rollAngle="-999";
      String flexPercentage="1.0";
      bool dataIsSent = true;    
};


NetworkClass mNetworkData;


#include "gyroscopeFunc.h"
#include "displayFunc.h"
#include "flexFunc.h"

#include "BluetoothSerial.h"
BluetoothSerial SerialBT;
boolean IsBLEConnected = false;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif


void setup() {
  
  Wire.begin();
  DisplayInit();
  delay(100);
  Serial.begin(115200);
  
  MPU6050_Init();
  
  SerialBT.begin("BLETEST"); 
  
}

String receivedMsg = "";
String DisplayMsg = "";
void loop() {
  receivedMsg = "";
  while(SerialBT.available()){
    char incomingChar = SerialBT.read();
    receivedMsg += String(incomingChar);
  }
  if(receivedMsg !=""){
    Serial.print(" data ");
    Serial.print(receivedMsg);
    Serial.print("\n");
    handleBlueToothMsg(receivedMsg);
  }
  if(state == GameState::RequestCalibration){
      handleSensorData();
      if(mNetworkData.flexPercentage=="0.70"){
        SerialBT.println("{\"action\":\"StartCalibrating\"}"); 
        calibratingForMPU();
        SerialBT.println("{\"action\":\"CompleteCalibrating\"}"); 
        state = GameState::RequestCompleteAndStartMotionExchange;
      }else{
        checkSendNetworkData();
        Serial.print(mNetworkData.flexPercentage);
        Serial.print("\n");
      }
  } 
  if(state == GameState::RequestCompleteAndStartMotionExchange){
      handleSensorData();
      checkSendNetworkData();
  } 
  DisplayDrawContent(getDisplayMsg());

}

long crtClockForMultipleSensor = -999; 
long lastClockForMultipleSensor = -999; 

void handleSensorData(){
    updateFlexValue();
    crtClockForMultipleSensor = millis();   
    if(lastClockForMultipleSensor == -999){
      lastClockForMultipleSensor = crtClockForMultipleSensor;
    }
    
    if(crtClockForMultipleSensor-lastClockForMultipleSensor > 60){
      GetFlexData();
      if(state == GameState::RequestCompleteAndStartMotionExchange){
        GetGyroscopeData();
      }
      lastClockForMultipleSensor = crtClockForMultipleSensor;
    }
}

void handleBlueToothMsg(String receivedMsg){
    if(receivedMsg=="welcomeMsg"){
         state = GameState::BLEConnected;
         SerialBT.println("{\"action\":\"DisplayCalibration\"}"); 
    }
    if(receivedMsg=="RequestCalibration"){
         state = GameState::RequestCalibration;
    }
}

String getDisplayMsg(){
  if(state == GameState::WaitForBLEConnect){
    return "waiting for BoothDeviceToConnect";
  } 
  if(state == GameState::BLEConnected){
    return "Bluetooth connect, please hold vertically";
  } 

  if(state == GameState::RequestCalibration){
    return "Calibrating";
  } 
  if(state == GameState::RequestCompleteAndStartMotionExchange){
    return "Gaming";
  } 
  return "";
}

void checkSendNetworkData(){
   String networkDataMsg = "";
   String flexData = "";
   String mpu3060Data = "";
   if( mNetworkData.dataIsSent == false){
      networkDataMsg = "\"flexPercentage\":\""+mNetworkData.flexPercentage+"\""; 
      networkDataMsg += ",\"yawData\":\""+mNetworkData.yawAngle+"\""; 
      networkDataMsg += ",\"pitchData\":\""+mNetworkData.pitchAngle+"\""; 
      networkDataMsg += ",\"rollData\":\""+mNetworkData.rollAngle+"\""; 
      mNetworkData.dataIsSent = true;
   }

   if(networkDataMsg !=""){
      String networkData = "";
      networkData += "{\"action\":\"InGameType\"";
      networkData += ",\"type\":\"ringData\"";
      networkData += ",\"sender\":\"sensor1\"";    
      networkData += ",\"gamehost\":\""+gameHostId+"\"";   
      networkData += ","+networkDataMsg;         
      networkData += "}";
      Serial.print(" data ");
      Serial.print(networkData);
      Serial.print(" \n ");
      SerialBT.println(networkData); 
      //client.send(networkData);
   }
   
}
