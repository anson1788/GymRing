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
const int MPU_addr=0x68; 


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

void sendGyroscopeStatusForGame(String _status, String _time){
      client.send("{\"action\":\"InGameType\",\"type\":\"ringData\",\"sender\":\"sensor1\",\"gamehost\":\""+gameHostId+"\",\"yawData\":\""+_status+"\",\"rollDataTime\":\""+_time+"\"}");
}

long crtClockForGyroscope = 0; 
long lastClockForGyroscope = 0; 
bool startConfig;

void GetGyroscopeData(){

    
    crtClockForGyroscope = millis();


    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) {
        if (mpuInterrupt && fifoCount < packetSize) {
          // try to get out of the infinite loop 
          fifoCount = mpu.getFIFOCount();
        }  
    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();
    if(fifoCount < packetSize){
            //Lets go back and wait for another interrupt. We shouldn't be here, we got an interrupt from another event
        // This is blocking so don't do it   while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    }
    // check for overflow (this should never happen unless our code is too inefficient)
    else if ((mpuIntStatus & _BV(MPU6050_INTERRUPT_FIFO_OFLOW_BIT)) || fifoCount >= 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
      //  fifoCount = mpu.getFIFOCount();  // will be zero after reset no need to ask
        Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & _BV(MPU6050_INTERRUPT_DMP_INT_BIT)) {

     // read a packet from FIFO
    while(fifoCount >= packetSize){ // Lets catch up to NOW, someone is using the dreaded delay()!
      mpu.getFIFOBytes(fifoBuffer, packetSize);
      // track FIFO count here in case there is > 1 packet available
      // (this lets us immediately read more without waiting for an interrupt)
      fifoCount -= packetSize;
    }


    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
   
    if(crtClockForGyroscope-lastClockForGyroscope >100){
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            double angle= ((double)gz)*(crtClockForGyroscope-lastClockForGyroscope) / 5000.0;
            int dl = (int)round(angle);
            Serial.print("angle :");
            Serial.print(dl);
            Serial.print("\n");
        
            if(dl>5 || dl<-5){
                double _deltaTime = (crtClockForGyroscope-lastClockForGyroscope)/1000;
                sendGyroscopeStatusForGame(String(dl),String(_deltaTime));
            }
            lastClockForGyroscope = crtClockForGyroscope;
     }
   
   }
  


   
}




#endif
