


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


#include "gyroscopeFunc.h"
#include "displayFunc.h"
#include "networkFunc.h"

void setup() {
  Wire.begin();
  WifiConnectInit();
  Serial.begin(115200);
  MPU6050_Init();
  DisplayInit();
  irrecv.enableIRIn();
  pinMode(35, INPUT_PULLUP);
  delay(100);

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

enum RingState { Normal, Inbound, InboundOut, Outbound, OutboundRest, OutboundIn};
class FlexDataHandler {
  public:
    RingState _ringstate = RingState::Normal ;
    SmoothValueFlexInt flexValue;
    SmoothValueFlexInt flexValue32;
    long SensorLastClock = 0;
    long SensorCrtClock = 0;
    long lastMonitorTime = 0;


    int lastValIn = 0;
    int StateArrIn[6]   = {999, 999, 999, 999, 999, 999};
    long StateTimeIn[6] = {999, 999, 999, 999, 999, 999};
    int crtSeqIdxIn[6]  = {999, 999, 999, 999, 999, 999};

    int lastValOut = 0;
    int StateArrOut[6]   = {999, 999, 999, 999, 999, 999};
    long StateTimeOut[6] = {999, 999, 999, 999, 999, 999};
    int crtSeqIdxOut[6]  = {999, 999, 999, 999, 999, 999};



    boolean initReady = false;
    int stableThrehold = 15;
    boolean checkIsStable() {
      for (int i = 0; i < (sizeof(crtSeqIdxOut) / sizeof(int)) - 1; i++) {
        if (crtSeqIdxOut[i] == 999) {
          return false;
        }
      }

      for (int i = 0; i < (sizeof(crtSeqIdxOut) / sizeof(int)) - 1; i++) {
        if (abs(crtSeqIdxOut[i] - crtSeqIdxOut[i + 1]) > stableThrehold) {
          return false;
        }
      }
      return true;
    };

    void pushCrtSeqIdxOut() {
      // Serial.print("--\n");
      for (int i = (sizeof(crtSeqIdxOut) / sizeof(int)) - 1; i >= 0; i--) {
        if (i == 0) {
          crtSeqIdxOut[i] = flexValue.getValue();
        } else {
          crtSeqIdxOut[i] = crtSeqIdxOut[i - 1];
        }
      }

    };
    void pushStatueArrOut() {
      for (int i = (sizeof(StateArrOut) / sizeof(int)) - 1; i >= 0; i--) {
        if (i == 0) {
          StateArrOut[i] = flexValue.getValue();
        } else {
          StateArrOut[i] = StateArrOut[i - 1];
        }
      }
 
    };
    void pushStateTimeOut() {
      for (int i = (sizeof(StateTimeOut) / sizeof(int)) - 1; i >= 0; i--) {
        if (i == 0) {
          StateTimeOut[i] = SensorCrtClock;
        } else {
          StateTimeOut[i] = StateTimeOut[i - 1];
        }
      }

    };
    void processData(int crtReading, int crtReading32) {
      SensorCrtClock =  millis();
      flexValue32.insert(crtReading32);
      flexValue.insert(crtReading);

      pushCrtSeqIdxOut();
      if (checkInitReady() == false) {
        return;
      }

      if (SensorCrtClock - lastMonitorTime >= 50) {
        performMonitoring();
        lastMonitorTime = SensorCrtClock;
      }


    };

    void performMonitoring() {
      if (lastValOut == 0) {
        lastValOut = flexValue.getValue();
      }
      if (lastMonitorTime == 0) {
        lastMonitorTime = SensorCrtClock;
      }

      /*
        Out out
        600 500 400 400 500 500


        Out In
        400 500 500 600

      */
      int triggerVal = 30;
      if (_ringstate == RingState::Normal) {
        triggerVal = 50;
      }
      if (abs(lastValOut - flexValue.getValue()) > triggerVal) {
        if (StateArrOut[0] == 999) {
          StateArrOut[0] = lastValOut;
        }
        if (StateTimeOut[0] == 999) {
          StateTimeOut[0] = lastMonitorTime;
        }
        pushStatueArrOut();
        pushStateTimeOut();
        lastValOut = flexValue.getValue();
        //Serial.print("\ndifferent detected \n");
      }

      if (_ringstate == RingState::Normal || _ringstate == RingState::OutboundIn) {
        if (StateArrOut[0] != 999) {
          if (StateArrOut[1] - StateArrOut[0] > triggerVal) {
            _ringstate = RingState::Outbound;
            Serial.print("trigger OutBound\n");
            return;
          }
        }
      }

      if (_ringstate == RingState::Outbound) {
        if (StateArrOut[0] != 999) {
          if (StateArrOut[0] - StateArrOut[1] > triggerVal) {
            _ringstate = RingState::OutboundIn;
            Serial.print("trigger OutboundIn\n");
            return;
          }
        }
      }

      if (_ringstate == RingState::OutboundIn) {
        if (SensorCrtClock - StateTimeOut[0] > 100) {
          for (int i = (sizeof(StateArrOut) / sizeof(i.nt)) - 1; i >= 0; i--) {
            StateArrOut[i] = 999;
            StateTimeOut[i] = 999;
          }
          _ringstate = RingState::Normal;
          Serial.print("trigger return to Normal\n");
          return;
        }
      }


    }


    boolean checkInitReady() {
      if (initReady == false) {
        if (checkIsStable() == true) {
          initReady = true;
        } else {
          return false;
        }
      }
      return true;
    }

};



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
  if (action == "register") {
    String response = _data["response"];
    if (response == "success") {
      requestToStartGame();
    }
  } else if (action = "startGame") {
    String response = _data["response"];
    if (response == "success") {
      socketState = SocketState::GameStarted;
      state = InGameMode;
    }
  }
}
