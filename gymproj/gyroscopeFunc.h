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

void GetGyroscopeData(){
  float AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
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

  //if(crtClockForGyroscope-lastClockForGyroscope >1000){
     lastClockForGyroscope = crtClockForGyroscope;
      
      Serial.print("AcX = "); 
      Serial.print(AcX);
      Serial.print("||"); 
      AcX = AcX;
      if(AcX>50000){
        AcX = (AcX - 57600)/(65400-57600) -1;
      }else{
        AcX = (AcX)/(8500);
        AcX = min(1.0f,AcX);
      }
      AcX = - AcX;

      
      /*
      Serial.print(" | AcY = "); Serial.print(AcY);
      Serial.print(" | AcZ = "); Serial.print(AcZ);
      Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53); //equation for temperature in degrees C from datasheet
      Serial.print(" | GyX = "); Serial.print(GyX);
      Serial.print(" | GyY = "); Serial.print(GyY);
      Serial.print(" | GyZ = "); Serial.println(GyZ);
      */
      Serial.print(AcX);
      Serial.print("\n");
     
      delay(1000);
  //}

}

#endif
