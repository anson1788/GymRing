

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


class GyroscopeNetworkClass{
  public:
      long lastClockForGyroscope;
      String yawAngle;
      String pitchAngle;
      String rollAngle;
      String animDuration;
      bool dataIsSent = true;    
};

class FlexNetworkClass{
  public:
      long lastClockForFlex;
      String flexPercentage;
      String animDuration;
      bool dataIsSent = true;

};


GyroscopeNetworkClass gyroscopeNetworkData;
FlexNetworkClass flexNetworkData;

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
      //GetGyroscopeData();
      GetFlexData();
      checkSendNetworkData();
      
      if(flexNetworkData.flexPercentage=="0.70"){
        calibratingForMPU();
        state = GameState::RequestCompleteAndStartMotionExchange;
      }else{
        Serial.print(flexNetworkData.flexPercentage);
        Serial.print("\n");
      }
  } 
  if(state == GameState::RequestCompleteAndStartMotionExchange){
      GetGyroscopeData();
      GetFlexData();
      checkSendNetworkData();
  } 
  DisplayDrawContent(getDisplayMsg());
  //GetFlexData();
  //GetGyroscopeData();
  //delay(30);
  

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
    return "Calibration Complete, gaming";
  } 
  return "";
}

void checkSendNetworkData(){
   String flexData = "";
   String mpu3060Data = "";
   if( flexNetworkData.dataIsSent == false){
      flexData = "\"flexPercentage\":\""+flexNetworkData.flexPercentage+"\""; 
      flexData += ",\"flexDataTime\":\""+flexNetworkData.animDuration+"\""; 
      //client.send("{\"action\":\"InGameType\",\"type\":\"ringData\",\"sender\":\"sensor1\",\"gamehost\":\""+gameHostId+"\",\"flexPercentage\":\""+flexNetworkData.flexPercentage+"\",\"flexDataTime\":\""+flexNetworkData.animDuration+"\"}");
      flexNetworkData.dataIsSent = true;
   }

   if( gyroscopeNetworkData.dataIsSent == false){
      mpu3060Data = "\"yawData\":\""+gyroscopeNetworkData.yawAngle+"\""; 
      mpu3060Data += ",\"pitchData\":\""+gyroscopeNetworkData.pitchAngle+"\""; 
      mpu3060Data += ",\"rollData\":\""+gyroscopeNetworkData.rollAngle+"\""; 
      mpu3060Data += ",\"mpuDataTime\":\""+gyroscopeNetworkData.animDuration+"\""; 
      //client.send("{\"action\":\"InGameType\",\"type\":\"ringData\",\"sender\":\"sensor1\",\"gamehost\":\""+gameHostId+"\",\"flexPercentage\":\""+flexNetworkData.flexPercentage+"\",\"flexDataTime\":\""+flexNetworkData.animDuration+"\"}");
      gyroscopeNetworkData.dataIsSent = true;
   }

   if(mpu3060Data!="" || flexData !=""){
      String networkData = "";
      networkData += "{\"action\":\"InGameType\"";
      networkData += ",\"type\":\"ringData\"";
      networkData += ",\"sender\":\"sensor1\"";    
      networkData += ",\"gamehost\":\""+gameHostId+"\"";   
      if(flexData!=""){   
        networkData += ","+flexData;         
      }
      if(mpu3060Data!=""){   
        networkData += ","+mpu3060Data;         
      }
      networkData += "}";
      Serial.print(" data ");
      Serial.print(networkData);
      Serial.print(" \n ");
      SerialBT.println(networkData); 
      //client.send(networkData);
   }
}
