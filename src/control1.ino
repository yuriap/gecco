#include <LiquidCrystal.h>
#include "DHT.h"

////////////////////////////////////////////////////////////////////////
//Display initializing
#define LCD_LIGHT_PIN A4
const int buttonPin = 6;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int buttonState = 0;

////////////////////////////////////////////////////////////////////////
//Humidity and temperature sensor setup
#define DHTPIN 13     // data pin

//sensor type
#define DHTTYPE DHT22   // DHT 22  (AM2302)

//Humidity and temperature sensor initializing
DHT dht(DHTPIN, DHTTYPE);

////////////////////////////////////////////////////////////////////////
//Relay setup
const int pin_UV_Lamp   = 7; // UV Lamp
const int pin_DAY_Lamp  = 8; // Day Lamp
const int pin_NightLamp = 9; // Night Lamp

const int pinOut4 = 10;

////////////////////////////////////////////////////////////////////////
//Configuration
const int   DayNightLevel = 150;  //photo resistor level
const float NightLow      = 25;  // Night low temp
const float NightHigh     = 27;  // Night high temp
const float DayLow        = 26;  // Day low temp
const float DayHigh       = 28;  // Day high temp
float HighTemp;
float LowTemp;

const int   CycleDelay    = 1000; //Cycle delay millisecs
const int   CyclesRead    = 30;   //Read sensor and turn on/off relays every CyclesRead cycles
const int   CyclesDisplay = 10;   //Display ON duration

////////////////////////////////////////////////////////////////////////
//Variables
int   light;
float h;
float t;
int   readcycles = 0;
int   dispcycles = 0;

//int   DayLampState   = 0;
//int   NightLampState = 0;

const int   Cold     = 1;
const int   Hot      = 2;
int   ColdHot        = 0; // 0 init, 1 cold, 2 hot

int IfDebug = 0;
////////////////////////////////////////////////////////////////////////
void debug(String msg) {
  if (IfDebug==1)
  {
    Serial.println(msg);
  }
}

void turn_relay_on(int pin) {
  digitalWrite(pin, LOW);
}

void turn_relay_off(int pin) {
  digitalWrite(pin, HIGH);
}

void setup() {
  Serial.begin(9600);
  
  debug("Start UP:"); 
  
//+Display Setup
  // Setup the number of columns and rows that are available on the LCD.
  lcd.begin(16, 2);
  lcd.noDisplay();

  // Set the button pin as an input.
  pinMode(buttonPin, INPUT);

   // Set the LCD display backlight pin as an output.
  pinMode(LCD_LIGHT_PIN, OUTPUT);

  // Turn off the LCD backlight.
  lcd.display();
  digitalWrite(LCD_LIGHT_PIN, HIGH);  
  lcd.print("Starting up...");
  delay(5000);

  // Turn off the LCD backlight.
  //lcd.noDisplay();
  digitalWrite(LCD_LIGHT_PIN, LOW);
    
//-Display Setup 

//+Humidity and temperature sensor Setup
  dht.begin();
//-Humidity and temperature sensor Setup

//+Relay Setup
  pinMode(pin_UV_Lamp, OUTPUT);
  //digitalWrite(pin_UV_Lamp, LOW);
  turn_relay_off(pin_UV_Lamp);
  
  pinMode(pin_DAY_Lamp, OUTPUT);
  //digitalWrite(pin_DAY_Lamp, LOW);
  turn_relay_off(pin_DAY_Lamp);
  
  pinMode(pin_NightLamp, OUTPUT);
  //digitalWrite(pin_NightLamp, LOW);
  turn_relay_off(pin_NightLamp);
  
  pinMode(pinOut4, OUTPUT);
  //digitalWrite(pinOut4, LOW); 
  turn_relay_off(pinOut4);
//-Relay Setup  
}

void loop() {

debug("Cycling=========================="); 
debug("readcycles: " + String(readcycles)); 
debug("dispcycles: " + String(dispcycles)); 

    //Humidity and temperature reading
    h = dht.readHumidity();
    t = dht.readTemperature();
    if (isnan(t) || isnan(h)) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Read error DHT ");
    } 
  
    //Photo Resistor reading
    light = analogRead(A0);

  if (readcycles==0)
  {
    readcycles = CyclesRead;

debug("Readings:"); 
debug("humidity:    " + String(h)); 
debug("temperature: " + String(t)); 
debug("light:       " + String(light));

    /////////////////////////////////

    if (light>DayNightLevel)
    {
      HighTemp = DayHigh;
      LowTemp  = DayLow;
    }
    else
    {
      HighTemp = NightHigh;
      LowTemp  = NightLow;      
    }
    
debug("Limits:"); 
debug("HighTemp:" + String(HighTemp)); 
debug("LowTemp: " + String(LowTemp)); 
    
    if (ColdHot==0)
      { //Init Hot/Cold status
      if (t<HighTemp)
      {
        ColdHot = Cold;
      }
      else
      {
        ColdHot = Hot;
      }
debug("Init status:"); 
debug("ColdHot:    " + String(ColdHot));     
    }
    else
    {
      if ((ColdHot==Cold)&&(t>=HighTemp))
      {
        ColdHot = Hot;
      }
      if ((ColdHot==Hot)&&(t<LowTemp))
      {
        ColdHot = Cold;
      }      
debug("Cycling status:"); 
debug("ColdHot:    " + String(ColdHot));       
    }
    
    if (light>DayNightLevel) // Day Lamp
    {
      turn_relay_off(pin_NightLamp);
debug("Night Lamp OFF"); 
      turn_relay_on(pin_UV_Lamp);
debug("UV Lamp ON");        
      if (ColdHot==Cold) 
      {
        //digitalWrite(pin_DAY_Lamp, HIGH); // turn it ON
        turn_relay_on(pin_DAY_Lamp);
debug("Day Lamp ON");           
      }

      if (ColdHot==Hot) 
      {
        //digitalWrite(pin_DAY_Lamp, LOW); // turn it OFF
        turn_relay_off(pin_DAY_Lamp);
debug("Day Lamp OFF");         
      }      
    }
  
   
    if (light<=DayNightLevel) // Night Lamp
    {
      turn_relay_off(pin_DAY_Lamp);
debug("DAY Lamp OFF");        
      turn_relay_off(pin_UV_Lamp);
debug("UV Lamp OFF");        

      if (ColdHot==Cold) 
      {
        //digitalWrite(pin_NightLamp, HIGH); // turn it ON
        turn_relay_on(pin_NightLamp);
debug("Night Lamp ON");          
      }

      if (ColdHot==Hot) 
      {
        //digitalWrite(pin_NightLamp, LOW); // turn it OFF
        turn_relay_off(pin_NightLamp);
debug("Night Lamp OFF");          
      }      
    } 
   
    //digitalWrite(pinOut4, LOW);
    //digitalWrite(pinOut4, HIGH);   
  }
  
  /////////////////////////////////

    // Print text to the LCD.
    lcd.clear();
    lcd.print("Light Humid Temp");
    lcd.setCursor(0, 1);
    lcd.print(light);
    lcd.setCursor(5, 1);
    lcd.print(h);
    lcd.setCursor(11, 1);
    lcd.print(t);  
    
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH)
  {
    dispcycles = CyclesDisplay;

    // Turn the backlight on.
    digitalWrite(LCD_LIGHT_PIN, HIGH);

    // Display the text on the LCD.
    //lcd.display();
debug("Light display........................");     
  }



  if(dispcycles>0)
  {
    
//    if (dispcycles % 3 == 0)
//    {
//      
//    }
    
    if (dispcycles==1)
    {
      //lcd.noDisplay();
      digitalWrite(LCD_LIGHT_PIN, LOW);
debug("Stop display........................");          
    }
    dispcycles = dispcycles - 1;
debug("dispcycles2:    " + String(dispcycles));     
  }
  
  readcycles = readcycles - 1;  
  delay(CycleDelay);  

}
