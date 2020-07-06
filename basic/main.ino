#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("FlankenDetektor"));
  display.display();
  
  pinMode(11,INPUT_PULLUP);       //Pin11 Input
  pinMode(10,OUTPUT);
  delay(100);
  for(int i=0;i<10;i++){
    digitalWrite(10,HIGH);
    delay(100);
    digitalWrite(10,LOW);
    delay(100);
  }
  delay(1000);
}


byte toggle=0;
byte set=0;
int begin=0,ende=0;
int last=0;
int sek=0;
int lastSek=0;
int stunde=0,minute=0;
int current=0;
byte wertArray[7]={1,2,4,8,10,20};
short int bits[59];                    //DCF datensatz
int travel=0,bitsSet=0;

void loop() {
  
  int io13=digitalRead(11);       //Pin11 Input
  delay(17);//<------------------Extrem wichtig für den µC zur Stabilisierung
    
  if(io13!=set){                  //Flankenwechsel    

      set=io13;
      
      if(io13==LOW){              //fallende Flanke

        travel=millis()-last;
        
        last=millis();
        
        if(travel>1500){          //minute voll
          lastSek=sek;
          
          if(sek==58){
            digitalWrite(10,HIGH);    // signal länge OK
            stunde=0;
            minute=0;
            bitsSet=0;
                  
            if(bits[19]==1){
            for (int i=28;i<34;i++){
              if(bits[i]==1){
              stunde+=wertArray[i-28];
              bitsSet++;
             }
            }
             for (int i=20;i<27;i++){
              if(bits[i]==1){
              minute+=wertArray[i-20];
              bitsSet++;
             }}
              /*Serial.println(stunde);
              display.clearDisplay();
              bitter(bits,19,30,0,0);
              display.display();*/
            }
            //}
          }else{
            digitalWrite(10,LOW);     // signal länge nicht OK
          }
          
          sek=0;                      // sekunden wieder auf NULL
          
        }else{
                  
          if((travel>900) && (travel<1100)){
                        
            sek++;
            
          }else{
                        
          }
        }

       
      }//<----------------------------------------------------------fallende Flanke
      
      if(io13==HIGH){                                               //steigende Flanke        
        current=millis()-last;
        Serial.println(current);
        
        if((sek-1)<59){                                             //Grundbedingung für brauchbaren Wert
          
          if((current<150) && (current>50)){
            
            bits[sek-1]=0;
            
            Serial.println("LOW");
          }
          
          if((current<250) && (current>=150)){
            
            bits[sek-1]=1;
            
            Serial.println("HIGH");
          }

          
        }//<-------------------------------------------------------Grundbedingung für brauchbaren Wert
      }//<---------------------------------------------------------steigende Flanke
      anzeige(travel,sek,lastSek,stunde,bits[sek-1],minute);
      
    }//<-----------------------------------------------------------Flankenwechsel
    
}
void bitter(short int *input,int n0, int n1,int x,int y){
                  //int x=0,y=0;
                  //display.clearDisplay();
                  display.setTextSize(1);             // Normal 1:1 pixel scale
                  display.setTextColor(SSD1306_WHITE);        // Draw white text
                  for(int i=n0;i<n1+1;i++){
                  
                        x++;
                        if(x>20){
                          y++;
                          x=0;
                        }
                  display.setCursor(x*7,y*10);             // Start at top-left corner
                  display.println(input[i]);
                  
                  }
                  //display.display();
}
void anzeige(int interval,int sek,int lastSek,int stunde,int bitsSet,int minute){
          display.clearDisplay();
          display.setTextSize(1);             // Normal 1:1 pixel scale
          display.setTextColor(SSD1306_WHITE);        // Draw white text
          display.setCursor(0,0);             // Start at top-left corner
          display.println(F("FlankenDetektor"));
          display.setCursor(0,10);             // Start at top-left corner
          display.println(interval);
          display.setCursor(64,10);             // Start at top-left corner
          display.println(sek);
          display.setCursor(0,20);             // Start at top-left corner
          display.println(lastSek);
          display.setCursor(64,20);             // Start at top-left corner
          display.println(bitsSet);
          display.setCursor(0,30);             // Start at top-left corner
          display.println(stunde);
          display.setCursor(64,30);             // Start at top-left corner
          display.println(minute);
          display.display();
}
