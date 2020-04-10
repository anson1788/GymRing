/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is almost the same as with the WiFi Shield library,
 *  the most obvious difference being the different file you need to include:
 */
#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "helperClass.h"
#include <IRremote.h>
#include <ArduinoJson.h>
int RECV_PIN = 34;

IRrecv irrecv(RECV_PIN);

decode_results results;
 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int16_t AccelX, AccelY, AccelZ, Temperature, GyroX, GyroY, GyroZ;
SmoothValueDouble S_Ax, S_Ay, S_Az, S_T, S_Gx, S_Gy, S_Gz; 

using namespace websockets;
WebsocketsClient client;
SocketState socketState = WaitForFirstConnect;

GameState state = WaitForIRInput;
String inputString = "";
String crtInput = "";
String role="";
String roomId="";
int timerCounter = 0;

#include "gyroscopeFunc.h"
#include "displayFunc.h"
#include "networkFunc.h"

void setup() {
  Wire.begin();
  Serial.begin(115200);

  MPU6050_Init();
  DisplayInit();
  irrecv.enableIRIn();
  pinMode(35,INPUT_PULLUP);
  delay(100);

}


void loop() {
  if(state==BLESettingMode){
    DisplayDrawContent("BLE Setting Mode");
  }else if(state==WaitForIRInput){
    HandleIRData();
    DisplayDrawContentForIRInput();
  }else if(state==StartSocketConnection){
    DisplayDrawContent("Trying to Connect to server");
    ConnectWebSocket();
  }else if(state==WaitForSocketConnection){
    DisplayDrawContent("Waiting for server to connect");
    if(socketState == RegisterCompleted){
       state = WaitingForGameToStart;
    }
  }else if(state==WaitingForGameToStart){
    if(socketState == RegisterCompleted){
      HandleIRData();
      checkIsGameStartInput();
      DisplayDrawContent("Waiting For Game To Start");
    }else if(socketState == SocketState::RequestingGameToStart){
      DisplayDrawContent("Requesting Game Start");
    }else if(socketState == SocketState::GameStarted){
       state = InGameMode;
    }
  }else if(state==InGameMode){
    DisplayDrawContent("Gaming");
    GetGyroscopeData();
    int potValue = analogRead(35);
    Serial.println(potValue);
  }

  if(socketState!=WaitForFirstConnect){
     client.poll();
  }
  if(state!=WaitForIRInput){
    delay(100);
  }
}




void ConnectWebSocket(){
      WifiConnectInit();
      Serial.print("Start connect to wifi");
      client.setInsecure();
      client.onMessage(onMessageCallback);
      client.onEvent(onEventsCallback);
      // Connect to server
      client.connect(websockets_server);
}

void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());
}

void onEventsCallback(WebsocketsEvent event, String data) {
    Serial.println("111MSG");
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
        state = WaitForSocketConnection;
        connectToRoom();
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connnection Closed");
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, data);
        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.c_str());
          return;
        }
        String action = doc["action"];
        Serial.println("get a ping action : "+action);
        HandleMsgEvent(doc);
        
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

void HandleMsgEvent(StaticJsonDocument<200> _data){
   String action = _data["action"];
   if(action=="register"){
     String response = _data["response"];
     if(response=="success"){    
        requestToStartGame();
     }
   }else if(action="startGame"){
      String response = _data["response"];
     if(response=="success"){    
        socketState = SocketState::GameStarted;
        state = InGameMode;
     }
  }
}
