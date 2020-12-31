#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"
#include "FS.h"

/*Put your SSID & Password*/
const char* ssid = "wifi_name";  // Enter SSID here
const char* password = "wifi_password";  //Enter Password here

ESP8266WebServer server(80);

float temp, humi;
               
// Initialize DHT sensor.
const byte dhtPin = D5;
#define DHTTYPE DHT11
DHT dht(dhtPin, DHTTYPE);             

float Temperature = 0;
float Humidity = 0;
char buffer_temp[1000];
char buffer_humidity[1000];
 
void setup() {
if(SPIFFS.format())
  {
    Serial.println("File System Formated");
  }
  else
  {
    Serial.println("File System Formatting Error");
  }
  
//FILE
  bool success = SPIFFS.begin();
 
  if (!success) {
    Serial.println("Error mounting the file system");
    return;
  }
//FILE


  
    dht.begin();
  Serial.begin(115200);
  delay(100);            
  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");

}

void update_buffs(){
    File temperatureFile = SPIFFS.open("/temperatureFile.txt", "r");
    File humidityFile= SPIFFS.open("/humidityFile.txt", "r");
 
  if (!temperatureFile || !humidityFile) {
    Serial.println("Failed to open file for reading");
    return;
  }
 
  Serial.println("File Content:");
 
 
    temperatureFile.readBytes(buffer_temp, temperatureFile.size());
    humidityFile.readBytes(buffer_humidity, humidityFile.size());
    buffer_temp[temperatureFile.size()] = 0;
  Serial.println(buffer_temp);
  Serial.println("\n");
  Serial.println(buffer_humidity);
  
  temperatureFile.close();
  humidityFile.close();
 
}

void loop() {
  if(getTH() == true)
  {
    Serial.print("Temp  = ");
    Serial.print(temp);
    Serial.print("  Humidity  = ");
    Serial.print(humi);

    File temperatureFile = SPIFFS.open("/temperatureFile.txt", "a");
    File humidityFile = SPIFFS.open("/humidityFile.txt", "a");
   
    if (!temperatureFile || !humidityFile) {
      Serial.println("Error opening file for writing");
      return;
    }
      temperatureFile.print(temp);
      temperatureFile.print(",");

      humidityFile.print(humi);
      humidityFile.print(",");
      update_buffs();
      delay(2000);
   
    temperatureFile.close();
    humidityFile.close();
  
  }
  server.handleClient();
  
}

boolean getTH()
{
  static unsigned long timer = 0;
  unsigned long interval = 5000;

  if (millis() - timer > interval)
  {
    timer = millis();   
    humi = dht.readHumidity();
    temp = dht.readTemperature(); 
    if (isnan(humi) || isnan(temp))
    {
      Serial.println("Failed to read from DHT sensor!");     
      return false;
    }
    return true;
  }
  else
  {
    return false;
  }
}

void handle_OnConnect() {

  float Temperature = dht.readTemperature(); // Gets the values of the temperature
  float Humidity = dht.readHumidity(); // Gets the values of the humidity 
  server.send(200, "text/html", SendHTML(Temperature,Humidity, String(buffer_temp), String(buffer_humidity))); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float Temperaturestat,float Humiditystat, String buffer_temp, String buffer_humidity){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<!DOCTYPE html>";
ptr +="<html>";
ptr +="<meta http-equiv='refresh' content='5'>";
ptr +="<body>";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>ESP8266 NodeMCU Weather Report</h1>\n";
  
  ptr +="<p>Temperatura: ";
  ptr +=(float)Temperaturestat;
  ptr +="°C</p>";
  ptr +="<p>Umiditate: ";
  ptr +=(float)Humiditystat;
  ptr +="%</p>";
  ptr +="<p>buffer_temp: ";
  ptr +=(String)buffer_temp;
  ptr +="%</p><br>";
  ptr +="<p>buffer_temp: ";
  ptr +=(String)buffer_humidity;
  ptr +="%</p><br>";
  ptr +="<canvas id='myCanvas' width='2000' height='300' style='border:1px solid #d3d3d3;'>";
    ptr +="Your browser does not support the HTML canvas tag.</canvas>";
      ptr +="<canvas id='myCanvas2' width='2000' height='300' style='border:1px solid #d3d3d3;'>";
    ptr +="Your browser does not support the HTML canvas tag.</canvas>";
  ptr +="<script>";
    ptr +="var x ='" + (String)buffer_temp + "';";
     ptr +="entryArray = x.split(',');";
    ptr +="var inputdata= entryArray;";
    ptr +="const pointdist = 50;";
  ptr +="var currentposition=70;";
  ptr +="const basepath=150;";
 ptr +="var c = document.getElementById('myCanvas');";
  ptr +="var ctx = c.getContext('2d');";
  ptr +="ctx.beginPath();";
  ptr +="ctx.moveTo(50, basepath);";
  ptr +="inputdata.forEach(element =>{";
        ptr +="ctx.lineTo(currentposition, basepath-element);";
        ptr +="ctx.font = '15px serif';";
        ptr +="ctx.fillText(element, currentposition-7, basepath-element);";
        ptr +="currentposition+=pointdist;";
    ptr +="});";
    ptr +="ctx.stroke();";
    ptr +="currentposition=70;";
    ptr +="ctx.strokeStyle = 'black';";
    ptr +="inputdata.forEach(element =>{";
        ptr +="ctx.lineWidth = 5;";
        ptr +="ctx.lineCap = element;";
        ptr +="ctx.beginPath();";
        ptr +="ctx.moveTo(currentposition, basepath);";
        ptr +="ctx.lineTo(currentposition, basepath-element);";
        ptr +="ctx.stroke();";
        ptr +="currentposition+=pointdist;";
    ptr +="});";
    ptr +="ctx.font = '24px serif';";
    ptr +="ctx.fillText('Variatie timp', 80, basepath+20);";
    ptr +="ctx.font = '24px serif';";
    ptr +="ctx.fillText('Temperatura', 0, basepath/2+10);";
    ptr +="</script>";

     ptr +="<script>";
    ptr +="var y ='" + (String)buffer_humidity + "';";
     ptr +="entryArray = y.split(',');";
    ptr +="var inputdata= entryArray;";
    ptr +="const pointdist2 = 50;";
  ptr +="var currentposition=70;";
  ptr +="const basepath2=150;";
 ptr +="var c = document.getElementById('myCanvas2');";
  ptr +="var ctx = c.getContext('2d');";
  ptr +="ctx.beginPath();";
  ptr +="ctx.moveTo(50, basepath2);";
  ptr +="inputdata.forEach(element =>{";
        ptr +="ctx.lineTo(currentposition, basepath2-element);";
        ptr +="ctx.font = '15px serif';";
        ptr +="ctx.fillText(element, currentposition-7, basepath2-element);";
        ptr +="currentposition+=pointdist2;";
    ptr +="});";
    ptr +="ctx.stroke();";
    ptr +="currentposition=70;";
    ptr +="ctx.strokeStyle = 'black';";
    ptr +="inputdata.forEach(element =>{";
        ptr +="ctx.lineWidth = 5;";
        ptr +="ctx.lineCap = element;";
        ptr +="ctx.beginPath();";
        ptr +="ctx.moveTo(currentposition, basepath2);";
        ptr +="ctx.lineTo(currentposition, basepath2-element);";
        ptr +="ctx.stroke();";
        ptr +="currentposition+=pointdist;";
    ptr +="});";
    ptr +="ctx.font = '24px serif';";
    ptr +="ctx.fillText('Variatie timp', 80, basepath2+20);";
    ptr +="ctx.font = '24px serif';";
    ptr +="ctx.fillText('Umiditate:', 0, basepath2/2+10);";
    ptr +="</script>";
ptr +="</body>";
ptr +="</html>";
  return ptr;
}

