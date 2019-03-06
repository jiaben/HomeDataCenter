#include <SPI.h>
#include <Ethernet.h>
#include <RF24.h>
#include "ArduinoJson.h"
#include "printf.h"


const uint64_t rAddress[] = {
    0x7878787878LL, 
    0xB3B4B5B6F1LL, 
    0xB3B4B5B6CDLL, 
    0xB3B4B5B6A3LL, 
    0xB3B4B5B60FLL, 
    0xB3B4B5B605LL 
  };
RF24 myRadio (5, 6);


struct package
{
  int cmdid = 0;
  int ForwarderId = 0;
  uint16_t info_size = 0;
};
typedef struct package Package;
Package data;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 87);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

StaticJsonDocument<512> arr;

void initJsonObj() {
  for (int i = 0; i < 3; ++i) {
    arr.createNestedObject();
    arr[i]["forwarder"] = i;
    arr[i]["info"] = "";
  }
}


void setup() {
  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

  initJsonObj();
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Ethernet WebServer Example");

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  printf_begin();
  myRadio.begin();

  //myRadio.setChannel(11);
  //myRadio.setAutoAck(false);
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_250KBPS);

  myRadio.openReadingPipe(0, rAddress[0]);
  myRadio.openReadingPipe(1, rAddress[1]);
  myRadio.openReadingPipe(2, rAddress[2]);
  myRadio.openReadingPipe(3, rAddress[3]);
  myRadio.openReadingPipe(4, rAddress[4]);
  myRadio.openReadingPipe(5, rAddress[5]);

  myRadio.startListening();
  myRadio.printDetails();
}

void getRadio() {
  uint8_t u_pipe = 0;
  while (myRadio.available(&u_pipe)) {
    Serial.print("pipe num is");
    Serial.println(u_pipe);
    //for (int i = 0; i < pipe; ++i) {
    myRadio.read( &data, sizeof(data) );
    
    char buf[512];
    int info_size = data.info_size;
    myRadio.read(buf, info_size);
    arr[data.ForwarderId]["info"] = buf;
  }
}
void loop() {

  if (myRadio.available())
    getRadio();

  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {

          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Access-Control-Allow-Origin: *");
          client.println("Content-Type: application/json");
          client.print("Content-Length: ");
          client.println(measureJson(arr));

          client.println("Connection: close");  // the connection will be closed after completion of the response

          //client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          serializeJson(arr, client);

          client.println();
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    //Serial.println("client disconnected");
  }
  delay(1);
}

