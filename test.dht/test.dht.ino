#include <SPI.h>  
#include "dht.h"

#define dht_apin A0

dht DHT;

void setup(){
  Serial.begin(9600);
}

void loop()
{
  Serial.println("before send data");
  DHT.read11(dht_apin);
  Serial.print("temp:");
  Serial.println(DHT.temperature);
  Serial.print("humidity:");
  Serial.println(DHT.humidity);
  delay(5*1000);
}



