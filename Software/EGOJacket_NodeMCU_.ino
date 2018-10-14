// Libraries
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <TwitterApi.h>
#include <LiquidCrystal.h>
#include <Adafruit_NeoPixel.h>

// Token and number of pixels
#define BEARER_TOKEN "AAAAAAAAAAAAAAAAAAAAAEqh8gAAAAAA%2FHJg%2FRcRiTxn5AV1m%2FVAATQrsR0%3Dnc6BilKLirGbZL6KBANsr1JMiw8hO1PlnwQ0aPHzJ5vDDkpdkG"
#define NUMPIXELS 15

// WiFi and Twitter settings
WiFiClientSecure client;
TwitterApi api(client);

unsigned long api_mtbs = 60000; //mean time between api requests
unsigned long api_lasttime = 0;   //last time api request has been done
bool firstTime = true;
bool responseSuccess = false;

// LCD pins
// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(D10, D9, D4, D3, D1, D0);

//Neopixels and colors
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, D5, NEO_GRB + NEO_KHZ800);
int c1[] = {255, 51, 51};
int c2[] = {0, 204, 102};

// Network

// Twitter inputs and variables
String usernames[] = {"EPN", "BillGates", "lopezobrador_", "realDonaldTrump", "belindapop", "elonmusk", "yuyacast", "Snowden", "pewdiepie", "KimKardashian", "CeciliaSuarezOF", "NeilHarbisson", "taylorswift13"};
bool haveBearerToken = false;
String t_data;
int followers_count, statuses_count;
String currentUsr;
int rndUsr;

void setup() {

  // Set D2 output, pixels and lcd setup
  pinMode(D2, OUTPUT);
  pixels.begin();
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, 0, 0, 0);
  }
  pixels.show();
  lcd.begin(16, 2);

  // Connect to predefined network
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  delay(100);

  lcd.print("Connecting to:");
  lcd.setCursor(0, 1);
  lcd.print("ccemx-aulas");

  delay(2000);

  lcd.clear();
  WiFi.begin("ccemx-aulas", NULL);
  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    lcd.print(".");
  }
  lcd.clear();
  lcd.print("Connected!");

  delay(2500);

  lcd.clear();
  lcd.print("IP address: ");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  delay(2500);

  api.setBearerToken(BEARER_TOKEN);
  haveBearerToken = true;

  // Get the first Twitter's user data
  if (haveBearerToken) {
    rndUsr = random(sizeof(usernames) / sizeof(String));
    currentUsr = usernames[rndUsr];
    getTwitterStats(currentUsr);
    api_lasttime = millis();
    lcd.clear();
    lcd.print("Twitter user:");
    lcd.setCursor(0, 1);
    lcd.print("@" + String(currentUsr));
    delay(2500);
  }

}

void getTwitterStats(String name) {

  // Ask specfic user statistics to API

  String responseString = api.getUserStatistics(name);
  DynamicJsonBuffer jsonBuffer;
  JsonObject& response = jsonBuffer.parseObject(responseString);

  // Get response
  responseSuccess = response.success();

  if (responseSuccess) {
    // Get followers and tweets count
    followers_count = response["followers_count"];
    statuses_count = response["statuses_count"];
    t_data = "Followers: " + String(followers_count) + " Tweets: " + String(statuses_count) + " ";
  } else {
    // If not reachable
    t_data = "Not available. ";
  }

}

void loop() {

  // Get the next Twitter's user data after (api_mtbs)ms
  if (haveBearerToken) {
    if ((millis() > api_lasttime + api_mtbs))  {
      rndUsr = random(sizeof(usernames) / sizeof(String));
      currentUsr = usernames[rndUsr];
      getTwitterStats(currentUsr);
      api_lasttime = millis();
      lcd.clear();
      lcd.print("Twitter user:");
      lcd.setCursor(0, 1);
      lcd.print("@" + String(currentUsr));

      delay(2500);
    }
  }

  // Display and scroll data.
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("@" + String(currentUsr));
  lcd.setCursor(0, 1);
  lcd.print(t_data);
  t_data += t_data.charAt(0);
  t_data.remove(0, 1);

  //Set pixels colors (semi-random)
  for (int i = 0; i < NUMPIXELS; i++) {
    if (random(100) > 50) {
      pixels.setPixelColor(i, pixels.Color(c1[0], c1[1], c1[2]));
    } else {
      pixels.setPixelColor(i, pixels.Color(c2[0], c2[1], c2[2]));
    }
  }
  pixels.show();

  analogWrite(D2, 1023);
  //delay(500);
  delay(3000);
  analogWrite(D2, 0);
  delay(3000);
}
