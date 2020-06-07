#ifndef flexFunc_h
#define flexFunc_h


int lastFlex32Val = -999;

long crtClockForFlex = -999; 
long lastClockForFlex = -999; 


int flexSensorVal = 0;
int flex_S = 0;
int flexHighpass = 0;
void initFlex(){

}


void updateFlexValue(){
  /*
  int potValue32 = analogRead(35);
  flexSensorVal = potValue32;       
  flex_S = (0.3*flexSensorVal) + ((1-0.3)*flex_S);
  flexHighpass = flexSensorVal - flex_S;  
  
  if(lastFlex32Val = -999){
      lastFlex32Val = potValue32;
  }else{
      lastFlex32Val = lastFlex32Val*0.5 + potValue32*0.5;
  }*/
  

}

void GetFlexData(){
       Serial.print("flexData :");
       Serial.print(flexHighpass);
       Serial.print("\n");
       float calVal = (float)lastFlex32Val;
       float limit = 300;
       if(lastFlex32Val > 1200){
          limit=300;
       }
       if(lastFlex32Val>1400){
          calVal = 1400;
       }
       if(lastFlex32Val<850){
          calVal = 850; 
       }

       float flexPercentage = 1+((lastFlex32Val-1100)/limit)*0.2;

       if(flexPercentage>1.3){
         flexPercentage= 1.3;
       }
       if(flexPercentage<0.7){
         flexPercentage= 0.7;
       }
       /*
      float timeDiff = (float)(crtClockForFlex-lastClockForFlex)/1000;
      */
      mNetworkData.dataIsSent = false;
      mNetworkData.flexPercentage= String(flexPercentage);
      
}

#endif
