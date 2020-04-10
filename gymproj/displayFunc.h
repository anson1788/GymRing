#ifndef display_h
#define display_h

void DisplayInit(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
}



void DisplayDrawContentForIRInput(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  
  
  if(crtInput == "enter" && inputString == ""){
     state = BLESettingMode;
     return;
  }else if(crtInput!=""){
      if(crtInput=="enter"){
        state = StartSocketConnection;
        return;
      }
      
      if(crtInput!="-"){
         inputString = inputString + crtInput;
         Serial.println(" input val = " + inputString);  
      }else{
         inputString = inputString.substring(0, inputString.length() - 1);
      }
      display.print(inputString);
      display.display();
      delay(500);
  }else if(inputString!=""){
    display.print(inputString);
    display.display();
  }else{
     display.print("Please input roomid");  
     display.display();
  }

}


void DisplayDrawContent(String text){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print(text);
  display.display();
}
void HandleIRData(){

  crtInput = "";
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    int rv = 0;
    switch (results.value)
    {
      case 0xFF18E7: rv = -10 ;   Serial.println(" UP ARROW"); break;
      
      case 0x8C22657B: rv = -11 ;  crtInput = "-"; Serial.println(" LEFT ARROW");    break;
      case 0xFF10EF: rv = -11 ;  crtInput = "-"; Serial.println(" LEFT ARROW");    break;

      case 0x488F3CBB: rv = -12; crtInput = "enter";  Serial.println(" -OK-");    break;
      case 0xFF38C7: rv = -12 ;  crtInput = "enter";  Serial.println(" -OK-");    break;
      
      case 0xFF5AA5: rv = -13 ;   Serial.println(" RIGHT ARROW");   break;
      case 0xFF4AB5: rv = -14 ;   Serial.println(" DOWN ARROW"); break;
      
      case 0xE318261B: rv = 1   ; crtInput = "1";   Serial.println(" 1");    break; 
      case 0xFFA25D: rv = 1   ; crtInput = "1";   Serial.println(" 1");    break;
      
      case 0xFF629D: rv = 2   ; crtInput = "2";  Serial.println(" 2");    break;
      case 0x511DBB: rv = 2   ; crtInput = "2";  Serial.println(" 2");    break;
      
      case 0xFFE21D: rv = 3   ; crtInput = "3";  Serial.println(" 3");    break;
      case 0xEE886D7F: rv = 3   ; crtInput = "3";  Serial.println(" 3");    break;      

      case 0x52A3D41F: rv = 4   ; crtInput = "4";  Serial.println(" 4");    break;
      case 0xFF22DD: rv = 4   ; crtInput = "4";  Serial.println(" 4");    break;

      case 0xD7E84B1B: rv = 5   ; crtInput = "5";  Serial.println(" 5");    break;
      case 0xFF02FD: rv = 5   ; crtInput = "5";  Serial.println(" 5");    break;

      case 0x20FE4DBB: rv = 6   ; crtInput = "6";  Serial.println(" 6");    break;
      case 0xFFC23D: rv = 6   ; crtInput = "6";  Serial.println(" 6");    break;

      case 0xF076C13B: rv = 7   ; crtInput = "7";  Serial.println(" 7");    break;
      case 0xFFE01F: rv = 7   ; crtInput = "7";  Serial.println(" 7");    break;
      
      case 0xA3C8EDDB: rv = 8   ; crtInput = "8";  Serial.println(" 8");    break;
      case 0xFFA857: rv = 8   ; crtInput = "8";  Serial.println(" 8");    break;

      case 0xE5CFBD7F: rv = 9   ; crtInput = "9";  Serial.println(" 9");    break;
      case 0xFF906F: rv = 9   ; crtInput = "9";  Serial.println(" 9");    break;
      
      case 0xFF9867: rv = 0   ; crtInput = "0";  Serial.println(" 0");    break;
      case 0x97483BFB: rv = 0   ; crtInput = "0";  Serial.println(" 0");    break;
            
      case 0xFF6897: rv = -15 ;   Serial.println(" *");    break;
      case 0xFFB04F: rv = -16 ;   Serial.println(" #");    break;
        // case 0xFFFFFFFF: rv = -20 ; break; // Serial.println(" REPEAT");    break;
        // default: break ;
    }

 
    irrecv.resume(); // Receive the next value
  }
}


void checkIsGameStartInput(){
  if(crtInput == "enter"){
     state = InGameMode;
     return;
  }
}


#endif
