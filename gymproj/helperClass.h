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
