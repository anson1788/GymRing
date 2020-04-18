


/*
    This sketch demonstrates how to scan WiFi networks.
    The API is almost the same as with the WiFi Shield library,
    the most obvious difference being the different file you need to include:
*/
#define INTERRUPT_ATTR IRAM_ATTR
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
SocketState socketState = WaitForWifi;

GameState state = WaitForIRInput;
String inputString = "";
String crtInput = "";
String role = "";
String roomId = "";
int timerCounter = 0;
String gameHostId ="";


#include "gyroscopeFunc.h"
#include "displayFunc.h"
#include "networkFunc.h"
#include "flexSensor.h"
void setup() {
  Wire.begin();
  WifiConnectInit();
  Serial.begin(115200);
  MPU6050_Init();
  DisplayInit();
  irrecv.enableIRIn();
  pinMode(35, INPUT_PULLUP);
  delay(100);
    Serial.println("ss");
}


void loop() {
  if (state == BLESettingMode) {
    DisplayDrawContent("BLE Setting Mode");
  } else if (state == WaitForIRInput) {
    HandleIRData();
    DisplayDrawContentForIRInput();
  } else if (state == StartSocketConnection) {
    DisplayDrawContent("Trying to Connect to server");
    if (socketState == SocketState::WaitForWifi) {
      socketState = SocketState::WaitForFirstConnect;
      ConnectWebSocket();
    }
  } else if (state == WaitForSocketConnection) {
    DisplayDrawContent("Waiting for server to connect");
    if (socketState == RegisterCompleted) {
      state = WaitingForGameToStart;
    }
  } else if (state == WaitingForGameToStart) {
    if (socketState == RegisterCompleted) {
      HandleIRData();
      checkIsGameStartInput();
      DisplayDrawContent("Waiting For Game To Start");
    } else if (socketState == SocketState::RequestingGameToStart) {
      DisplayDrawContent("Requesting Game Start");
    } else if (socketState == SocketState::GameStarted) {
      state = InGameMode;
    }
  } else if (state == InGameMode) {
    DisplayDrawContent("Gaming");
    GetGyroscopeData();
  }
  GetGyroscopeData();
  handleSensorData();
  if (socketState != WaitForFirstConnect && socketState != WaitForWifi) {
    client.poll();
  }
  if (state != WaitForIRInput) {
    // delay(100);
  }
}




FlexDataHandler flexHandler;
void handleSensorData() {
  int potValue = analogRead(35);
  int potValue32 = analogRead(32);
  flexHandler.processData(potValue, potValue32);

  /*
    Serial.print("step 10 monitor \n");
    Serial.print(potValue32);
    Serial.print("---\n");
  */
  /*
    if(abs(lastVal-flexValue.getValue())>50){
    Serial.print("------------------------------------------------------ \n");
    Serial.print(flexValue.getValue());
    Serial.print("-------------------------------------------------------- \n");
    lastVal = flexValue.getValue();
    }
  */
}


void ConnectWebSocket() {

  client.setInsecure();
  client.onMessage(onMessageCallback);
  client.onEvent(onEventsCallback);
  client.connect(websockets_server);

}

void onMessageCallback(WebsocketsMessage message) {
  Serial.print("Got Message: ");
  Serial.println(message.data());
  StaticJsonDocument<200> doc;
  String _data = message.data();
  int firstBacketIdx = _data.indexOf('{');
  _data = _data.substring(firstBacketIdx);
  Serial.print("data " + _data);
  DeserializationError error = deserializeJson(doc, _data);
  if (error) {

    Serial.println(error.c_str());
    return;
  }
  String action = doc["action"];
  HandleMsgEvent(doc);
}

void onEventsCallback(WebsocketsEvent event, String data) {
  Serial.println("111MSG");
  if (event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("Connnection Opened");
    state = WaitForSocketConnection;
    connectToRoom();
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    Serial.println("Connnection Closed");
  } else if (event == WebsocketsEvent::GotPing) {
    Serial.println("Got a Ping!");
    Serial.println("get a ping action : ");
  } else if (event == WebsocketsEvent::GotPong) {
    Serial.println("Got a Pong!");
  }
}

void HandleMsgEvent(StaticJsonDocument<200> _data) {
  String action = _data["action"];
  Serial.print("print i am here");
  Serial.print(action);
  if (action == "register") {
    String response = _data["response"];
      Serial.print("print i am here22");
      Serial.print(response);
    if (response == "success") {
      requestToStartGame();
    }
  } else if (action = "startGame") {
    String response = _data["response"];
    String gamehost = _data["gamehost"];
    if (response == "success") {
      socketState = SocketState::GameStarted;
      state = InGameMode;
      gameHostId = gamehost;
    }
  }
}
