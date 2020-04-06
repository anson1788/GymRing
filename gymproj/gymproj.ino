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
String lastVal = "";

int timerCounter = 0;

void setup() {
  Wire.begin();
  Serial.begin(115200);

  MPU6050_Init();
  WifiConnectInit();
  DisplayInit();
  irrecv.enableIRIn();
  pinMode(35,INPUT_PULLUP);
  delay(100);

}


void loop() {
  if(state==WaitForIRInput){
    HandleIRData();
    DisplayDrawContentForIRInput();
  }else if(state==StartSocketConnection){
    DisplayDrawContent("Trying to Connect to server");
    ConnectWebSocket();
  }else if(state==WaitForSocketConnection){
    DisplayDrawContent("Waiting for server to connect");
    if(socketState == RegisterCompleted){
       state = InGameMode;
    }
  }else if(state==InGameMode){
    DisplayDrawContent("Gaming");
    GetGyroscopeData();
  }
  int potValue = analogRead(35);
  //Serial.println(potValue);
  if(socketState!=WaitForFirstConnect){
     client.poll();
  }
  if(state!=WaitForIRInput){
    delay(100);
  }
}

void HandleIRData(){
  lastVal = crtInput;
  crtInput = "";
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    int rv = 0;
    switch (results.value)
    {
      case 0xFF18E7: rv = -10 ;   Serial.println(" UP ARROW"); break;
      
      case 0x8C22657B: rv = -11 ;  crtInput = "-"; Serial.println(" LEFT ARROW");    break;
      case 0xFF10EF: rv = -11 ;  crtInput = "-"; Serial.println(" LEFT ARROW");    break;

      case 0x488F3CBB: rv = -12; crtInput = "enter";  Serial.println(" -OK-");    break;
      case 0xFF38C7: rv = -12 ;  crtInput = "enter";  Serial.println(" -OK-");    break;
      
      case 0xFF5AA5: rv = -13 ;   Serial.println(" RIGHT ARROW");   break;
      case 0xFF4AB5: rv = -14 ;   Serial.println(" DOWN ARROW"); break;
      
      case 0xE318261B: rv = 1   ; crtInput = "1";   Serial.println(" 1");    break; 
      case 0xFFA25D: rv = 1   ; crtInput = "1";   Serial.println(" 1");    break;
      
      case 0xFF629D: rv = 2   ; crtInput = "2";  Serial.println(" 2");    break;
      case 0x511DBB: rv = 2   ; crtInput = "2";  Serial.println(" 2");    break;
      
      case 0xFFE21D: rv = 3   ; crtInput = "3";  Serial.println(" 3");    break;
      case 0xEE886D7F: rv = 3   ; crtInput = "3";  Serial.println(" 3");    break;      

      case 0x52A3D41F: rv = 4   ; crtInput = "4";  Serial.println(" 4");    break;
      case 0xFF22DD: rv = 4   ; crtInput = "4";  Serial.println(" 4");    break;

      case 0xD7E84B1B: rv = 5   ; crtInput = "5";  Serial.println(" 5");    break;
      case 0xFF02FD: rv = 5   ; crtInput = "5";  Serial.println(" 5");    break;

      case 0x20FE4DBB: rv = 6   ; crtInput = "6";  Serial.println(" 6");    break;
      case 0xFFC23D: rv = 6   ; crtInput = "6";  Serial.println(" 6");    break;

      case 0xF076C13B: rv = 7   ; crtInput = "7";  Serial.println(" 7");    break;
      case 0xFFE01F: rv = 7   ; crtInput = "7";  Serial.println(" 7");    break;
      
      case 0xA3C8EDDB: rv = 8   ; crtInput = "8";  Serial.println(" 8");    break;
      case 0xFFA857: rv = 8   ; crtInput = "8";  Serial.println(" 8");    break;

      case 0xE5CFBD7F: rv = 9   ; crtInput = "9";  Serial.println(" 9");    break;
      case 0xFF906F: rv = 9   ; crtInput = "9";  Serial.println(" 9");    break;
      
      case 0xFF9867: rv = 0   ; crtInput = "0";  Serial.println(" 0");    break;
      case 0x97483BFB: rv = 0   ; crtInput = "0";  Serial.println(" 0");    break;
            
      case 0xFF6897: rv = -15 ;   Serial.println(" *");    break;
      case 0xFFB04F: rv = -16 ;   Serial.println(" #");    break;
        // case 0xFFFFFFFF: rv = -20 ; break; // Serial.println(" REPEAT");    break;
        // default: break ;
    }

    Serial.println(" last val = " + lastVal);
    Serial.println(" crt val = " + crtInput);  
    if(lastVal!="" && lastVal == crtInput){
        crtInput = "";
    }
 
    irrecv.resume(); // Receive the next value
  }
}

void WifiConnectInit(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi..");
  }
   
  Serial.println(WiFi.localIP());
  delay(2000);
}



void ConnectWebSocket(){
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
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

void connectToRoom(){
      socketState = WaitForControllerMsg;
      String lastCharacter = inputString.substring(inputString.length() - 1, inputString.length());
      String roomid = inputString.substring(0, inputString.length()-1);
      String _sensorType = "";
      if(lastCharacter=="1"){
        _sensorType = "sensor1";
      }

      if(lastCharacter=="2"){
        _sensorType = "sensor2";
      }

      if(lastCharacter=="3"){
        _sensorType = "sensor3";
      }

      if(lastCharacter=="4"){
        _sensorType = "sensor4";
      }

        client.send("{\"action\":\"registerType\",\"type\":\""+_sensorType+"\",\"roomid\":\""+roomid+"\"}");
      //client.send("{'action':'registerType','type':'"+_sensorType+"','roomid':'"+roomid+"'}");
}

void DisplayInit(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
}



void DisplayDrawContentForIRInput(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  
  if(crtInput!=""){
      if(crtInput=="enter"){
        state = StartSocketConnection;
        return;
      }
      
      if(crtInput!="-"){
         inputString = inputString + crtInput;
         Serial.println(" input val = " + inputString);  
      }else{
         inputString = inputString.substring(0, inputString.length() - 1);
      }
      display.print(inputString);
      display.display();
      delay(500);
  }else if(inputString!=""){
    display.print(inputString);
    display.display();
  }else{
     display.print("Please input roomid");  
     display.display();
  }

}


void DisplayDrawContent(String text){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print(text);
  display.display();
}

void GetGyroscopeData(){
  double Ax, Ay, Az, T, Gx, Gy, Gz;
  
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
  
  //divide each with their sensitivity scale factor
  Ax = (double)AccelX/AccelScaleFactor;
  Ay = (double)AccelY/AccelScaleFactor;
  Az = (double)AccelZ/AccelScaleFactor;
  T = (double)Temperature/340+36.53; //temperature formula
  Gx = (double)GyroX/GyroScaleFactor;
  Gy = (double)GyroY/GyroScaleFactor;
  Gz = (double)GyroZ/GyroScaleFactor;

  S_Ax.insert(Ax);
  S_Ay.insert(Ax);
  S_Az.insert(Ax);

  S_Gx.insert(Gx);
  S_Gy.insert(Gy);
  S_Gz.insert(Gz);

  S_T.insert(T);       

  /*
  Serial.print("Ax: "); Serial.print(S_Ax.getValue());
  Serial.print(" Ay: "); Serial.print(S_Ay.getValue());
  Serial.print(" Az: "); Serial.print(S_Az.getValue());
  Serial.print(" T: "); Serial.print(S_T.getValue());
  Serial.print(" Gx: "); Serial.print(S_Gx.getValue());
  Serial.print(" Gy: "); Serial.print(S_Gy.getValue());
  Serial.print(" Gz: "); Serial.println(S_Gz.getValue());
  */
}

void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
}

// read all 14 register
void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t)14);
  AccelX = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelY = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelZ = (((int16_t)Wire.read()<<8) | Wire.read());
  Temperature = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroX = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroY = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroZ = (((int16_t)Wire.read()<<8) | Wire.read());
}

//configure MPU6050
void MPU6050_Init(){
  delay(150);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SMPLRT_DIV, 0x07);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_1, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_2, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_GYRO_CONFIG, 0x00);//set +/-250 degree/second full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_CONFIG, 0x00);// set +/- 2g full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_FIFO_EN, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_INT_ENABLE, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SIGNAL_PATH_RESET, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_USER_CTRL, 0x00);
}
