#ifndef helperClass_h
#define helperClass_h

class SmoothValueDouble{
    public:
      double dataInt[SAMPLE_SIZE_DOUBLE];
      int idx = 0;
      void insert(double add){
          int TmpIn[SAMPLE_SIZE_DOUBLE];
          TmpIn[SAMPLE_SIZE_DOUBLE-1]=add;
          for(int i=SAMPLE_SIZE_DOUBLE-2;i>=0;i--){
            TmpIn[i] = dataInt[i+1];
          };
          for(int i=SAMPLE_SIZE_DOUBLE-1;i>=0;i--){
            dataInt[i] = TmpIn[i];
          };
      };
      double getValue(){
         double returnVal = 0;
         for(int i=0;i<SAMPLE_SIZE_DOUBLE;i++){
          returnVal = returnVal + dataInt[i];
         };    
         return returnVal/SAMPLE_SIZE_DOUBLE;
      }
};
class SmoothValueInt{
    public:
      int dataInt[SAMPLE_SIZE_INT];
      int idx = 0;
      void insert(int add){
          int TmpIn[SAMPLE_SIZE_INT];
          TmpIn[SAMPLE_SIZE_INT-1]=add;
          for(int i=SAMPLE_SIZE_INT-2;i>=0;i--){
            TmpIn[i] = dataInt[i+1];
          };
          for(int i=SAMPLE_SIZE_INT-1;i>=0;i--){
            dataInt[i] = TmpIn[i];
          };
      };
      int getValue(){
         int returnVal = 0;
         for(int i=0;i<SAMPLE_SIZE_INT;i++){
          returnVal = returnVal + dataInt[i];
         };    
         return returnVal/SAMPLE_SIZE_INT;
      }
};



class SmoothValueFlexInt{
    public:
      int dataInt[SAMPLE_SIZE_FLEX_INT];
      int idx = 0;
      void insert(int add){
          int TmpIn[SAMPLE_SIZE_FLEX_INT];
          TmpIn[SAMPLE_SIZE_FLEX_INT-1]=add;
          for(int i=SAMPLE_SIZE_FLEX_INT-2;i>=0;i--){
            TmpIn[i] = dataInt[i+1];
          };
          for(int i=SAMPLE_SIZE_FLEX_INT-1;i>=0;i--){
            dataInt[i] = TmpIn[i];
          };
      };
      int getValue(){
         int returnVal = 0;
         for(int i=0;i<SAMPLE_SIZE_FLEX_INT;i++){
          returnVal = returnVal + dataInt[i];
         };    
         return returnVal/SAMPLE_SIZE_FLEX_INT;
      }

      int ifStable(int multiple){
           int totalSum = 0;
           for(int i=0;i<SAMPLE_SIZE_FLEX_INT;i++){
              totalSum = totalSum + dataInt[i];
           }
           int mean = totalSum/SAMPLE_SIZE_FLEX_INT;
           totalSum = 0;
           for(int i=0;i<SAMPLE_SIZE_FLEX_INT;i++){
              totalSum = abs(mean - dataInt[i]);
           }
           if(totalSum<SAMPLE_SIZE_FLEX_INT*multiple){
              return 1;
           }
              return 0;
      }


      int ifShapeValueChange(int checkVal){
        
        for(int i=4;i<SAMPLE_SIZE_FLEX_INT-3;i++){
          if(abs(dataInt[i]-dataInt[i-1])>checkVal){
            Serial.println("abnormal value come");
            Serial.println(abs(dataInt[i]-dataInt[i-1]));
            Serial.println(i);
            int totalSum = 0;
            for(int j=i+1;j<SAMPLE_SIZE_FLEX_INT;j++){
              totalSum = totalSum + abs(dataInt[i]-dataInt[j]);
             //Serial.println(totalSum);
            }
            Serial.println("-----");
            Serial.println((SAMPLE_SIZE_FLEX_INT-i)*20-totalSum);
            Serial.println("-----");
            if(totalSum<(SAMPLE_SIZE_FLEX_INT-i)*20){
              Serial.println("shape change occur");
              Serial.println();
              if(dataInt[i]>dataInt[i-1]){
                Serial.println(1);
                return 1;
              }else{
                 Serial.println(-1);
                return -1;
              }
            }
          }
        };
        
        return 0;
      }
     
      int ifValueStable(int checkVal){
        int totalSum = 0;
        for(int i=0;i<SAMPLE_SIZE_FLEX_INT;i++){
              totalSum = totalSum + abs(dataInt[i]-dataInt[i]);
        };
        if(totalSum<checkVal*checkVal){
          return 1;
        }
        return 0;
      }
};


class GyroscopeNetworkClass{
	public:
	    long lastClockForGyroscope;
		  String yawAngle;
		  String pitchAngle;
		  String rollAngle;
		  String animDuration;
		  float dataIsSent = true;
			
		
};


class FlexNetworkClass{
  public:
      long lastClockForFlex;
      String flexPercentage;
      String animDuration;
      float dataIsSent = true;
      
    
};



#endif
