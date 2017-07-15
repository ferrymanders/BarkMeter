#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// Below are my custom settings, if you want to use this script remove the next two includes
#include <WiFiSettings.h>
#include <BarkMeterSettings.h>

// Uncomment lines below to setup wifi
// const char* ssid     = "SSID";       // WiFi SSID
// const char* password = "WiFiPassword";  // WiFi Password

// Uncomment lines below to set needed settings
// const char* host="sub.domain.tld"; // Hostname to send the alerts to
// const char* url="/path/to/script.php"; // URL on the hostname to send the alerts to

const int sampleWindow = 500; // Sample window width in mS (500 mS = 2Hz)
const int signalThreshold = 3; // Threshold from where to report the loud noise
unsigned int sample;

// ***** Do not edit below this line ***************

WiFiClient client;

void setup() 
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rWorking to connect");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}
 
void loop() 
{
  unsigned long startMillis= millis();  // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level
 
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
 
  // collect data for $sampleWindow mS
  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(0);
    if (sample < 1024)  // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample;  // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  double volts = (peakToPeak * 5.0) / 1024;  // convert to volts

  if (volts > signalThreshold)
  {
    post(volts);
  }
  Serial.println(volts);
}

void post(double volts)
{
  String postVolts=String(volts,2);
  String postData="sensor=blaf&data="+postVolts;
  if (client.connect(host, 80)) {
    Serial.println("connected");
    client.println("POST " + String(url) + " HTTP/1.1");
    client.println("Host:  " + String(host));
    client.println("User-Agent: Katoma-WeMos/1.0");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.print("Content-Length: ");
    client.println(postData.length());
    client.println();
    client.println(postData);
  } else {
    Serial.println("connection failed");
  }  
}
