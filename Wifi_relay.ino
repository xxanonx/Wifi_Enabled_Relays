#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <SpecialBool.h>

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
byte last_website_served = 0;
bool website_recently_served = false;
bool website_ready = false;
bool change_made = false;
Timer website_timeout(2000, 1);

struct Logic_based_output{
  byte output_pin;
  SpecialBool state;
  byte timed_out_type;
  byte hour_on;
  byte hour_off;
  unsigned int change_made:1;
  unsigned int automatic_state:1;
  unsigned int reserve2:1;
  unsigned int reserve3:1;
  unsigned int reserve4:1;
  unsigned int reserve5:1;
  unsigned int reserve6:1;
  unsigned int reserve7:1;
  
};
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
  
  for(int i = 0; i < 8; i++){
    AofRelays[i]->output_pin = outputs[i];
    AofRelays[i]->timed_out_type = 1;
    AofRelays[i]->hour_on = 0;
    AofRelays[i]->hour_off = 0;
  }
  for(int i = 0; i < 8; i++){
    pinMode(AofRelays[i]->output_pin, OUTPUT);
    digitalWrite(AofRelays[i]->output_pin, LOW);
    AofRelays[i]->state.write(true);
  }
  
  delay(500);
  for(int i = 0; i < 8; i++){
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
  for(int i = 0; i < 8; i++){
    if (AofRelays[i]->change_made){ // f is the manual bit
      digitalWrite(AofRelays[i]->output_pin, (AofRelays[i]->state.actualNot()));
      change_made = true;
      AofRelays[i]->change_made = false;
    }
  }
  if (change_made and (last_website_served == 1)){
    send_man_update();
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
