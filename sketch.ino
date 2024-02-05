#define BLYNK_TEMPLATE_ID "TMPL6dH4jHEW5"
#define BLYNK_TEMPLATE_NAME "Kelompok 1"
#define BLYNK_AUTH_TOKEN "AvK7KRN47W9mWkBjoFgMlzn1NkEZwknv"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <DHTesp.h>
#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BlynkSimpleEsp32.h>

#define SCREEN_WIDTH 128 // OLED width,  in pixels
#define SCREEN_HEIGHT 64 // OLED height, in pixels

const char* wifiSsid = "Wokwi-GUEST";
const char* wifiPassword = "";
int ldrPin = 34;
int servoPin = 4;
int DHT_PIN = 13;
int buzzerPin = 32;
int ledPin = 12;
int val;
String tempString,tempString1,ldrString, ldrString1;
float tempCelcius;
float gama = 0.7;
float rl10 = 50;
unsigned long waktu;
unsigned long prevTime;


#define switchVpin V0
#define statusVpin V1
#define tempVpin V2
#define lightVpin V3

Servo servo;
DHTesp dhtSensor;
#define display_RESET     -1 // Reset pin 
#define SCREEN_ADDRESS 0x3C ///address i2c oled 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


BLYNK_CONNECTED() 
{
    Serial.println("Blynk connected");
    Blynk.syncVirtual(switchVpin);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(ldrPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  servo.attach(servoPin);
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  Blynk.begin(BLYNK_AUTH_TOKEN, wifiSsid, wifiPassword);

   if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  prevTime = 0;
  delay(2000);         // wait for initializing
  display.clearDisplay(); // clear display

  //display.setTextSize(2);          // text size
  //display.setTextColor(WHITE);     // text color

  //displayDisplayCenter("SUHU : ", 4);

  servo.write(0);
  tone(buzzerPin,0);
  Serial.println ("finish.setup");

}


void loop() 
{
  Blynk.run();
  waktu = millis();
  if (waktu-prevTime >= 1000)
  {
    //if (ledPin == HIGH){
    
    Blynk.virtualWrite(V0,1);
      
    int ldrStatus = analogRead(ldrPin);
    ldrStatus = map(ldrStatus, 4095, 0, 1024, 0); //mengubah nilai pembacaan sensor LDR dari nilai ADC arduino menjadi nilai ADC ESP32
    float voltase = ldrStatus / 1024.*5;
    float resistansi = 2000 * voltase / (1-voltase/5);
    float ldrStatus1 = pow(rl10*1e3*pow(10,gama)/resistansi,(1/gama));

    TempAndHumidity  data = dhtSensor.getTempAndHumidity();
    tempCelcius = data.temperature;
    Blynk.virtualWrite(V2, tempCelcius);
    Blynk.virtualWrite(V3, ldrStatus1 );
    if (ldrStatus1 < 400 && tempCelcius >= 70 )
    {
      servo.write(45);
      delay (20);
      tone(buzzerPin,200);
      Blynk.virtualWrite(V1,1);
    }
    else if (ldrStatus1 < 400 && tempCelcius < 70)
    {
      servo.write(90);
      delay (20);
      tone(buzzerPin,0);
      Blynk.virtualWrite(V1,0);
    }
    else if (ldrStatus1 >= 1126 && tempCelcius < 70)
    {
      servo.write(45);
      delay (20);
      tone(buzzerPin,0);
      Blynk.virtualWrite(V1,0);
    }
    else if (ldrStatus1 >= 1126 && tempCelcius >= 70)
    {
      servo.write(45);
      delay(20);
      tone(buzzerPin,200);
      Blynk.virtualWrite(V1,1);
    }
    tempString  = String(tempCelcius, 2); // two decimal places
    tempString += (char)247;
    tempString += "C";
    ldrString  = String(ldrStatus1, 2); // two decimal places
    ldrString += "Lux";
    //biar ketumpuk hitam dulu jika suhu nya berubah
      
    if(tempString != tempString1 || ldrString != ldrString1)
    {
      display.setTextColor(BLACK);
      displayDisplayCenter(tempString1,10);
      displayDisplayCenter(ldrString1,50);
    }
    tempString1= tempString; 
    ldrString1= ldrString;
    Serial.println(tempString); // print the temperature in Celsius to Serial Monitor
    Serial.println(ldrStatus1);

    //lanjut tulis warna putih
    display.setTextColor(WHITE);
    display.setTextSize(2);
    displayDisplayCenter(tempString,10);
    displayDisplayCenter(ldrString,50);
  } 
  else 
  {
    Blynk.virtualWrite(V0, 0);
    tone(buzzerPin, 0);
    servo.write(90);
    delay(2000);         // wait for initializing
    display.setTextColor(WHITE,BLACK); // clear display
  }
  prevTime = waktu;
}

void displayDisplayCenter(String text, int posisi) 
{
  int16_t x1;
  int16_t y1;
  uint16_t width;
  uint16_t height;

  display.getTextBounds(text, 0, 0, &x1, &y1, &width, &height);

  display.setCursor((SCREEN_WIDTH - width) / 2, posisi);
  display.println(text); // text to display
  display.display();
}
