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
      Socket.onopen = function(){
        console.log("Ready");
        Socket.send("Ready");
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
  <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.6.4/jquery.min.js"></script>
  <script>
    
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

const char page_timing[] PROGMEM = R"=====(
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <title>Wifi relay timed functions</title>
  <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.6.4/jquery.min.js"></script>
    <script>
    var Socket;
    function init() {
      Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
      Socket.onmessage = function(event){
        console.log("Socket Event!");
        var message = event.data;
        console.log(message);
        var includesTODbased = (message.indexOf("TOD") != -1);
        var includesCTbased = (message.indexOf("CT") != -1);
        var includesNTbased = (message.indexOf("NT") != -1);
        
        var includesOnLoc = message.indexOf("ON");
        var includesOffLoc = message.indexOf("OF");
        var endmes = message.indexOf("\u0000");
        var on_time = message.substring(includesOnLoc + 2, includesOffLoc).trim();
        var off_time = message.substring(includesOffLoc + 2, endmes).trim();
        console.log(on_time, off_time);
        var newValue;
        
        if (includesTODbased){
          console.log("includesTODbased");
          newValue = 1;
        }else if (includesCTbased){
          console.log("includesCTbased");
          newValue = 2;
        }else if (includesNTbased){
          console.log("includesNTbased");
          newValue = 3;
        }
        var Relay_state;
        for (let i = 1; i <= 8; i++) {
          if (message.substring(0, 6).indexOf(i) != -1){
            Relay_state = i;
            break;
          }
        }
        if (message.indexOf("RTS") != -1){
          var state = 'Relay_select' + Relay_state;
          var on = 'Hour_on' + Relay_state;
          var off = 'Hour_off' + Relay_state;
          document.getElementById(state).value = newValue;
          document.getElementById(on).value = on_time;
          document.getElementById(off).value = off_time;
        }
      }
      Socket.onopen = function(){
        console.log("Ready");
        Socket.send("Ready");
      }
    }
    function sendTimeMode(){
      limitTimes();
      console.log("Sending Timing Data!");
      
      for (let i = 1; i <= 8; i++) {
        var relay = 'Relay_select' + i;
        var relay_setting = document.querySelector('#' + relay).value;
        var Hour_on = document.querySelector('#Hour_on' + i).value;
        var Hour_off = document.querySelector('#Hour_off' + i).value;

        var message = "RTime ";
        message += i;
        message += " M ";
        message += relay_setting;
        message += " ON ";
        message += Hour_on;
        message += " OF ";
        message += Hour_off;
        message += ".";
        console.log(message);
        Socket.send(message);
                   
      }
    }  
    function limitTimes(){
      console.log("Limiting Timing Data!");
      
      for (let i = 1; i <= 8; i++) {
        var state = 'Relay_select' + i;
        var on = 'Hour_on' + i;
        var off = 'Hour_off' + i;
        var relay_setting = document.querySelector('#' + state).value;
        var Hour_on = document.querySelector('#' + on).valueAsNumber;
        var Hour_off = document.querySelector('#' + off).valueAsNumber;
        
        console.log(relay_setting, Hour_on, Hour_off);

        switch (parseInt(relay_setting)){
          case 1:
            console.log("time based");
            if ((Hour_on < 0) || (Hour_on > 23)){
              document.getElementById(on).value = Math.min(Math.max(Hour_on, 0), 23)
            }
            if ((Hour_off < 0) || (Hour_off > 23)){
              document.getElementById(off).value = Math.min(Math.max(Hour_off, 0), 23)
            }
          break;
          default:
            console.log("continuous based");
            if ((Hour_on < 0) || (Hour_on > 254)){
              document.getElementById(on).value = Math.min(Math.max(Hour_on, 0), 254)
            }
            if ((Hour_off < 0) || (Hour_off > 254)){
              document.getElementById(off).value = Math.min(Math.max(Hour_off, 0), 254)
            }
          break;
        }
                   
      }
    }  
    
    $(document).ready(function(){
    var items={option1:{value:1,text:"Time of day based"},option2:{value:2,text:"Continuously time based"},option3:{value:3,text:"Not time based"}}

    $.each(items, function (i, item) {

      $('.TimeSelect').append($('<option>', { 
        value: item.value,
        text : item.text 
      }));
    });
    
    })
  </script>
  <style>
    .inputer {
      -webkit-appearance: none;
      width: 10%;
      height: 20px;
      outline: none;
      opacity: 0.9;
      -webkit-transition: .5s;
      transition: opacity .5s;
    }
    .inputer:hover {
      opacity: 1;
    }
  </style>
  </head>
  <body onload="javascript:init()">
    <style>body{ background-color: #537846; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }</style>
    <a href="/"><button>HOME</button></a>
    <h1>Timing Page</h1>
  <br>Relay : &emsp;&emsp;Timing Function*: &emsp;&ensp;Hour ON &emsp;&ensp; Hour OFF
  <br><br>Relay 1: &emsp; <select id="Relay_select1" class="TimeSelect"></select>&emsp;<input id="Hour_on1" type="number" class="inputer"></input>&emsp;&emsp;<input id="Hour_off1" type="number" class="inputer"></input>
  <br><br>Relay 2: &emsp; <select id="Relay_select2" class="TimeSelect"></select>&emsp;<input id="Hour_on2" type="number" class="inputer"></input>&emsp;&emsp;<input id="Hour_off2" type="number" class="inputer"></input>
  <br><br>Relay 3: &emsp; <select id="Relay_select3" class="TimeSelect"></select>&emsp;<input id="Hour_on3" type="number" class="inputer"></input>&emsp;&emsp;<input id="Hour_off3" type="number" class="inputer"></input>
  <br><br>Relay 4: &emsp; <select id="Relay_select4" class="TimeSelect"></select>&emsp;<input id="Hour_on4" type="number" class="inputer"></input>&emsp;&emsp;<input id="Hour_off4" type="number" class="inputer"></input>
  <br><br>Relay 5: &emsp; <select id="Relay_select5" class="TimeSelect"></select>&emsp;<input id="Hour_on5" type="number" class="inputer"></input>&emsp;&emsp;<input id="Hour_off5" type="number" class="inputer"></input>
  <br><br>Relay 6: &emsp; <select id="Relay_select6" class="TimeSelect"></select>&emsp;<input id="Hour_on6" type="number" class="inputer"></input>&emsp;&emsp;<input id="Hour_off6" type="number" class="inputer"></input>
  <br><br>Relay 7: &emsp; <select id="Relay_select7" class="TimeSelect"></select>&emsp;<input id="Hour_on7" type="number" class="inputer"></input>&emsp;&emsp;<input id="Hour_off7" type="number" class="inputer"></input>
  <br><br>Relay 8: &emsp; <select id="Relay_select8" class="TimeSelect"></select>&emsp;<input id="Hour_on8" type="number" class="inputer"></input>&emsp;&emsp;<input id="Hour_off8" type="number" class="inputer"></input>
  <br><br><button onclick=sendTimeMode()>UPDATE</button><br>
  </div>
  <br><br><br><br><div>
  <p style="font-size:10px;">
  "Time of day based" = hour of the day dependent, therefore the times for hour on/off is limited between 0 and 23. This is military time.
  <br> "Continuously time based" = not hour of day dependant, asynchronous. limited between 0 and 254. 
  <br> "Not time based" = hours don't matter as the relay will never react to time based logic.
  <br> TIMED RELAY STATE DOESN'T MATTER IF THAT RELAY IS KEPT IN MANUAL!
  </p>
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
      if (AofRelays[i]->manual_mode){
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

void send_time_update(){
  Serial.print("\n");
  for(int i = 0; i < NUM_OF_RELAYS; i++){
      char c[24] = "RTS                   ";
      byte mes_end = 4;
      c[mes_end] = String(i + 1)[0];
      mes_end = 6;
      if (AofRelays[i]->timed_out_type == 1){
        c[mes_end] = 'T';
        mes_end += 1;
        c[mes_end] = 'O';
        mes_end += 1;
        c[mes_end] = 'D';
      }
      else if (AofRelays[i]->timed_out_type == 2){
        c[mes_end] = 'C';
        mes_end += 1;
      }
      else if (AofRelays[i]->timed_out_type == 3){
        c[mes_end] = 'N';
        mes_end += 1;
      }
      if (AofRelays[i]->timed_out_type != 1){
        c[mes_end] = 'T';
      }
      mes_end += 2;
      c[mes_end] = 'O';
      mes_end += 1;
      c[mes_end] = 'N';
      mes_end += 2;
      String temp = String(AofRelays[i]->hour_on);
      Serial.println(temp);
      for (int t = 0; t <= (temp.length() - 1); t++){
        Serial.println(temp[t]);
        c[mes_end] = temp[t];
        mes_end += 1;
      }
      mes_end += 1;
      c[mes_end] = 'O';
      mes_end += 1;
      c[mes_end] = 'F';
      mes_end += 2;
      
      temp = String(AofRelays[i]->hour_off);
      Serial.println(temp);
      for (int t = 0; t <= (temp.length() - 1); t++){
        Serial.println(temp[t]);
        c[mes_end] = temp[t];
        mes_end += 1;
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
  website_recently_served = true;
  last_website_served = 1;
}

void show_page_timing(){
  server.send(200, "text/html", page_timing);
  website_recently_served = true;
  last_website_served = 2;
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
  char relay_mode_char;
  int relay_number = 0;
  int relay_mode = 0;
  byte on_time = 0;
  byte off_time = 0;
  
  if (load.substring(0,5) == "Ready"){
    Serial.println("Website Ready");
    website_ready = true;
    return;
  }
  switch (last_website_served){
      case 1:
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
        
      break;
      case 2:
        if (load.substring(0,5) == "RTime"){
          relay_num_char = load.substring(6, 7)[0];
          relay_mode_char = load.substring(10, 11)[0];
          Serial.println(relay_mode_char);
          if (isDigit(relay_mode_char)){
            relay_mode = relay_mode_char - '0';
          }
          byte period = load.indexOf('.');
          byte on_loc = load.indexOf('O');
          byte off_loc = load.lastIndexOf('O');
          Serial.println(load.substring(on_loc + 2, off_loc - 1).toInt());
          Serial.println(load.substring(off_loc + 2, period).toInt());
          on_time = load.substring(on_loc + 2, off_loc - 1).toInt();
          off_time = load.substring(off_loc + 2, period).toInt();
        }
      break;
    }
  Serial.println(relay_num_char);
    if (isDigit(relay_num_char)){
      relay_number = relay_num_char - '0';
    }
  Serial.println(relay_number);
  
  for(int i = 0; i < NUM_OF_RELAYS; i++){
    if (relay_number == (i + 1)){
      if (last_website_served == 1){
        if (man){
          AofRelays[i]->manual_mode = true;
          AofRelays[i]->state.write(manual_state);
          AofRelays[i]->change_made = true;
        }
        else{
          AofRelays[i]->manual_mode = false;
          AofRelays[i]->state.write(AofRelays[i]->automatic_state);  //Automatic state
          AofRelays[i]->change_made = true;
        }
      }
      else{
        if (AofRelays[i]->timed_out_type != relay_mode){
          AofRelays[i]->timed_out_type = relay_mode;
          AofRelays[i]->change_made = true;
          if (relay_mode == TIME_OF_DAY_BASED){
            AofRelays[i]->counter = 20;
          }
          else if (relay_mode == CONTINUOUSLY_TIME_BASED){
            AofRelays[i]->counter = 0;
          }
        }
        if (AofRelays[i]->hour_on != on_time){
          AofRelays[i]->hour_on = on_time;
          AofRelays[i]->change_made = true;
        }
        if (AofRelays[i]->hour_off != off_time){
          AofRelays[i]->hour_off = off_time;
          AofRelays[i]->change_made = true;
        }
      }
    }
  }
}

void update_time_of_day_states(){
  byte current_hour = byte(timeClient.getHours());
  
  if (timeClient.isTimeSet()){
    for(int i = 0; i < NUM_OF_RELAYS; i++){

      if (AofRelays[i]->timed_out_type == TIME_OF_DAY_BASED){
        if ((AofRelays[i]->hour_on == current_hour) and not (AofRelays[i]->automatic_state)){
          if (AofRelays[i]->hour_on == AofRelays[i]->hour_off){
            if (AofRelays[i]->counter == 1){
              continue;
            }
            else{
              AofRelays[i]->counter = 1;
            }
          }
          AofRelays[i]->automatic_state = true;
          AofRelays[i]->change_made = true;
        }
        else if ((AofRelays[i]->hour_off == current_hour) and (AofRelays[i]->automatic_state)){
          if (AofRelays[i]->hour_on == AofRelays[i]->hour_off){
            if (AofRelays[i]->counter == 0){
              continue;
            }
            else{
              AofRelays[i]->counter = 0;
            }
          }
          AofRelays[i]->automatic_state = false;
          AofRelays[i]->change_made = true;
        }
      }
      else if (AofRelays[i]->timed_out_type == CONTINUOUSLY_TIME_BASED){
        
      }
      if (not AofRelays[i]->manual_mode and AofRelays[i]->change_made){  // f is the manual bit
        AofRelays[i]->state.write(AofRelays[i]->automatic_state);  //Automatic state
      }
    }
  }
  else{
    Serial.println("Time is not set properly, possibly no WIFI?");
  }
}


bool write_relay_data(){
  bool ret = true;
	for(int i = 0; i < NUM_OF_RELAYS; i++){
    char file_location[12] = "           ";
    char relay_char[] = {i + '0'};
    Serial.println(relay_char);
    strcpy(file_location,"/relay");
    strcat(file_location, relay_char);
    strcat(file_location, ".txt");
		File f = LittleFS.open(file_location, "\w");
		if (!f) {
      Serial.print("file open failed while writing file for relay: ");
      Serial.println(file_location);
      ret = false;
		}
		else{
			f.write((byte *)&AofRelays[i], sizeof(Logic_based_output));
		}
		f.close();
	}
  if (ret){
    Serial.println("successfully saved all relay settings");  
  }
  return ret;
}
