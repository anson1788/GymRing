#ifndef networkFunc_h
#define networkFunc_h

void WifiConnectInit(){
  Serial.println("get wifi");
  char ssid[] = "HOSK2018";
  char password[] = "94858500";
  WiFi.begin(ssid, password);
  Serial.println("get wifi22");
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi..");
  }
   
  Serial.println(WiFi.localIP());
  delay(500);
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
      role = _sensorType;
     // client.send("{\"action\":\"registerType\",\"type\":\"gamehost\"}");
      roomId = roomid;
      client.send("{\"action\":\"registerType\",\"type\":\""+_sensorType+"\",\"roomid\":\""+roomid+"\"}");
}

void requestToStartGame(){
      socketState = SocketState::RequestingGameToStart;
      client.send("{\"action\":\"InGameType\",\"type\":\"startGame\",\"role\":\""+role+"\",\"roomid\":\""+roomId+"\",\"\":\"\"}");
}
#endif
