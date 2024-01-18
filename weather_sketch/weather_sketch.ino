#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include "bsec.h"
#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h" 


#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C

float hum_weighting = 0.25; // so hum effect is 25% of the total air quality score
float gas_weighting = 0.75; // so gas effect is 75% of the total air quality score

float hum_score, gas_score;
float gas_reference = 250000;
float hum_reference = 40;
int   getgasreference_count = 0;

const char WEBSITE[] = "api.pushingbox.com"; //pushingbox API server
const char devid[] = SECRET_DEVID; //device ID on Pushingbox for our Scenario

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;
char server[] = "api.thingspeak.com";    // name address for Google (using DNS)
WiFiClient client;

boolean DEBUG = true;

int temperatureData;
int pressureData;
int humidityData;
// High concentration of VOCs = lower resistance
int gasResistance;
int altitudeData;

float air_quality_score;
float humidityScore;
float gasScore;
String airQualityScore;


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Wire.begin();
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  } else Serial.println("Found a sensor");

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_2X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_2X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320°C for 150 ms
  // Now run the sensor for a burn-in period, then use combination of relative humidity and gas resistance to estimate indoor air quality as a percentage.
  GetGasReference();

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the status:
  printWifiStatus();
}


void loop() {

  temperatureData = bme.readTemperature();
  pressureData = bme.readPressure();
  humidityData = bme.readHumidity();
  // High concentration of VOCs = lower resistance
  gasResistance = bme.readGas();
  altitudeData = bme.readAltitude(SEALEVELPRESSURE_HPA);

  if (! bme.performReading()) {
    Serial.println("Failed to perform reading from sensor");
    return;
  }
  Serial.print("Temperature = ");
  Serial.print(bme.temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bme.pressure / 100.0);
  Serial.println(" hPa");

  Serial.print("Humidity = ");
  Serial.print(bme.humidity);
  Serial.println(" %");

  Serial.print("Gas = ");
  Serial.print(bme.gas_resistance);
  Serial.println(" R/n");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.println();

  calculateContributions();

  Serial.println("Air Quality = "+String(air_quality_score,1)+"% derived from 25% of Humidity reading and 75% of Gas reading - 100% is good quality air");
  Serial.println("Humidity element was : "+String(humidityScore)+" of 0.25");
  Serial.println("     Gas element was : "+String(gasScore)+" of 0.75");
  if (gasResistance < 120000) Serial.println("***** Poor air quality *****");
  Serial.println();
  if ((getgasreference_count++)%10==0) GetGasReference(); 
  Serial.println(airQualityScore);
  Serial.println("------------------------------------------------");
   

  Serial.println("\nSending Data to Server..."); 
  // listen for incoming clients
  // setup the connection to google sheet
  sendToPushingBox();

  if (client.available()) {
    char c = client.read();
    if(DEBUG){Serial.print(c);}
  }
  
  // amount of time between measurements 1 hour (ms)
  delay(3600000);
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void GetGasReference(){
  // Now run the sensor for a burn-in period, then use combination of relative humidity and gas resistance to estimate indoor air quality as a percentage.
  Serial.println("Getting a new gas reference value");
  int readings = 10;
  for (int i = 1; i <= readings; i++){ // read gas for 10 x 0.150mS = 1.5secs
    gas_reference += bme.readGas();
  }
  gas_reference = gas_reference / readings;
}

String CalculateIAQ(float score){
  String IAQ_text = "Air_quality_is_";
  score = (100-score)*5;
  if      (score >= 301)                  IAQ_text += "Hazardous";
  else if (score >= 201 && score <= 300 ) IAQ_text += "Very_Unhealthy";
  else if (score >= 176 && score <= 200 ) IAQ_text += "Unhealthy";
  else if (score >= 151 && score <= 175 ) IAQ_text += "Unhealthy_for_Sensitive_Groups";
  else if (score >=  51 && score <= 150 ) IAQ_text += "Moderate";
  else if (score >=  00 && score <=  50 ) IAQ_text += "Good";
  return IAQ_text;
}

void sendToPushingBox(){
  client.stop(); if(DEBUG){Serial.println("connecting...");}
  if(client.connect(WEBSITE, 80)) { 
    if(DEBUG){Serial.println("connected");}
    if(DEBUG){Serial.println("sending request");}
    String queryString = {String("&temperatureData=") 
      + String(temperatureData) + String("&pressureData=") 
      + String(pressureData) + String("&humidityData=") 
      + String(humidityData) + String("&gasResistance=") 
      + String(gasResistance) + String("&airQualityScore=") 
      + String(airQualityScore) + String("&humidityScore=") 
      + String(humidityScore) + String("&gasScore=") 
      + String(gasScore) + String("&altitudeData=") 
      + String(altitudeData)};
    client.print("GET /pushingbox?devid=");
    client.print(devid);
    client.print(queryString);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(WEBSITE);
    client.println("User-Agent: MKR1010/1.0");
    client.println("Connection: close");
    client.println();
    if(DEBUG){Serial.println("request sent");}
  } 
  else { 
    if(DEBUG){Serial.println("connection failed");} 
  } 
}

void calculateContributions(){
  //Calculate humidity contribution to IAQ index
  float current_humidity = humidityData;
  if (current_humidity >= 38 && current_humidity <= 42)
    hum_score = 0.25*100; // Humidity +/-5% around optimum 
  else
  { //sub-optimal
    if (current_humidity < 38) 
      hum_score = 0.25/hum_reference*current_humidity*100;
    else
    {
      hum_score = ((-0.25/(100-hum_reference)*current_humidity)+0.416666)*100;
    }
  }
  
  //Calculate gas contribution to IAQ index
  float gas_lower_limit = 5000;   // Bad air quality limit
  float gas_upper_limit = 50000;  // Good air quality limit 
  if (gas_reference > gas_upper_limit) gas_reference = gas_upper_limit;
  if (gas_reference < gas_lower_limit) gas_reference = gas_lower_limit;
  gas_score = (0.75/(gas_upper_limit-gas_lower_limit)*gas_reference -(gas_lower_limit*(0.75/(gas_upper_limit-gas_lower_limit))))*100;

  //Combine results for the final IAQ index value (0-100% where 100% is good quality air)
  air_quality_score = hum_score + gas_score;
  humidityScore = hum_score / 100;
  gasScore = gas_score / 100;
  airQualityScore = CalculateIAQ(air_quality_score);
}