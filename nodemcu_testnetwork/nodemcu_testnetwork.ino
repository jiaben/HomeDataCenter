#include <ESP8266WiFi.h>

//using wifi manager
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

int ledPin = D2;
WiFiServer server(80);

int value = LOW;
bool bBlinked = false;

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  WiFiManager wifiMng;

  IPAddress ip (192, 168, 1, 249);
  IPAddress gw (192, 168, 1, 1);
  IPAddress sn (255, 255, 255, 0);
  wifiMng.setSTAStaticIPConfig(ip, gw, sn);

  if (!wifiMng.autoConnect("AutoConnectAP")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.reset();
    delay(5000);
  }


  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

}

void checkBlink() {
  if (bBlinked) {
    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW);
    delay(1000);
  }
}

void loop() {

  checkBlink();
  // Check if a client has connected
  WiFiClient client = server.available();
  if (client) {


    // Wait until the client sends some data
    Serial.println("new client");
    while (!client.available()) {
      delay(1);
    }

    // Read the first line of the request
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();
    //digitalWrite(ledPin, LOW);
    // Match the request

    if (request.indexOf("/LED=ON") != -1)  {
      digitalWrite(ledPin, HIGH);
      value = HIGH;
      bBlinked = false;
    }
    if (request.indexOf("/LED=OFF") != -1)  {
      digitalWrite(ledPin, LOW);
      value = LOW;
      bBlinked = false;
    }
    if (request.indexOf("/LED=BLINK") != -1) {
      value = HIGH;
      bBlinked = true;
    }

    // Set ledPin according to the request
    //digitalWrite(ledPin, value);

    // Return the response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println(""); //  do not forget this one
    client.println("<!DOCTYPE HTML>");
    client.println("<html style=\"font-size:50px !important;\">");

    client.print("Led pin is now: ");

    if (value == HIGH) {
      client.print("On");
    } else {
      client.print("Off");
    }
    client.println("<br><br>");
    client.println("<a href=\"/LED=ON\"\"><button style=\"font-size:50px;\">Turn On </button></a>");
    client.println("<a href=\"/LED=BLINK\"\"><button style=\"font-size:50px;\">BLINK </button></a>");
    client.println("<a href=\"/LED=OFF\"\"><button style=\"font-size:50px;\">Turn Off </button></a><br />");
    client.println("</html>");

    delay(1);
    //client.stop();
    Serial.println("Client disonnected");
    Serial.println("");
  }


}

