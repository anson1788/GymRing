#ifndef gyroscopeFunc_h
#define gyroscopeFunc_h




void MPU6050_Init(){
     
}

void sendGyroscopeStatusForGame(String _status, String _time){
      client.send("{\"action\":\"InGameType\",\"type\":\"ringData\",\"sender\":\"sensor1\",\"gamehost\":\""+gameHostId+"\",\"yawData\":\""+_status+"\",\"rollDataTime\":\""+_time+"\"}");
}



void GetGyroscopeData(){

    
   
}





#endif
