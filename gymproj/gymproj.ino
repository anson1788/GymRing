

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
#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 35;
const int LOADCELL_SCK_PIN = 33;

HX711 scale;



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
String gameHostId = "";


class NetworkClass {
  public:
    String yawAngle = "-999";
    String pitchAngle = "-999";
    String rollAngle = "-999";
    String flexPercentage = "1.0";
    bool dataIsSent = true;
};


NetworkClass mNetworkData;
NetworkClass mNetworkDataMotion;

#include "gyroscopeFunc.h"
#include "displayFunc.h"


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
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
}

String receivedMsg = "";
String DisplayMsg = "";
void loop() {  
  receivedMsg = "";
  getRingData();
  while (SerialBT.available()) {
    char incomingChar = SerialBT.read();
    receivedMsg += String(incomingChar);
  }
  if (receivedMsg != "") {
    Serial.print(" data ");
    Serial.print(receivedMsg);
    Serial.print("\n");
    handleBlueToothMsg(receivedMsg);
  }
  if (state == GameState::RequestCalibration || state == GameState::RequestCompleteAndStartMotionExchange) {
    handleSensorData();
  }
  DisplayDrawContent(getDisplayMsg());

}

long crtClockForMultipleSensor = -999;
long lastClockForMultipleSensor = -999;

long crtFlexSendTime = -999;
long ringVal = 0;
long ringVal_S = -1;
void getRingData(){
  
  if (scale.is_ready()) {
    long reading = scale.read();

    if(ringVal_S==-1){
      ringVal_S = reading;
    }else{  
      
      ringVal = reading - ringVal_S;  
      ringVal_S = reading;

      Serial.print("HX711 reading: ");
      Serial.println(ringVal);
      if (ringVal < -130000){
        Serial.print("HX711 reading: ");
        Serial.println(ringVal);    
      }
    }
  } else {
 
  }
}
void handleSensorData() {
  crtClockForMultipleSensor = millis();
  if (lastClockForMultipleSensor == -999) {
    lastClockForMultipleSensor = crtClockForMultipleSensor;
  }

  if (crtFlexSendTime == -999) {
    crtFlexSendTime = crtClockForMultipleSensor;
  }

  if (crtClockForMultipleSensor - lastClockForMultipleSensor > 30) {
    if (state == GameState::RequestCalibration) {
      if (ringVal < -130000 && crtClockForMultipleSensor - crtFlexSendTime > 250) {
        SerialBT.println("{\"action\":\"StartCalibrating\"}");
        crtFlexSendTime = crtClockForMultipleSensor;
        calibratingForMPU();
        SerialBT.println("{\"action\":\"CompleteCalibrating\"}");
        state = GameState::RequestCompleteAndStartMotionExchange;
      }

      Serial.print("ringVal :");
      Serial.print(ringVal);
      Serial.print("\n");
    } else if (state == GameState::RequestCompleteAndStartMotionExchange) {

      mNetworkData.dataIsSent = true;
      if (ringVal < -130000 && crtClockForMultipleSensor - crtFlexSendTime > 250) {
        Serial.print("Time :");
        Serial.print(crtClockForMultipleSensor - crtFlexSendTime);
        Serial.print("\n");
        crtFlexSendTime = crtClockForMultipleSensor;
        mNetworkData.flexPercentage = 0.70;
        mNetworkData.dataIsSent = false;

      }
      if (!mNetworkData.dataIsSent) {
        checkSendNetworkData();
      } else {
        GetGyroscopeData();
        mNetworkData.yawAngle = valZ;
        mNetworkData.pitchAngle = valZ;
        mNetworkData.rollAngle = valZ;
        sendPositionData();
      }

    }
    lastClockForMultipleSensor = crtClockForMultipleSensor;
  }
}

void handleBlueToothMsg(String receivedMsg) {
  if (receivedMsg == "welcomeMsg") {
    state = GameState::BLEConnected;
    SerialBT.println("{\"action\":\"DisplayCalibration\"}");
  }
  if (receivedMsg == "RequestCalibration") {
    state = GameState::RequestCalibration;
  }
}

String getDisplayMsg() {
  if (state == GameState::WaitForBLEConnect) {
    return "waiting for BoothDeviceToConnect";
  }
  if (state == GameState::BLEConnected) {
    return "Bluetooth connect, please hold vertically";
  }

  if (state == GameState::RequestCalibration) {
    return "Calibrating";
  }
  if (state == GameState::RequestCompleteAndStartMotionExchange) {
    return "Gaming";
  }
  return "";
}

void sendPositionData() {

  String networkDataMsg = "";
  String flexData = "";
  String mpu3060Data = "";

  //  networkDataMsg = "\"flexPercentage\":\""+mNetworkData.flexPercentage+"\"";
  networkDataMsg = "\"yawData\":\"" + mNetworkData.yawAngle + "\"";
  networkDataMsg += ",\"pitchData\":\"" + mNetworkData.pitchAngle + "\"";
  networkDataMsg += ",\"rollData\":\"" + mNetworkData.rollAngle + "\"";



  if (networkDataMsg != "") {
    String networkData = "";
    networkData += "{\"action\":\"InGameType\"";
    networkData += ",\"type\":\"ringData\"";
    networkData += ",\"sender\":\"sensor1\"";
    networkData += ",\"gamehost\":\"" + gameHostId + "\"";
    networkData += "," + networkDataMsg;
    networkData += "}";

    // Serial.print(" data ");
    // Serial.print(networkData);
    // Serial.print(" \n ");
    SerialBT.println(networkData);

  }

}

void checkSendNetworkData() {
  String networkDataMsg = "";
  String flexData = "";
  String mpu3060Data = "";
  if ( mNetworkData.dataIsSent == false) {
    networkDataMsg = "\"flexPercentage\":\"" + mNetworkData.flexPercentage + "\"";
    //networkDataMsg += ",\"yawData\":\""+mNetworkData.yawAngle+"\"";
    //networkDataMsg += ",\"pitchData\":\""+mNetworkData.pitchAngle+"\"";
    //networkDataMsg += ",\"rollData\":\""+mNetworkData.rollAngle+"\"";
    mNetworkData.dataIsSent = true;
  }

  if (networkDataMsg != "") {
    String networkData = "";
    networkData += "{\"action\":\"InGameType\"";
    networkData += ",\"type\":\"ringData\"";
    networkData += ",\"sender\":\"sensor1\"";
    networkData += ",\"gamehost\":\"" + gameHostId + "\"";
    networkData += "," + networkDataMsg;
    networkData += "}";

    //Serial.print(" data ");
    // Serial.print(networkData);
    //Serial.print(" \n ");
    SerialBT.println(networkData);


  }

}
