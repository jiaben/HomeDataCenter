/*
* Arduino Wireless Communication Tutorial
*     Example 1 - Transmitter Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>  
#include "dht.h"
#include "RF24.h"
#include "printf.h"

#define dht_apin A0 // Analog Pin sensor is connected to

byte addresses[][6] = {"jia01"};
uint64_t addr = 0xB3B4B5B6CDLL;

struct package
{
  int id = 0;
  double temperature = 0.0;
  double humidity = 0.0;
  char  text[16] = "Colding";
};

RF24 myRadio(8,9);
dht DHT;
typedef struct package Package;
Package data;


void setup() 
{
  Serial.begin(9600);
  printf_begin();
  delay(1000);

  myRadio.begin(); 
  
  //myRadio.setAutoAck(false);
  //myRadio.setChannel(11); 
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_250KBPS ) ;
  //myRadio.disableDynamicPayloads();
  myRadio.openWritingPipe(addr);
  myRadio.stopListening();
  myRadio.printDetails();
}

void printData(const Package & dataInfo )
{
    Serial.println("----------");
    Serial.print(dataInfo.id);
    Serial.print(" ");
    Serial.print(dataInfo.text);
    Serial.print(" ");
    Serial.print(dataInfo.temperature);
    Serial.print(" ");
    Serial.println(dataInfo.humidity);
    Serial.println("--------------");
}

int step_num = 0;

void loop()
{
  step_num ++;
  if (step_num == 10){
    DHT.read11(dht_apin);
    data.temperature = DHT.temperature;
    data.humidity = DHT.humidity;
  
    printData(data);
    
    bool flag = myRadio.write(&data, sizeof(data));
    Serial.println(flag);
     
    data.id = data.id + 1;
    step_num = 0;
  }
  else{
    delay(1000);
  }
}
