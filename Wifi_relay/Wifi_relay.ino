#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <SpecialBool.h>
#include <LittleFS.h>

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

byte last_website_served = 0;
bool website_recently_served = false;
bool website_ready = false;
bool change_made = false;
Timer website_timeout(2000, 1);
Timer update_the_time(60000, 1);
bool update_the_time_in;

struct Logic_based_output{
  byte output_pin;
  SpecialBool state;
  byte timed_out_type;
  byte hour_on;
  byte hour_off;
  byte counter;
  unsigned int change_made:1;
  unsigned int automatic_state:1;
  unsigned int manual_mode:1;
  unsigned int reserve3:1;
  unsigned int reserve4:1;
  unsigned int reserve5:1;
  unsigned int reserve6:1;
  unsigned int reserve7:1;
};

const int NUM_OF_RELAYS = 8;
const byte TIME_OF_DAY_BASED = 1;
const byte CONTINUOUSLY_TIME_BASED = 2;
const byte NOT_TIME_BASED = 3;
int outputs[] = {2,4,5,12,13,14,15,16};
int outputs_type[] = {1,2,3,1,1,3,3,2};
Logic_based_output Relay1;
Logic_based_output Relay2;
Logic_based_output Relay3;
Logic_based_output Relay4;
Logic_based_output Relay5;
Logic_based_output Relay6;
Logic_based_output Relay7;
Logic_based_output Relay8;

Logic_based_output *AofRelays[] = {&Relay1, &Relay2, &Relay3, &Relay4, &Relay5, &Relay6, &Relay7, &Relay8};

#include "Pages.h"
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.begin("FBI", "pck=#3eyagq3");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.print(".");
  }
  Serial.println(" connected");
  Serial.print("IP address = ");
  Serial.println(WiFi.localIP());

  server.on("/", show_page_home);
  server.on("/manual", show_page_manual);
  server.on("/timing", show_page_timing);
  server.begin();

 
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  timeClient.begin();
  timeClient.setTimeOffset(-14400);
  timeClient.setUpdateInterval(3600000);

  //LittleFS.format();
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }
  
	for(int i = 0; i < NUM_OF_RELAYS; i++){
		AofRelays[i]->output_pin = outputs[i];
		char file_location[12];
    char relay_char[] = {(char(i) + '0'), 0};
    strcpy(file_location,"/relay");
    strcat(file_location, relay_char);
    strcat(file_location, ".txt");
    File f = LittleFS.open(file_location, "r");
		if (!f) {
  		Serial.println("file open failed");
  		AofRelays[i]->timed_out_type = 1;
  		AofRelays[i]->hour_on = 0;
  		AofRelays[i]->hour_off = 0;
		}
		else{
      Logic_based_output temp_relay;
			f.read((byte *)AofRelays[i], sizeof(temp_relay));
      
		}
		f.close();
	}
  
	for(int i = 0; i < NUM_OF_RELAYS; i++){
		pinMode(AofRelays[i]->output_pin, OUTPUT);
		digitalWrite(AofRelays[i]->output_pin, LOW);
		AofRelays[i]->state.write(true);
	}
  
  delay(500);
	for(int i = 0; i < NUM_OF_RELAYS; i++){
		digitalWrite(AofRelays[i]->output_pin, HIGH);
		AofRelays[i]->state.write(false);
		AofRelays[i]->change_made = true;
		delay(200);
	}
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  webSocket.loop();

  // update time every minute
  if (update_the_time.Q() or not timeClient.isTimeSet()){
    // This should cycle on the first cylcle and every minute after
    update_the_time_in = false;
    timeClient.update();
    Serial.print("The day is: ");
    Serial.print(timeClient.getDay());
    Serial.print(" and the time is: ");
    Serial.println(timeClient.getFormattedTime());
    if ((timeClient.getSeconds() > 10) and (timeClient.getSeconds() < 50)){
      update_the_time.time = (60000 + ((60 - timeClient.getSeconds()) * 1000));
    }
    else{
      update_the_time.time = 60000;
    }
	  update_time_of_day_states();
  }
  else{
    update_the_time_in = true;
  }
  update_the_time.input(update_the_time_in);
  
  for(int i = 0; i < NUM_OF_RELAYS; i++){
    if (AofRelays[i]->change_made){
      digitalWrite(AofRelays[i]->output_pin, (AofRelays[i]->state.actualNot()));
      change_made = true;
      AofRelays[i]->change_made = false;
    }
  }
  if (change_made and (last_website_served == 1)){
    send_man_update();
    change_made = false;
  }else if (change_made){
    write_relay_data();
    change_made = false;
  }
  if (website_recently_served and website_ready){
    Serial.println("sending update");
    switch (last_website_served){
      case 1:
        send_man_update();
      break;
      case 2:
        send_time_update();
      break;
    }
    website_recently_served = false;
    website_ready = false;
  }
  if (website_timeout.input((website_recently_served xor website_ready))){
    Serial.println("something failed");
    website_recently_served = false;
    website_ready = false;
  }
}
