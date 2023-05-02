const char page_home[] PROGMEM = R"=====(
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <title>WIFI controlled relays</title>
  </head>
  <body>
    <style>body{ background-color: #945454; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }</style>
    <a href="/"><button>HOME</button></a>
    <h1>WIFI Relays Home Page</h1>
    <p>Time based Relay activation<a href="/timing"><button>GO</button></a></p>
    <p>Sensor or logic based Relay activation<a href="/logic"><button>GO</button></a></p>
    <p>Sensor data<a href="/sensor_data"><button>GO</button></a></p>
    <p>Manual Mode<a href="/manual"><button>GO</button></a></p>
  </body>
</html>
)=====";

const char page_manual[] PROGMEM = R"=====(
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <title>Wifi relay manual functions</title>
    <script>
    var Socket;
    function init() {
      Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
      Socket.onmessage = function(event){
        console.log("Socket Event!");
        var message = event.data;
        console.log(message);
        var includesOn = (message.indexOf("ON") != -1);
        var includesOff = (message.indexOf("OFF") != -1);
        var includesAuto = (message.indexOf("AUTO") != -1);
        var includesMan = (message.indexOf("MAN") != -1);
        var newText;
        
        if (includesOn){
          console.log("includesOn");
          newText = "On";
        }else if (includesOff){
          console.log("includesOff");
          newText = "Off";
        }
        var Relay_state;
        for (let i = 1; i <= 8; i++) {
          if (message.indexOf(i) != -1){
            Relay_state = i;
            break;
          }
        }
        if (message.indexOf("RS") != -1){
          var man = 'RElAY_' + Relay_state + '_man';
          var state = 'RElAY_' + Relay_state + '_state';
          document.getElementById(state).innerHTML = newText;
          if (includesAuto){
            document.getElementById(man).innerHTML = "Auto";
          }else if (includesMan){
            document.getElementById(man).innerHTML = "Man";
          }
        }
      }
    }
    function setAutoMode(relay){
      console.log("Sending Manual Data!");
      var man = 'RElAY_' + relay + '_man';
      if (document.querySelector('#' + man).innerHTML == "Auto"){
        document.getElementById(man).innerHTML = "Man";
        sendManData(relay);
      }
      else if (document.querySelector('#' + man).innerHTML == "Man"){
        document.getElementById(man).innerHTML = "Auto";
        console.log("Switching to Automatic!");
        var message = "Auto ";
        message += "RT ";
        message += relay;
        message += ".";
        console.log(message);
        Socket.send(message);
      }
    }  
    function sendManData(relay){
      var man = 'RElAY_' + relay + '_man';
      var state = 'RElAY_' + relay + '_state';
      var newText;
      if (document.querySelector('#' + state).innerHTML == "Off"){
        newText = "On";
      }
      else if (document.querySelector('#' + state).innerHTML == "On"){
        newText = "Off";
      }
      if (document.querySelector('#' + man).innerHTML == "Man"){
        console.log("Sending Manual Data!");
        var message = "Man ";
        message += "RT ";
        message += relay;
        message += " "
        message += newText;
        message += ".";
        console.log(message);
        Socket.send(message);
      }else{
        console.log("Relay " + relay + "is not in manual");
      }
    }
    </script>
  </head>
  <body onload="javascript:init()">
    <style>body{ background-color: #537846; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }</style>
    <a href="/"><button>HOME</button></a>
    <h1>Manual Page</h1>
  <br>Manual Relay States:
  <br>Relay 1: &emsp;<button id="RElAY_1_man" onclick="setAutoMode('1')">Auto</button> &emsp;<button id="RElAY_1_state" onclick="sendManData('1')">Off</button>
  <br>Relay 2: &emsp;<button id="RElAY_2_man" onclick="setAutoMode('2')">Auto</button> &emsp;<button id="RElAY_2_state" onclick="sendManData('2')">Off</button>
  <br>Relay 3: &emsp;<button id="RElAY_3_man" onclick="setAutoMode('3')">Auto</button> &emsp;<button id="RElAY_3_state" onclick="sendManData('3')">Off</button>
  <br>Relay 4: &emsp;<button id="RElAY_4_man" onclick="setAutoMode('4')">Auto</button> &emsp;<button id="RElAY_4_state" onclick="sendManData('4')">Off</button>
  <br>Relay 5: &emsp;<button id="RElAY_5_man" onclick="setAutoMode('5')">Auto</button> &emsp;<button id="RElAY_5_state" onclick="sendManData('5')">Off</button>
  <br>Relay 6: &emsp;<button id="RElAY_6_man" onclick="setAutoMode('6')">Auto</button> &emsp;<button id="RElAY_6_state" onclick="sendManData('6')">Off</button>
  <br>Relay 7: &emsp;<button id="RElAY_7_man" onclick="setAutoMode('7')">Auto</button> &emsp;<button id="RElAY_7_state" onclick="sendManData('7')">Off</button>
  <br>Relay 8: &emsp;<button id="RElAY_8_man" onclick="setAutoMode('8')">Auto</button> &emsp;<button id="RElAY_8_state" onclick="sendManData('8')">Off</button>
  <br><br><br>
  </div>
  </body>
</html>
)=====";

void send_man_update(){
  Serial.print("\n");
  for(int i = 0; i < 8; i++){
      char c[15] = "RS       ";
      byte mes_end = 3;
      c[mes_end] = String(i + 1)[0];
      mes_end = 6;
      c[5] = 'O';
      if (AofRelays[i]->state.actual()){
        c[mes_end] = 'N';
      }
      else{
        c[mes_end] = 'F';
        mes_end += 1;
        c[mes_end] = 'F';
      }
      mes_end += 2;
      if (AofRelays[i]->state.special.f){
        c[mes_end] = 'M';
        mes_end += 1;
        c[mes_end] = 'A';
        mes_end += 1;
        c[mes_end] = 'N';
      }
      else{
        c[mes_end] = 'A';
        mes_end += 1;
        c[mes_end] = 'U';
        mes_end += 1;
        c[mes_end] = 'T';
        mes_end += 1;
        c[mes_end] = 'O';
      }
      
      Serial.println(c);
      webSocket.broadcastTXT(c, sizeof(c));
  }
}

void show_page_home(){
  server.send(200, "text/html", page_home);
}

void show_page_manual(){
  server.send(200, "text/html", page_manual);
  delay(300);
  send_man_update();
}



void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t leng){
  bool man;
  bool manual_state;
  char p[leng];
  for(int i = 0; i < leng; i++){
    Serial.print((char) payload[i]);
    p[i] = ((char) payload[i]);
  }
  Serial.println(p);
  String load = String(p);
  char relay_num_char;
  int relay_number = 0;
  
  if (load.substring(0,6) == "Man RT"){
    Serial.print("Manual ");
    man = true;
    relay_num_char = load.substring(7, 8)[0];
    if (load.substring(9,11) == "On"){
      manual_state = true;
      Serial.print("On ");
    }
    else if (load.substring(9,12) == "Off"){
      manual_state = false;
      Serial.print("Off ");
    }
  }
  else if (load.substring(0,7) == "Auto RT"){
    man = false;
    relay_num_char = load.substring(8, 9)[0];
  }
  Serial.println(relay_num_char);
  if (isDigit(relay_num_char)){
    relay_number = relay_num_char - '0';
  }
  Serial.println(relay_number);
  
  for(int i = 0; i < 8; i++){
    if (relay_number == (i + 1))
      if (man){
        AofRelays[i]->state.special.f = true;
        AofRelays[i]->state.write(manual_state);
        AofRelays[i]->change_made = true;
      }
      else{
        AofRelays[i]->state.special.f = false;
        AofRelays[i]->state.write(AofRelays[i]->state.special.g);  //Automatic state
        AofRelays[i]->change_made = true;
      }
      
  }
}
