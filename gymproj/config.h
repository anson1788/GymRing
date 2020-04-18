
#ifndef config_h
#define config_h

//sample size
#define SAMPLE_SIZE_DOUBLE 10
#define SAMPLE_SIZE_INT 10
#define SAMPLE_SIZE_FLEX_INT 1

enum GameState { WaitForIRInput, StartSocketConnection, WaitForSocketConnection, WaitingForGameToStart,InGameMode, BLESettingMode };
enum SocketState { WaitForWifi,WaitForFirstConnect, WaitForControllerMsg, RegisterCompleted ,RequestingGameToStart,GameStarted};

//wifiConfig
char path[] = "/uat";
char host[] = "fmt0duuywk.execute-api.us-east-1.amazonaws.com";
String websockets_server = "wss://fmt0duuywk.execute-api.us-east-1.amazonaws.com/uat";


//screen display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


//gyroscope
const uint8_t MPU6050SlaveAddress = 0x68;

// sensitivity scale factor respective to full scale setting provided in datasheet 
const uint16_t AccelScaleFactor = 16384;
const uint16_t GyroScaleFactor = 131;

// MPU6050 few configuration register addresses
const uint8_t MPU6050_REGISTER_SMPLRT_DIV   =  0x19;
const uint8_t MPU6050_REGISTER_USER_CTRL    =  0x6A;
const uint8_t MPU6050_REGISTER_PWR_MGMT_1   =  0x6B;
const uint8_t MPU6050_REGISTER_PWR_MGMT_2   =  0x6C;
const uint8_t MPU6050_REGISTER_CONFIG       =  0x1A;
const uint8_t MPU6050_REGISTER_GYRO_CONFIG  =  0x1B;
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG =  0x1C;
const uint8_t MPU6050_REGISTER_FIFO_EN      =  0x23;
const uint8_t MPU6050_REGISTER_INT_ENABLE   =  0x38;
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H =  0x3B;
const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET  = 0x68;


#endif
