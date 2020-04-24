#ifndef gyroscopeFunc_h
#define gyroscopeFunc_h


const int MPU_addr=0x68; 
void MPU6050_Init(){
  delay(150);
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); 
  Wire.write(0);
  Wire.endTransmission(true);
}



long crtClockForGyroscope = 0; 
long lastClockForGyroscope = 0; 
long lastClockForG = 0;

void GetGyroscopeData(){
  double AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
  crtClockForGyroscope = millis();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true); // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY=Wire.read()<<8|Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  if(crtClockForGyroscope-lastClockForGyroscope >1000){

      /*
      Serial.print("AcX = "); 
      Serial.print(AcX);
      Serial.print("||"); 
      */
      AcX = AcX;
      if(AcX>50000){
        AcX = (AcX - 57600)/(65400-57600) -1;
      }else{
        AcX = (AcX)/(8500);
        AcX = min(1.0,AcX);
      }
      AcX = - AcX;
      if(AcX < -1){
        AcX = -1;
      }
      if(AcX > 1){
        AcX = 1;
      }
      /*
      Serial.print(AcX);
      Serial.print("\n");
      */

      /*
      Serial.print("AcY = "); 
      Serial.print(AcY);
      Serial.print("||"); */
      AcY = AcY;

      if(AcY>50000){
        AcY = (AcY - 57300)/(65000-57300) -1;
      }else{
        AcY = (AcY)/(8300);
      }
      AcY = - AcY;
      if(AcY < -1){
        AcY = -1;
      }
      if(AcY > 1){
        AcY = 1;
      }
      /*
      Serial.print(AcY);
      Serial.print("\n");
      */
      
      /*
      Serial.print("AcZ = "); 
      Serial.print(AcZ);
      Serial.print("||"); 
      */
      AcZ = AcZ;
      if(AcZ>50000){
        AcZ = (AcZ - 57200)/(64000-57200) -1;
      }else{
        AcZ = (AcZ)/(8300);
      }
      AcZ = - AcZ;
      if(AcZ < -1){
        AcZ = -1;
      }
      if(AcZ > 1){
        AcZ = 1;
      }
      /*
      Serial.print(AcZ);
      Serial.print("\n");
      */

      /*
      Serial.print("GyZ = "); 
      Serial.print(GyZ);
      Serial.print("||"); 
      GyroZ = (max(0.0,GyroZ-550.0) / 60477.0)*(crtClockForGyroscope-lastClockForGyroscope)/1000;
      Serial.print("GyroZ= "); 
      Serial.print(GyroZ);
      Serial.print("\n");
      */
      Serial.print("ping\n"); 
      lastClockForGyroscope = crtClockForGyroscope;
  }

      if(lastClockForG==0){
         lastClockForG = crtClockForGyroscope;
      }
      if(GyZ>64400){
        Serial.print("GyZ = "); 
        Serial.print(GyZ);
        Serial.print("||"); 
        GyZ  = max(0.0,(GyZ-64400))/7800;
        GyZ = GyZ ;
        Serial.print(GyZ);
        Serial.print("\n");
      }
      lastClockForG = crtClockForGyroscope;
}

#endif
