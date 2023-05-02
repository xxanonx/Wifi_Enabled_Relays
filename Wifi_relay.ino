#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <SpecialBool.h>

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
/*struct Time_based_output{
  byte output_pin = 2;
  bool change_made;
  SpecialBool state;
  int time_on;
  int time_off;
};*/
struct Logic_based_output{
  byte output_pin;
  bool change_made;
  SpecialBool state;
};
int outputs[] = {2,4,5,12,13,14,15,16};
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
  server.begin();
  
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
  for(int i = 0; i < 8; i++){
    AofRelays[i]->output_pin = outputs[i];
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
      send_man_update();
      AofRelays[i]->change_made = false;
    }
  }
}
