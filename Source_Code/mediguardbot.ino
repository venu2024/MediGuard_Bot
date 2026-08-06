#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SimpleTimer.h>
SimpleTimer scantimer;
SimpleTimer timer;
SimpleTimer timecount;
SimpleTimer matchtimer;
SimpleTimer buttonscan;

const char *ssid = "project";  // replace with your wifi ssid and wpa2 key
const char *password = "project1234";

const char *host = "http://microembeddedtech.com/appinventor";
String get_host = "http://microembeddedtech.com/appinventor";
  
WiFiServer server(80);  // open port 80 for server connection
String tablename = "medibot";
WiFiClient client;

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display

int dbupdate=0;

int box1m1=2;
int box1m2=4;
int box2m1=15;
int box2m2=22;

#define b1s1voicepin 26
#define b1s2voicepin 27
int MA1 = 21;
int MA2 = 19;
int MB1 = 18;
int MB2 = 5;

int buzzerPin=23;

const char* d1s1time = "d1s1time";
const char* d1s2time = "d1s2time";
const char* d1s3time = "d1s3time";
const char* d2s1time = "d2s1time";
const char* d2s2time = "d2s2time";
const char* d2s3time = "d2s3time";

const char* robocmd = "robocmd";
const char* modebit = "modebit";
const char* d1manbit = "d1manbit";
const char* d2manbit = "d2manbit";

String k1, k2, k3, k4, k5, k6, k7, k8, k9, k10;

String currentime;
String chour;
String cmin;

int p1bhour=0;int p1bmin=0;
int p2bhour=0;int p2bmin=0;
int p3bhour=0;int p3bmin=0;

int currenthour=1;int currentmin=1;

String b1s1time;
String b1s2time;
String b1s3time;
String b2s1time;
String b2s2time;
String b2s3time;

String b1s1hour;String b1s1min;
String b1s2hour;String b1s2min;
String b1s3hour;String b1s3min;

int s1b1hour=0;int s1b1bmin=0;
int s2b1hour=0;int s2b1bmin=0;
int s3b1hour=0;int s3b1bmin=0;

String b2s1hour;String b2s1min;
String b2s2hour;String b2s2min;
String b2s3hour;String b2s3min;

int s1b2hour=0;int s1b2bmin=0;
int s2b2hour=0;int s2b2bmin=0;
int s3b2hour=0;int s3b2bmin=0;

int b1match = 0, b2match = 0, b3match = 0;  // MATCHING LOGIC 

int robobit=5;
int modeval=0;
int box1manual=0;
int box2manual=0;
int b1manstatus=0;
int b2manstatus=0;

int firepin=17;
String firestatus="NO";
int fireval;

int gaspin=16;
String gasstatus="NORMAL";
int gasval;

void setup() {
  Serial.begin(9600);
  robopins_setup();
  initmotors();
  voicepinsetup();
  pinMode(buzzerPin, OUTPUT); 
  pinMode(firepin,INPUT);
  pinMode(gaspin,INPUT);
  digitalWrite(buzzerPin, 0);
  timer.setInterval(2000, timematchfunction);
  scantimer.setInterval(2000, scandata);
  lcd.begin(); 
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("IoT MEDIGUARD");
  lcd.setCursor(0,1);
  lcd.print("  BOT SYSTEM");
  robopins_setup();
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(2000);
    Serial.print(".");
  }
  //Serial.println("");
  //Serial.println("WiFi connected.");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
  //testboxes();
  //testvoices();
  //testrobo();
  stoprobo();
  getdata();
}

void loop() {

  timer.run();
  scantimer.run();
  getdata();
  delay(2000);

}

void scandata(){
  fireval=digitalRead(firepin);
  //Serial.print("FIRE VAL");Serial.println(fireval);
  if(fireval==1){
    firestatus="NO";
  }
  else{
    firestatus="YES";
    buzzering();
  }
  gasval=digitalRead(gaspin);
  //Serial.print("GAS VAL: ");Serial.println(gasval);
  if(gasval==1){
    gasstatus="NORMAL";
  }
  else{
    gasstatus="DETECTED";
    buzzering();
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("F:");
  lcd.print(firestatus);
  lcd.print("  ");
  lcd.print("G:");
  lcd.print(gasstatus);
  lcd.setCursor(0, 1);
  lcd.print("   ");
  lcd.print(currenthour); lcd.print(":"); lcd.print(currentmin); lcd.print("   ");
  userupdate_status(tablename,firestatus,gasstatus);
}
void buzzering(){
  digitalWrite(buzzerPin, 1);
  delay(1000);
  digitalWrite(buzzerPin, 0);
}

void getdata(){
  get_device_status(tablename);
  delay(1000);
  gettime();
  delay(1000);
}

void userupdate_status(String table_name,String ufire,String ugas){
  WiFiClient client = server.available();
  HTTPClient http;
  String Ss1 = '"' + ufire + '"';
  String Ss2 = '"' + ugas + '"';
  String url = get_host+"/gumedibotupdate.php?table_name="+table_name+"&fire="+Ss1+"&gas="+Ss2;
  //Serial.println(url);
  http.begin(client,url);
  //GET method
  int httpCode = http.GET();
  String payload = http.getString();
  payload.trim();
  Serial.println(payload);
  http.end();
  delay(1000);
}

//FUNCTIONS
void initmotors(){
  pinMode(box1m1,OUTPUT);
  pinMode(box1m2,OUTPUT);
  pinMode(box2m1,OUTPUT);
  pinMode(box2m2,OUTPUT);
}
void box1open(){
  Serial.println("-------- BOX 1 OPEN ----------");
  digitalWrite(box1m1,1);
  digitalWrite(box1m2,0);
  delay(1500);
  digitalWrite(box1m1,0);
  digitalWrite(box1m2,0);
  b1s1voice();
}
void box1close(){
  Serial.println("-------- BOX 1 CLOSE ---------"); 
  digitalWrite(box1m1,0);
  digitalWrite(box1m2,1);
  delay(1500);
  digitalWrite(box1m1,0);
  digitalWrite(box1m2,0);
}
void box2open(){
  Serial.println("-------- BOX 2 OPEN --------");
  digitalWrite(box2m1,1);
  digitalWrite(box2m2,0);
  delay(1500);
  digitalWrite(box2m1,0);
  digitalWrite(box2m2,0);
  b1s2voice();
}
void box2close(){
  Serial.println("-------- BOX 2 CLOSE ----------"); 
  digitalWrite(box2m1,0);
  digitalWrite(box2m2,1);
  delay(1500);
  digitalWrite(box2m1,0);
  digitalWrite(box2m2,0);
}
void testboxes(){
  Serial.println("TESGING BOXES");
  box1open();
  delay(2000);
  box1close();
  delay(2000); 
  box2open();
  delay(2000);
  box2close();
  delay(2000);
}
void allboxesclose(){
  box1close();
  delay(1000); 
  box2close();
  delay(1000); 
}

void gettime(){
  WiFiClient client = server.available();
  HTTPClient http;
  String url = get_host+"/gettime.php";
  //Serial.println(url);
  http.begin(client,url);
  //GET method
  int httpCode = http.GET();
  String payload = http.getString();
  //Serial.print("PAYLOD TIME:");Serial.println(payload);
  currentime=payload;
  //Serial.print("CURRENT TIME:");Serial.println(currentime);
  chour = getValue(currentime, ':', 0);
  cmin = getValue(currentime, ':', 1);
  currenthour = chour.toInt(); currentmin = cmin.toInt();
}

String getValue(String data, char separator, int index){
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++){
    if (data.charAt(i) == separator || i == maxIndex){
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void get_device_status(String table_name){
  if (WiFi.status() != WL_CONNECTED){
    Serial.println("WiFi not connected! Retrying...");
    WiFi.begin(ssid, password);
    delay(3000);
    return;
  }
  HTTPClient http;  
  // Encode `task_key` to avoid issues with commas
  //String encodedTaskKey = "command%2Cgetdata%2Cservobit"; 
  // String encodedTaskKey = String(keyone) + "%2C" + String(keytwo) + "%2C" + String(keythree);
  String encodedTaskKey = String(d1s1time) + "%2C" + String(d1s2time) + "%2C" + String(d1s3time) + "%2C" +
                          String(d2s1time) + "%2C" + String(d2s2time) + "%2C" + String(d2s3time) + "%2C" +
                          String(robocmd) + "%2C" + String(modebit) + "%2C" + String(d1manbit) + "%2C" + String(d2manbit);
  //Serial.print(encodedTaskKey);
  String url = get_host + "/vitsmarthomegetmultiple.php?table_name=" + table_name + "&task_key=" + encodedTaskKey;
  //Serial.println("Requesting: " + url);  
  http.begin(url);
  http.setTimeout(10000); // 10 seconds timeout
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Connection", "close");
  int httpCode = http.GET();
  if (httpCode > 0){
    //Serial.print("HTTP Response Code: ");
    //Serial.println(httpCode);
    String payload = http.getString();
    //Serial.println("Raw Response: " + payload);
    // Extract JSON part if any unwanted data is present
    int jsonStart = payload.indexOf('[');
    if (jsonStart != -1){
      payload = payload.substring(jsonStart);
    }
    // Parse JSON response
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);
    if (error){
      Serial.print("JSON Parsing failed: ");
      Serial.println(error.f_str());
      return;
    }
    // Extract values
    for (JsonObject obj : doc.as<JsonArray>()){
      k1 = obj[d1s1time].as<String>();
      k2 = obj[d1s2time].as<String>();
      k3 = obj[d1s3time].as<String>();
      k4 = obj[d2s1time].as<String>();
      k5 = obj[d2s2time].as<String>();
      k6 = obj[d2s3time].as<String>();
      k7 = obj[robocmd].as<String>();
      k8 = obj[modebit].as<String>();
      k9 = obj[d1manbit].as<String>();
      k10 = obj[d2manbit].as<String>();
      //Serial.print("K9: "); Serial.println(k9);
      // Serial.print("K2: "); Serial.println(k2);
      // Serial.print("K3: "); Serial.println(k3);
    }
  } 
  else{
    Serial.print("HTTP Request failed, error: ");
    Serial.println(http.errorToString(httpCode).c_str());
  }
  http.end();
  checkstatus();
}

void checkstatus(){
  b1s1time=k1;
  b1s2time=k2;
  b1s3time=k3;
  b2s1time=k4;
  b2s2time=k5;
  b2s3time=k6;
  robobit=k7.toInt();
  modeval=k8.toInt();
  box1manual=k9.toInt();
  box2manual=k10.toInt();

  ///////////  robo commands ////////
  if(robobit==1){
    forwardrobo();
  }
  else if(robobit==2){
    backwardrobo() ;
  }
  else if(robobit==3){
    rightrobo();
  }
  else if(robobit==4){
    leftrobo();
  }
  else if(robobit==5){
    stoprobo();
  }
  
  if(modeval==1){
    if(box1manual==1 && b1manstatus==0){
      Serial.println("@@ BOX 1 MANUAL OPEN @@");
      box1open();
      b1manstatus=1;
    }
    else  if(box1manual==0 && b1manstatus==1){
      Serial.println("@@ BOX 1 MANUAL CLOSE @@");
      box1close();
      b1manstatus=0;
    }
    
    if(box2manual==1 && b2manstatus==0){
      Serial.println("@@ BOX 2 MANUAL OPEN @@");
      box2open();
      b2manstatus=1;
    }
    else  if(box2manual==0 && b2manstatus==1){
      Serial.println("@@ BOX 2 MANUAL CLOSE @@");
      box2close();
      b2manstatus=0;
    }
  }  // mode bit end
}

void timematchfunction(){
  if(modeval==0){
    struct Slot{
      int hour, min;
      int &match;
      void (*open)();
      void (*close)();
      const char* label;
    } slots[] = {
        {b1s1time.substring(0, 2).toInt(), b1s1time.substring(3, 5).toInt(), b1match, box1open, box1close, "B1S1"},
        {b1s2time.substring(0, 2).toInt(), b1s2time.substring(3, 5).toInt(), b1match, box1open, box1close, "B1S2"},
        {b1s3time.substring(0, 2).toInt(), b1s3time.substring(3, 5).toInt(), b1match, box1open, box1close, "B1S3"},
        {b2s1time.substring(0, 2).toInt(), b2s1time.substring(3, 5).toInt(), b2match, box2open, box2close, "B2S1"},
        {b2s2time.substring(0, 2).toInt(), b2s2time.substring(3, 5).toInt(), b2match, box2open, box2close, "B2S2"},
        {b2s3time.substring(0, 2).toInt(), b2s3time.substring(3, 5).toInt(), b2match, box2open, box2close, "B2S3"}
    };
    for (auto &slot : slots){
      //Serial.print(b1s1time.substring(0, 2).toInt());Serial.println(b1s1time.substring(3, 5).toInt());
      if (currenthour == slot.hour && currentmin == slot.min && slot.match == 0){
        Serial.print("SLOT OPEN: ");
        Serial.println(slot.label);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("SLOT OPEN: ");
        lcd.print(slot.label);
        slot.open();
        delay(1000);
        slot.match = 1;
      }
      if (currenthour == slot.hour && currentmin == slot.min + 1 && slot.match == 1){
        slot.close();
        slot.match = 0;
        Serial.print("SLOT CLOSED: ");
        Serial.println(slot.label);
      }
    }

  }  // MODE BIT VALUE IF 0 i.e AUTO MODE
}

void robopins_setup(){
  pinMode(MA1, OUTPUT);
  pinMode(MA2, OUTPUT);
  pinMode(MB1, OUTPUT);
  pinMode(MB2, OUTPUT);
}
void testrobo(){
  forwardrobo();
  delay(2000);
  stoprobo();
  delay(2000);
  backwardrobo() ;
  delay(2000);
  stoprobo();
  delay(2000);
  rightrobo();
  delay(2000);
  stoprobo();
  delay(2000);
  leftrobo();
  delay(2000);  
  stoprobo();
  //delay(2000);
}
void forwardrobo(){
  digitalWrite(MA1, 0);
  digitalWrite(MA2, 1);
  digitalWrite(MB1, 0);
  digitalWrite(MB2, 1);
}
void backwardrobo(){
  digitalWrite(MA1, 1);
  digitalWrite(MA2, 0);
  digitalWrite(MB1, 1);
  digitalWrite(MB2, 0);
}
void stoprobo(){
  digitalWrite(MA1, 0);
  digitalWrite(MA2, 0);
  digitalWrite(MB1, 0);
  digitalWrite(MB2, 0);
}
void rightrobo(){
  digitalWrite(MA1, 1);
  digitalWrite(MA2, 0);
  digitalWrite(MB1, 0);
  digitalWrite(MB2, 1);
}
void leftrobo(){
  digitalWrite(MA1, 0);
  digitalWrite(MA2, 1);
  digitalWrite(MB1, 1);
  digitalWrite(MB2, 0);
}

void voicepinsetup(){
  pinMode(b1s1voicepin,OUTPUT);
  pinMode(b1s2voicepin,OUTPUT);
  digitalWrite(b1s1voicepin, 1);
  digitalWrite(b1s2voicepin, 1);
}
void b1s1voice(){
  Serial.println("^^^ B1S1 VOICE PLAY ^^^");
  digitalWrite(b1s1voicepin, 0);
  delay(300);
  digitalWrite(b1s1voicepin, 1);
}
void b1s2voice(){
  Serial.println("^^^ B1S2 VOICE PLAY ^^^");
  digitalWrite(b1s2voicepin, 0);
  delay(300);
  digitalWrite(b1s2voicepin, 1);
}
void testvoices(){
  b1s1voice();
  delay(2000);
  b1s2voice();
  delay(2000);
}
