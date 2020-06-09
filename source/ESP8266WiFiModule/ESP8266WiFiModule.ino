/*
 * But to run the ESP8266 PLEASE NOTE that the ESP8266 needs more power, connect VCC to external power 3.3,FTDI doesn't have enough power.
 * Build/Flash Steps:
 * -Insert flasher with switch to the outside(program)
 * -grant permission for /dev/ttyUSB*
 * -choose the device from the Ard.'s tools menu
 * -Click on compile & flash
 * -unplug and switch to run (push the sliding button inwards)
 * -grant permission for /dev/ttyUSB*
 * -choose the device from the Ard.'s tools menu
 * open console
 */
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h> 
//#include <ESP8266WiFiMulti.h>
//#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#define  _DEBUG 
#define DEFAULT_SSID            "ARRIS-2906-2"
#define DEFAULT_SSID_PASSWORD   NULL
#define DEFAULT_SSID_2          "ROR194"
#define DEFAULT_SSID_PASSWORD_2 "salah987"
#define MAX_CONNECT_TIMEOUT     10000
#define BAUD_RATE               115200
#define BAUD_RATE_2             38400
#define PAYLOAD_PREFIX          "<pl>"
#define PAYLOAD_SUFFIX          "</pl>"

ESP8266WebServer server;
uint8_t pin_led = 0;
char* password = "password";
char* mySsid = "AP_ESP8266";

IPAddress local_ip(192,168,11,4);
IPAddress gateway(192,168,11,1);
IPAddress netmask(255,255,255,0);

char webpageBody[] PROGMEM = R"=====(
<html>
<head>
 <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=yes">
<style>
    #appNameID {FONT-SIZE: 2em; COLOR: Blue; text-align: center}
    table, th, td {
      border: 1px solid black;
      border-collapse: collapse;
    }
    th, td {
      padding: 5px;
      text-align: left;
    }
</style>
<script>
//onReady()
document.addEventListener('DOMContentLoaded', function(){ 
   refreshPlayerStatus();
}, false);
function execGETAjax(url, callback){
   var xhttp = new XMLHttpRequest();
   xhttp.onreadystatechange = function(){;
     if(this.readyState == 4 && this.status == 200){
       if ( callback != null ) 
          callback(xhttp);
     };
   };
   xhttp.open("GET", url, true);
   xhttp.send();
};
function reloadMainPage(){
  execGETAjax("/updateMainPage");
}
function reloadAppInfo(){
  execGETAjax("/getAppInfo",getAppInfo);
}
function execPOSTAjax(url, payloadAsString, callback){
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function(){;
     if(this.readyState == 4 && this.status == 200){
       if ( callback != null ) 
           callback(xhttp);
     };
  };
  xhttp.open("POST", url, true);
  xhttp.send(payloadAsString);
};
function getAppInfo(xhttp){
  if ( xhttp.responseText != null ){
    var appInfo = JSON.parse(xhttp.responseText)
    document.getElementById("appNameID").innerHTML = appInfo.appName;
  }
}
function setStatusMsg(xhttp){
    if ( this.responseText != null ){
      console.log(xhttp.responseText);
      var respJson = JSON.parse(xhttp.responseText);
      document.getElementById("msg").innerHTML = respJson.msg;
      console.log(respJson.msg);
  }
  else
      console.log("not response text");
}
function sendActionEvent( ){
  var eventName             = document.getElementById("actionEventNameID").value;
  var eventData             = document.getElementById("actionEventDataID").value;
  var payload         = {actionEvent:eventName,data: eventData };
  execPOSTAjax("/actionEvent",JSON.stringify(payload),setStatusMsg);
};
</script>
</head>
<body>
<span id="appNameID">Struts4Embedded</span>
</br>
<table id=ActionTableId>
  <caption><b>Action Events</B></caption>
  <tr>
    <th>
        <input value="" id="actionEventNameID"  size=10>
    </th>
    <th>
        <input value="" id="actionEventDataID"  size=10>
    </th>
    <th>
        <button class="primary" id="sendActionEventId" type="button" onclick="sendActionEvent()">Send Msg</button>
      </th>
  </tr>
</table>
</br></br>
 <div>
     <button class="primary" id="reloadMainPageId" type="button" onclick="reloadMainPage()">Reload Main Page</button>
 </div>
</br>
 <div>
     <button class="primary" id="reloadAppInfoId" type="button" onclick="reloadAppInfo()">Reload App Info</button>
 </div>
</body>
</html>
)=====";
void handleRoot();
void getPlayerStatus();
void toggleLED();
void updateMainPage();
void actionEvent();
void connectToWiFi(const char *_ssid, const char *_pass);
int httpGET( String &url, const char *fingerP, String &resp);
int execHTTPS(String &host, String &url, String &contentType, String &httpMethod, const char *fp, int httpsPort, String &pl, String &resp);
void postPayloadExternally(String &postPL);
int    connectedToWiFi = 0;
void setup(){
  pinMode(pin_led, OUTPUT);
  Serial.begin(BAUD_RATE);
  Serial.setRxBufferSize(512);
  connectToWiFi(DEFAULT_SSID,DEFAULT_SSID_PASSWORD);

  if (WiFi.status() != WL_CONNECTED)
      connectToWiFi(DEFAULT_SSID_2,DEFAULT_SSID_PASSWORD_2);
  server.on("/",                handleRoot);
  server.on("/actionEvent",     actionEvent);
  server.on("/getPlayerStatus", getPlayerStatus);
  server.on("/getAppInfo",      getAppInfo);
  server.on("/updateMainPage",  updateMainPage);
   
  server.begin();
}
String appName        = "Struts4Embedded!";
String reloadHost     = "raw.githubusercontent.com";
String reloadURL      = "/abusous2000/Struts4Embedded/master/source/ESP8266WiFiModule/defaultPage.html";
String mcuPayload     = "{\"status\":\"--\",\"track\":\"------\",\"vol\":\"0\",\"timeRemaining\":\"-:--\"}";
String appInfoPayload = "{\"appName\":\"Struts4Embedded!\"}";
String reloadFP       = "70 94 DE DD E6 C4 69 48 3A 92 70 A1 48 56 78 2D 18 64 E0 B7";

String postExternally = ""; 
String mainPage = "N/A";

void loop(){
  int               i = 0;
  String readString = "";
   while (Serial.available()) {
    if ( i++ % 2 == 0 )  
      delay(1);  //delay to allow buffer to fill 
    if (Serial.available() >0) {
      char c = Serial.read();  //gets one byte from serial buffer
      readString += c; //makes the string readString
    }
  }
  int payloadStartAt = readString.length()> 0 ? readString.indexOf(PAYLOAD_PREFIX): -1; 
  if (payloadStartAt >=0) {
      int payloadEndsAt  = readString.indexOf(PAYLOAD_SUFFIX);
      if ( payloadEndsAt >= 0 ){
          Serial.printf("Processing PL:%s\n",readString.c_str());
          String postPL = readString.substring(payloadStartAt+strlen(PAYLOAD_PREFIX),payloadEndsAt);
          if ( readString.indexOf("timeRemaining")>0 )
             mcuPayload = postPL;
          else
      if ( readString.indexOf("appName")>0 ){
        appInfoPayload = postPL;
        parseAppInfo(postPL);
      }
      else
          if ( readString.indexOf("postExt")>0 ){
               postPayloadExternally(postPL);
          }
          else
             Serial.printf("Unhandled/Dropped PL:%s\r\n",readString.c_str()); 
      }
     else
       Serial.printf("Mangled Msg:%s:\r\n",readString.c_str()); 
     readString = "";
   }
  
   server.handleClient();
}
void parseAppInfo(String &postPL){
     DynamicJsonBuffer  jBuffer;
     JsonObject         &jObject = jBuffer.parseObject(postPL);
     
     if(!jObject.success()){
          Serial.println("unable to parse payloadJSON:");
         return;
     }
     String reloadHost2     = jObject["reloadHost"];
     String reloadURL2      = jObject["reloadURL"];
     String appName2        = jObject["appName"];
     String reloadFP2       = jObject["fp"];
     String ssid            = jObject["ssid"];
     String pwd             = jObject["pwd"];
     
     if ( ssid != "" && ssid != DEFAULT_SSID)
        connectToWiFi(ssid.c_str(),pwd.c_str());     
     if ( reloadHost2 != "" ){ 
          //Compiler wouldn't work unless I do this hack
         reloadHost = reloadHost2;
         reloadURL = reloadURL2;
         reloadFP = reloadFP2;
         appName = appName2;
         reloadFP = reloadFP2;
         updateMainPage();
         Serial.printf("Updated AppInfo w/:%s: %s%s,%s\r\n", appName.c_str(), reloadHost.c_str(), reloadURL.c_str(), reloadFP.c_str()); 
         Serial.printf("%s%s%s\r\n",PAYLOAD_PREFIX,"{\"actionEvent\":\"newHTMLLoaded\"}",PAYLOAD_SUFFIX); 
     }

}

void postPayloadExternally(String &postPL){
     DynamicJsonBuffer  jBuffer;
     JsonObject         &jObject = jBuffer.parseObject(postPL);

     if ( connectedToWiFi == 0 ){
         Serial.println("URL cannot be posted externall since you are not  connected to WiFi");
         return;
     }
     if(!jObject.success()){
          Serial.println("unable to parse payloadJSON:");
         return;
     }
     String host        = jObject["host"];
     String url         = jObject["url"];
     int    port        = jObject["port"];
     String contentType = jObject["contentType"];
     String httpMethod  = jObject["httpMethod"];
     String secure      = jObject["secure"];
     String pl          = jObject["payload"];
     String fp          = jObject["fp"];
     String resp        = "";

     if ( httpMethod == "" )
         httpMethod = "POST";
     httpMethod.toUpperCase();
     if ( contentType == "" || contentType == "1")
         contentType = "application/json";
     else
     if ( contentType == "2" )
         contentType = "application/x-www-form-urlencoded";
      
     if ( secure.equalsIgnoreCase("yes") ){
         port = (port == 0) ?443:port;
         execHTTPS(host, url, contentType, httpMethod, fp.c_str(), port, pl, resp);
     }
     else
     if ( httpMethod == "POST" )
         httpPost(host,contentType, pl,resp);
     else
         httpGET(host,fp.c_str(),resp);
     Serial.printf("%s%s%s\r\n",PAYLOAD_PREFIX, resp.c_str(),PAYLOAD_SUFFIX); 
  
}
int httpPost(String &url, String &contentType, String &pl, String &resp){
    int        rCode = -1;
    HTTPClient http;    //Declare object of class HTTPClient
 
    if (http.begin(url)){
        http.addHeader("Content-Type", contentType.c_str());
   
        int httpCode = http.POST(pl);   //Send the request
        resp = http.getString();                                        //Get the response payload
   
        Serial.printf("httpCode:%d with response:%s",httpCode,resp.c_str());
        http.end();
   }

  if ( rCode != 0 )
      resp = "";
   return rCode;
}
//https://circuits4you.com/2019/02/08/esp8266-nodemcu-https-secured-post-request/
int execHTTPS(String &host, String &url, String &contentType, String &httpMethod, const char *fp, int httpsPort, String &pl, String &resp) {
  WiFiClientSecure httpsClient;    //Declare object of class WiFiClient

  Serial.printf("Tring:%s%s Using fingerprint '%s'\n", host.c_str(), url.c_str(), fp);
  httpsClient.setFingerprint(fp);
  httpsClient.setTimeout(15000); // 15 Seconds
  Serial.print("HTTPS Connecting");
  int r=0; //retry counter
  while((!httpsClient.connect(host, httpsPort)) && (r < 20)){
      delay(100);
      Serial.print(".");
      r++;
  }
  if(r>=20) {
    Serial.println("Connection failed");
    return -1;
  }
  else
    Serial.println("Connected to web");

  Serial.printf("Requesting host: %s  @ URL:%s\n", host.c_str(), url.c_str());
  if ( pl != "" )
     Serial.printf("Req w/ payload:%s\n",pl.c_str());
  String contentPL = "";
  if ( httpMethod == "POST" )
      contentPL = String("Content-Type:") + contentType + "\r\n" + "Content-Length: " + pl.length()+ "\r\n\r\n" + pl + "\r\n";
  httpsClient.print(httpMethod + " " + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               contentPL + 
               "Connection: close\r\n\r\n");
  Serial.printf("-->HTTPS %s Request Sent...\n",httpMethod.c_str());
  while (httpsClient.connected()) {
    String line = httpsClient.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }

  Serial.println("reply was:");
  Serial.println("==========");
  resp = ""; 
  while(httpsClient.available()){        
    resp += httpsClient.readStringUntil('\n');  //Read Line by Line
    resp += "\n";
    
  }
  Serial.println(resp);
  Serial.println("==========");
  Serial.println("closing connection");
  
  return 0;
}
int httpGET( String &url, const char *fingerP, String &resp){
    int        rCode = -1;
    HTTPClient http;  
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setFingerprint(fingerP);
    Serial.printf("[HTTP] Trying to load:%s\n",url.c_str());
    int rc = (url.indexOf("https") >=0)?http.begin(url,fingerP):http.begin(url);
    if (rc){
       Serial.print("[HTTP] GETing...\n");

       int httpCode = http.GET();                                                                  //Send the request
       if (httpCode > 0) {
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
             resp = http.getString();
             //Serial.println(payload);
             rCode = 0;
          }
        }
       else{
          Serial.printf("rc=%d, Failed to load Page",httpCode);
          rCode = httpCode;
        }
   
        http.end(); 
  }
  if ( rCode != 0 )
      resp ="";
  return rCode;
}

void handleRoot() {
  const char * page = mainPage.c_str() !=NULL && strcmp(mainPage.c_str(),"N/A") ==0 ?webpageBody:mainPage.c_str();
  server.send(200, "text/html", page);
}
void getPlayerStatus(){
   server.send(200, "application/json", mcuPayload);
}
void getAppInfo(){
   server.send(200, "application/json", appInfoPayload);
}
void toggleLED(){
   digitalWrite(pin_led,!digitalRead(pin_led));
   server.send(204,"");
}
void updateMainPage() {
   String httpMethod = "GET";
   String contentType = "text/html";
   String pl = "";
   int rCode = execHTTPS(reloadHost, reloadURL, contentType, httpMethod, reloadFP.c_str(), 443, pl, mainPage);
   if ( rCode == 0 )
       server.send(200, "application/json", "{\"status\" : \"ok\", \"msg\": \"Reloaded Main Page\"}");
   else
       server.send(200, "application/json", "{\"status\" : \"faile\", \"msg\": \"Failed to Reload Main Page\"}");
   return;
}

void actionEvent(){
   const char * actionEvent = "", *data = "";
   String payload = server.arg("plain");
   String msg = "";
   DynamicJsonBuffer jBuffer;
   JsonObject& jObject = jBuffer.parseObject(payload);
#ifdef _DEBUG
   //Serial.println("Received PayLoad:"+payload); 
#endif
   if(!jObject.success()){
      server.send(200, "application/json", "{\"status\" : \"false\", \"msg\":\"invalid JSON, unable to parse\"}");
      return;
   }
   if ( strcmp(actionEvent,"setSSID") == 0 ) {
     actionEvent = jObject["actionEvent"];
     data        = jObject["data"];
     const char * _ssid = "", *_pass = "";
     _ssid = strtok((char *)data,";");
     _pass = strtok(NULL,";");
     connectToWiFi(_ssid,_pass);
     delay(500);
     msg = "WiFi's SSID has been changed";
   }
   else
   if ( strcmp(actionEvent,"toggleLED") == 0 ) {
     msg = "LED Toggled";
#ifdef _DEBUG    
     Serial.println("\ntoggleLED on GPIO0");
#endif     
     toggleLED();
   }
   else{
     Serial.print(PAYLOAD_PREFIX);
     Serial.print(payload);
     Serial.println(PAYLOAD_SUFFIX);
     msg = "msg sent";
   }
   server.send(200, "application/json", "{\"status\" : \"ok\", \"msg\": \"" +msg + "\"}");
}
void connectToWiFi(const char *_ssid, const char *_pass){
  WiFi.softAPdisconnect(true);
  WiFi.disconnect();  
#ifdef  _DEBUG          
  Serial.println("WiFi-Disconnected"); 
#endif  
  delay(1000);

  WiFi.mode(WIFI_STA);
  if ( _pass !=NULL && strcmp(_pass,"") == 0)
     _pass =NULL;
  if ( _pass != NULL )
    WiFi.begin(_ssid, _pass);
  else
    WiFi.begin(_ssid);
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
      digitalWrite(pin_led,!digitalRead(pin_led));
      if ((unsigned long)(millis() - startTime) >= MAX_CONNECT_TIMEOUT) break;
  }
  
  if (WiFi.status() == WL_CONNECTED){
     connectedToWiFi = 1;
#ifdef  _DEBUG     
     Serial.print("WiFi connected to:"); 
     Serial.println(_ssid); 
#endif
  }
  else{
#ifdef  _DEBUG     
     Serial.print("Unable to connected to WiFi:"); 
     Serial.println(_ssid); 
     delay(2000);
#endif    
     WiFi.mode(WIFI_AP);
     WiFi.softAPConfig(local_ip, gateway, netmask);
     WiFi.softAP(mySsid, password); 
     digitalWrite(pin_led,LOW);   
     connectedToWiFi = 0;   
  }
#ifdef  _DEBUG   
 if (connectedToWiFi ){ 
    Serial.print(PAYLOAD_PREFIX);
    Serial.print("{\"actionEvent\":\"updateWiFi\"}");
    Serial.println(PAYLOAD_SUFFIX);
    Serial.print("\nIP address: ");
    Serial.println(WiFi.localIP()); 
 }
#endif
}
