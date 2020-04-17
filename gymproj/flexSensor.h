#ifndef flexSensor_h
#define flexSensor_h

enum RingState { Normal, Inbound, InboundOut, Outbound, OutboundIn};
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
    

    int lastValOut = 0;
    int StateArrOut[6]   = {999, 999, 999, 999, 999, 999};
    long StateTimeOut[6] = {999, 999, 999, 999, 999, 999};

    int crtSeqIdxOut[6]   = {999, 999, 999, 999, 999, 999};


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



    void pushCrtArrOut() {
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

 

    void pushStatueArrIn() {
      for (int i = (sizeof(StateArrIn) / sizeof(int)) - 1; i >= 0; i--) {
        if (i == 0) {
          StateArrIn[i] = flexValue32.getValue();
        } else {
          StateArrIn[i] = StateArrIn[i - 1];
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
    void pushStateTimeIn() {
      for (int i = (sizeof(StateTimeIn) / sizeof(int)) - 1; i >= 0; i--) {
        if (i == 0) {
          StateTimeIn[i] = SensorCrtClock;
        } else {
          StateTimeIn[i] = StateTimeIn[i - 1];
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

    
    void processData(int crtReading, int crtReading32) {
      SensorCrtClock =  millis();
      flexValue32.insert(crtReading32);
      flexValue.insert(crtReading);
      pushCrtArrOut();
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

      if (lastValIn == 0) {
        lastValIn = flexValue32.getValue();
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
      int triggerValOut = 30;
      if (_ringstate == RingState::Normal) {
        triggerValOut = 50;
      }

      //Serial.print(flexValue32.getValue());
      if (abs(lastValOut - flexValue.getValue()) > triggerValOut) {
        if (StateArrOut[0] == 999) {
          StateArrOut[0] = lastValOut;
        }
        if (StateTimeOut[0] == 999) {
          StateTimeOut[0] = lastMonitorTime;
        }
        pushStatueArrOut();
        pushStateTimeOut();
        lastValOut = flexValue.getValue();
      }

      //----------------------------------------------------
      int triggerValIn = 30;
      if (_ringstate == RingState::Normal) {
        triggerValIn = 50;
      }
       if (abs(lastValIn - flexValue32.getValue()) > triggerValIn) {
        if (StateArrIn[0] == 999) {
          StateArrIn[0] = lastValIn;
        }
        if (StateTimeIn[0] == 999) {
          StateTimeIn[0] = lastMonitorTime;
        }
        pushStatueArrIn();
        pushStateTimeIn();
        lastValIn = flexValue32.getValue();
      }

      if (_ringstate == RingState::Normal 
          ) {
        if (StateArrOut[0] != 999 && StateArrOut[1] != 999) {
          if (StateArrOut[1] - StateArrOut[0] > triggerValOut) {
            _ringstate = RingState::Outbound;
            Serial.print("trigger OutBound\n");
            return;
          }
        }
        if (StateArrIn[0] != 999 && StateArrOut[1] != 999) {
          if (StateArrIn[1]-StateArrIn[0] > triggerValIn) {
            _ringstate = RingState::Inbound;
            Serial.print("trigger Inbound\n");
            return;
          }
        }
      }

      if (_ringstate == RingState::Outbound) {
        if (StateArrOut[0] != 999 && StateArrOut[1] != 999) {
          if (StateArrOut[0] - StateArrOut[1] > triggerValOut) {
            _ringstate = RingState::OutboundIn;
            Serial.print("trigger OutboundIn\n");
            return;
          }
        }
      }

      if (_ringstate == RingState::OutboundIn) {
        if (SensorCrtClock - StateTimeOut[0] > 100) {
          for (int i = (sizeof(StateArrOut) / sizeof(int)) - 1; i >= 0; i--) {
            StateArrOut[i] = 999;
            StateTimeOut[i] = 999;
          }
          for (int i = (sizeof(StateArrIn) / sizeof(int)) - 1; i >= 0; i--) {
            StateArrIn[i] = 999;
            StateTimeIn[i] = 999;
          }
          _ringstate = RingState::Normal;
          Serial.print("trigger return to Normal\n");
          return;
        }
      }

      if (_ringstate == RingState::Inbound) {
        if (StateArrIn[0] != 999 && StateArrOut[1] != 999) {
          if (StateArrIn[0] - StateArrIn[1] > triggerValIn) {
            _ringstate = RingState::InboundOut;
            Serial.print("trigger InboundOut\n");
            return;
          }
        }
      }

      if (_ringstate == RingState::InboundOut) {
        if (SensorCrtClock - StateTimeIn[0] > 100) {
          for (int i = (sizeof(StateArrIn) / sizeof(int)) - 1; i >= 0; i--) {
            StateArrIn[i] = 999;
            StateTimeIn[i] = 999;
          }

          for (int i = (sizeof(StateArrOut) / sizeof(int)) - 1; i >= 0; i--) {
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

#endif
