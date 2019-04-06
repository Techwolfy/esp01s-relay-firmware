// ESP-01S WiFi Relay Controller

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include "index.h"
 
//
// Constants
//

#define GPIO_RELAY 0
#define GPIO_LED 2

#define SSID "ssid"
#define PASS "password"
#define HOST "esp01s-relay"
 
//
// Function prototypes
//

void handleRoot();
void handleControl();
void handleStatus();
void handleNotFound();

//
// Global variables
//

ESP8266WebServer server(80);
bool relayState = false;
bool ledState = false;

//
// Setup function, called once at startup
//
void setup()
{
    //
    // Start serial output
    //

    Serial.begin(115200);
    //Serial.setDebugOutput(true);

    //
    // Set up GPIO pins
    //

    pinMode(GPIO_RELAY, OUTPUT);
    pinMode(GPIO_LED, OUTPUT);
   
    //
    // Connect to WiFi network
    //
   
    Serial.print("Connecting to ");
    Serial.println(SSID);

    WiFi.hostname(HOST);
    WiFi.mode(WIFI_STA);
    WiFi.setPhyMode(WIFI_PHY_MODE_11N);
    WiFi.begin(SSID, PASS);
    
    int count = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");

        count++;
        if (count == 80) {
            Serial.println();
            WiFi.printDiag(Serial);
            Serial.print("Status: ");
            Serial.println(WiFi.status());

            count = 0;
        }
    }
   
    Serial.println();
    Serial.println("WiFi connected!");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Netmask: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("Gateway: ");
    Serial.println(WiFi.gatewayIP());

    //
    // Start mDNS responder
    //
  
    Serial.println();
    if (MDNS.begin(HOST)) {
        Serial.println("mDNS responder started.");
    } else {
        Serial.println("Error setting up MDNS responder!");
    }

    //
    // Start HTTP server
    //
  
    server.on("/", handleRoot);
    server.on("/control", handleControl);
    server.on("/status", handleStatus);
    server.onNotFound(handleNotFound);
    server.begin();

    Serial.println();
    Serial.println("HTTP server started.");
    Serial.println();
}

//
// Loop function, called in infinite loop after setup
//
void loop()
{
    //
    // Listen for HTTP requests
    //

    server.handleClient();
}

//
// Print the URI and query string of the current request
//
void printRequest()
{
    Serial.print("Received HTTP request for: ");
    Serial.print(server.uri());

    for(int i = 0; i < server.args(); i++)
    {
        if (i == 0)
        {
            Serial.print("?");
        }
        else
        {
            Serial.print("&");
        }

        Serial.print(server.argName(i));
        Serial.print("=");
        Serial.print(server.arg(i));
    }

    Serial.println();
}

//
// Callback for root web page
//
void handleRoot()
{
    printRequest();

    //
    // Display root page
    //

    server.send(200, "text/html", HTML);
}

//
// Callback for control endpoint
//
void handleControl()
{
    int pin = 0;
    bool state = false;

    printRequest();

    //
    // Toggle Relay/LED
    //

    if (!server.hasArg("pin"))
    {
        server.send(500);
        return;
    }

    pin = server.arg("pin").toInt();
    if (pin != GPIO_RELAY && pin != GPIO_LED)
    {
        server.send(500);
        return;
    }

    if (!server.hasArg("state"))
    {
        if (pin == GPIO_RELAY)
        {
            relayState = !relayState;
            state = relayState;
        }
        if (pin == GPIO_LED)
        {
            ledState = !ledState;
            state = ledState;
        }
    }
    else
    {
        state = server.arg("state").toInt() != 0;
    }

    digitalWrite(pin, state);

    server.send(200);
}

//
// Callback for status endpoint
//
void handleStatus()
{
    char buf[256] = {0};

    printRequest();

    //
    // Send JSON-formatted GPIO status
    //

    sprintf(buf,
            "{\"relayActive\":%s, \"ledActive\":%s, \"hostName\":\"%s\", \"macAddress\":\"%s\", \"networkSSID\":\"%s\", \"ipAddress\":\"%s\", \"subnetMask\":\"%s\", \"gatewayAddress\":\"%s\"}",
            relayState ? "true" : "false",
            ledState ? "true" : "false",
            HOST,
            WiFi.macAddress().c_str(),
            SSID,
            WiFi.localIP().toString().c_str(),
            WiFi.subnetMask().toString().c_str(),
            WiFi.gatewayIP().toString().c_str());

    server.send(200, "application/json", buf);
}

//
// Callback for unrecognized web page
//
void handleNotFound()
{
    printRequest();

    //
    // Send 404 error message
    //

    server.send(200, "text/plain", "404 Page Not Found");
}
