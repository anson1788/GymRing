#ifndef flexFunc_h
#define flexFunc_h


int lastFlex32Val = -999;

long crtClockForFlex = -999; 
long lastClockForFlex = -999; 

void GetFlexData(){
 // int potValue = analogRead(35);
  int potValue32 = analogRead(35);
   Serial.print("pot data ");
   Serial.print(potValue32);
   Serial.print("\n");
  if(lastFlex32Val = -999){
      lastFlex32Val = potValue32;
  }else{
      lastFlex32Val = lastFlex32Val*0.5 + potValue32*0.5;
  }

  crtClockForFlex = millis();   

  if(lastClockForFlex == -999){
    lastClockForFlex = crtClockForFlex;
  }

  if(crtClockForFlex-lastClockForFlex > 100){
      /*
        long lastClockForFlex;
        String flexPercentage;
        String animDuration;
        float dataIsSent = true;
       */
      // 700 /1000 500

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
       
       Serial.print("flex data");
       Serial.print(lastFlex32Val);
       Serial.print("----");
       Serial.print(flexPercentage);
       Serial.print("\n");

      float timeDiff = (float)(crtClockForFlex-lastClockForFlex)/1000;
      flexNetworkData.dataIsSent = false;
      flexNetworkData.lastClockForFlex = crtClockForFlex;
      flexNetworkData.animDuration = String(timeDiff);
      flexNetworkData.flexPercentage= String(flexPercentage);
      lastClockForFlex = crtClockForFlex;
  }
  
}

#endif
