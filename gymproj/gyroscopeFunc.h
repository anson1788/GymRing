#ifndef gyroscopeFunc_h
#define gyroscopeFunc_h

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#define INTERRUPT_PIN 2  // use pin 2 on Arduino Uno & most boards
#define SDA 21
#define SCL 22
#define OUTPUT_READABLE_YAWPITCHROLL
#include <Math.h>
MPU6050 mpu;



uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           
VectorInt16 aa;         
VectorInt16 aaReal;     
VectorInt16 aaWorld;    
VectorFloat gravity;    
float euler[3];         
float ypr[3];   
  

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}



void initI2Cdev(){
    Wire.begin(SDA, SCL, 400000);
    while (!Serial);
    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT_PULLUP);
    Serial.println(F("Initializing DMP..."));
    Serial.println(F("\nSend any character to begin DMP programming and demo: "));
    mpu.dmpInitialize();

  
    mpu.setXAccelOffset(-911); 
    mpu.setYAccelOffset(356); 
    mpu.setZAccelOffset(1906); 
    mpu.setXGyroOffset(-73);
    mpu.setYGyroOffset(32);
    mpu.setZGyroOffset(-127);
   
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);
    mpu.PrintActiveOffsets();
    
    mpu.setXAccelOffset(mpu.AccelOffX); 
    mpu.setYAccelOffset(mpu.AccelOffY); 
    mpu.setZAccelOffset(mpu.AccelOffZ); 
    mpu.setXGyroOffset(mpu.GyroOffX);
    mpu.setYGyroOffset(mpu.GyroOffY);
    mpu.setZGyroOffset(mpu.GyroOffZ);
    
     // turn on the DMP, now that it's ready
    Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
    Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
    Serial.println(F(")..."));
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();
    Serial.println(F("DMP ready! Waiting for first interrupt..."));
    packetSize = mpu.dmpGetFIFOPacketSize();

}

void MPU6050_Init(){
    initI2Cdev();
}

void sendGyroscopeStatusForGame(String yawData, String pitchData, String _time){
  //  client.send("{\"action\":\"InGameType\",\"type\":\"ringData\",\"sender\":\"sensor1\",\"gamehost\":\""+gameHostId+"\",\"yawData\":\""+yawData+"\",\"pitchData\":\""+pitchData+"\",\"rollDataTime\":\""+_time+"\"}");
}

void sendRequestCalibration(bool isData){
	String value = "false";
	if(isData){
		value = "true";
	}
  //  client.send("{\"action\":\"InGameType\",\"type\":\"ringData\",\"sender\":\"sensor1\",\"gamehost\":\""+gameHostId+"\",\"calibratingFlag\":\""+value+"\"}");	
}

long crtClockForGyroscope = 0; 
long lastClockForGyroscope = 0; 


bool startConfig;
enum MotionStatus {requestCalibration,calibrating,detecting };
MotionStatus ringMotionStatus = requestCalibration;

float lastYaw = -999;
float lastPitch = -999;
float lastRoll = -999;
int xSensorVal = 0;
int x_S = -999;
int xHighpass = 0;
long xSendTime = -999;
int valZ = 0;
void calibratingForMPU(){
        x_S = -999;
        ringMotionStatus = MotionStatus::calibrating;
        mpu.resetFIFO();
        mpu.initialize();
        mpu.dmpInitialize();
        mpu.CalibrateAccel(6);
        mpu.CalibrateGyro(6);
        mpu.PrintActiveOffsets();
        
        mpu.setXAccelOffset(mpu.AccelOffX); 
        mpu.setYAccelOffset(mpu.AccelOffY); 
        mpu.setZAccelOffset(mpu.AccelOffZ); 
        mpu.setXGyroOffset(mpu.GyroOffX);
        mpu.setYGyroOffset(mpu.GyroOffY);
        mpu.setZGyroOffset(mpu.GyroOffZ);
             // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);
    
        // enable Arduino interrupt detection
        Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
        Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
        Serial.println(F(")..."));
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        packetSize = mpu.dmpGetFIFOPacketSize();
        ringMotionStatus = MotionStatus::detecting;
        lastYaw = -999;
        lastPitch = -999;
}



void GetGyroscopeData(){
      
         
          while (!mpuInterrupt && fifoCount < packetSize) {
              if (mpuInterrupt && fifoCount < packetSize) {
                fifoCount = mpu.getFIFOCount();
              }  
          }
      
          mpuInterrupt = false;
          mpuIntStatus = mpu.getIntStatus();
      
          // get current FIFO count
          fifoCount = mpu.getFIFOCount();
          if(fifoCount < packetSize){
              Serial.println(F("FIFO error!"));
          }else if ((mpuIntStatus & _BV(MPU6050_INTERRUPT_FIFO_OFLOW_BIT)) || fifoCount >= 1024) {
              mpu.resetFIFO();
              Serial.println(F("FIFO overflow!"));
          } else if (mpuIntStatus & _BV(MPU6050_INTERRUPT_DMP_INT_BIT)) {
      
              while(fifoCount >= packetSize){ 
                mpu.getFIFOBytes(fifoBuffer, packetSize);
                fifoCount -= packetSize;
              }
                mpu.dmpGetQuaternion(&q, fifoBuffer);
                mpu.dmpGetAccel(&aa, fifoBuffer);
                mpu.dmpGetGravity(&gravity, &q);
                mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
                mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
                float yawAngle =ypr[0] * 180/M_PI;
                float pitchAngle = ypr[1] * 180/M_PI;
                float rollAngle = ypr[2] * 180/M_PI;
                if(lastYaw!=-999 && abs(lastYaw-yawAngle) > 120){
                     ringMotionStatus = MotionStatus::requestCalibration;
                }
                if(lastPitch!=-999 && abs(lastPitch-pitchAngle) > 120){
                     ringMotionStatus = MotionStatus::requestCalibration;
                }
                if(lastRoll!=-999 && abs(lastRoll-rollAngle) > 120){
                     ringMotionStatus = MotionStatus::requestCalibration;
                }

         
                //Serial.print(aaReal.x);
                //Serial.print(" ");

                if(x_S == -999){
                    x_S = aaReal.x;
                }else{
                    xSensorVal = aaReal.x;       
                    x_S = (0.3*xSensorVal) + ((1-0.3)*x_S);
                    xHighpass = xSensorVal - x_S;  
                }
                valZ = aaReal.z;
               
                /*
                Serial.print(aaReal.y);
                Serial.print("\t");
                Serial.println(aaReal.z);
                */
                
                mNetworkData.dataIsSent = false;
                mNetworkData.yawAngle = String(yawAngle);
                mNetworkData.pitchAngle = String(pitchAngle);
                mNetworkData.rollAngle  = String(rollAngle);

                
                lastYaw = yawAngle;
                lastPitch = pitchAngle;
                lastRoll = rollAngle;
         }

}





#endif
