#ifndef flexSensor_h
#define flexSensor_h

enum RingState { Normal, Inbound, InboundOut, Outbound, OutboundIn};
class FlexDataHandler {
  public:
    RingState _ringstate = RingState::Normal;
    SmoothValueFlexInt flexValue32;
    long SensorLastClock = 0;
    long SensorCrtClock = 0;
    long lastMonitorTime = 0;


    int lastVal = 0;
    int StateArr[6]   = {999, 999, 999, 999, 999, 999};
    long StateTime[6] = {999, 999, 999, 999, 999, 999};
    
    int crtSeqIdx[6]   = {999, 999, 999, 999, 999, 999};


    boolean initReady = false;
    int stableThrehold = 15;
    boolean checkIsStable() {
      for (int i = 0; i < (sizeof(crtSeqIdx) / sizeof(int)) - 1; i++) {
        if (crtSeqIdx[i] == 999) {
          return false;
        }
      }

      for (int i = 0; i < (sizeof(crtSeqIdx) / sizeof(int)) - 1; i++) {
        if (abs(crtSeqIdx[i] - crtSeqIdx[i + 1]) > stableThrehold) {
          return false;
        }
      }
      return true;
    };


    void pushStatueArr() {
      for (int i = (sizeof(StateArr) / sizeof(int)) - 1; i >= 0; i--) {
        if (i == 0) {
          StateArr[i] = flexValue32.getValue();
        } else {
          StateArr[i] = StateArr[i - 1];
        }
      }
      /*
       Serial.print("\n----\n");
        for(int i=0;i<(sizeof(StateArrIn) / sizeof(int)) - 1; i++){
          Serial.print(StateArrIn[i]);
          Serial.print(",");
        }
       Serial.print("\n----\n");
       */
    };

    
    void pushStateTime() {
      for (int i = (sizeof(StateTime) / sizeof(int)) - 1; i >= 0; i--) {
        if (i == 0) {
          StateTime[i] = SensorCrtClock;
        } else {
          StateTime[i] = StateTime[i - 1];
        }
      }
      /*
       Serial.print("\n----\n");
        for(int i=0;i<(sizeof(StateTimeIn) / sizeof(int)) - 1; i++){
          Serial.print(StateTimeIn[i]);
          Serial.print(",");
        }
       Serial.print("\n----\n");
       */
    };

    
    void pushCrtArr() {
      for (int i = (sizeof(crtSeqIdx) / sizeof(int)) - 1; i >= 0; i--) {
        if (i == 0) {
          crtSeqIdx[i] = flexValue32.getValue();
        } else {
          crtSeqIdx[i] = crtSeqIdx[i - 1];
        }
      }
 
    };
    void processData(int crtReading, int crtReading32) {
      SensorCrtClock =  millis();
      flexValue32.insert(crtReading32);


      pushCrtArr();
      if (checkInitReady() == false) {
        return;
      }

      if (SensorCrtClock - lastMonitorTime >= 10) {
        performMonitoring();
        lastMonitorTime = SensorCrtClock;
      }


    };

    void performMonitoring() {
    
      if (lastVal == 0) {
        lastVal = flexValue32.getValue();
      }
      
      if (lastMonitorTime == 0) {
        lastMonitorTime = SensorCrtClock;
      }

      /*
        (          )   ---   1200         (1200<x)    (x>1200)
          (      )     ---   1200 - 800   (800<x<1200)(x>800 , x<1200) 
            (  )       ---   800          (x<800)     (x<800)
 
      */
    
     
       if (abs(lastVal - flexValue32.getValue()) > 30) {
        if (StateArr[0] == 999) {
          StateArr[0] = lastVal;
        }
        if (StateTime[0] == 999) {
          StateTime[0] = lastMonitorTime;
        }
        pushStatueArr();
        pushStateTime();
        lastVal = flexValue32.getValue();
        /*
        Serial.print("\n---\n");  
        for (int i = 0; i < (sizeof(StateArr) / sizeof(int)) - 1; i++) {
          Serial.print( StateArr[i]);
          Serial.print(", "); 
        }
        Serial.print("\n---\n");  
        */
      }
     
      StateArr[0] = flexValue32.getValue();
      StateArr[1] = flexValue32.getValue();

    
      if(_ringstate == RingState::Normal){
          if(StateArr[0]!=999 && StateArr[1]!=999 ){
                if(StateArr[0]>1000){
                    //trigger outbound
                    _ringstate = RingState::Outbound;
                    sendStatusForGame("Outbound");
                    Serial.print("\n  ---  Outbound --- \n");
                }else if(StateArr[0]<700){  
                    _ringstate = RingState::Inbound;
                    Serial.print("\n  ---  Inbound --- \n");
                    sendStatusForGame("Inbound");
                }else{
                  
                }
          }  
      }else if(_ringstate ==  RingState::Outbound){
          if(StateArr[0]!=999 && StateArr[1]!=999 ){
                if(StateArr[0]<1000 && StateArr[0]>700){
                    //trigger outbound
                    _ringstate = RingState::Normal;
                    sendStatusForGame("outBoundIn");
                    Serial.print("\n  ---  Normal --- \n");
                }
          }  
      }else if(_ringstate ==  RingState::Inbound){
          if(StateArr[0]!=999 && StateArr[1]!=999 ){
                if(StateArr[0]<1000 && StateArr[0]>700){
                    //trigger outbound
                    _ringstate = RingState::Normal;
                    sendStatusForGame("InboundOut");
                    Serial.print("\n  ---  Normal --- \n");
                }
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
  void sendStatusForGame(String _status){
      client.send("{\"action\":\"InGameType\",\"type\":\"ringData\",\"sender\":\"sensor1\",\"gamehost\":\""+gameHostId+"\",\"ringData\":\""+_status+"\"}");
  }
};



#endif
