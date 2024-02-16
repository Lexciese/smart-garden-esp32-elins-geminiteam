#include <WiFi.h>
#include <WebServer.h>
#include <math.h>
#include <string>

// Setup
#define RXp2 16
#define TXp2 17
const char Ultrasonik_Trigger = 5;
const char Ultrasonik_Echo = 18; 
String dirt_status;
String water_level;
String comInfo;
const char interupt_pin_irrigate = 5;

int distance;
int duration;
int moisture_signal_pin = 35;
int moisture_percentage;

// Wifi Setup
const char* ssid = "Gemini Team";
const char* password = "Terbaik12345";
IPAddress local_ip(192,168,0,1);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);
WebServer server(80);

void setup()
{
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2); //from arduino

  // Start Wifi
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  // delay(100);
  server.on("/", page_root);
  server.on("/irrigate_on", irrigate_on);
  server.onNotFound(page_404);
  server.begin();
}

void loop()
{
  server.handleClient();
  // Serial.println(Serial2.readString().substring(0, 3));
  comInfo = Serial2.readString();
  if (comInfo.substring(0, 3) == "cm:") {
    // percentage based on max 50cm
    distance = (comInfo.substring(3).toInt());
    Serial.println(distance);
    water_level = String(distance);
  }
  
  comInfo = Serial2.readString();
  if(comInfo.substring(0, 3) == "mp:") {
    moisture_percentage = comInfo.substring(3, 5).toInt();
  }
  if (moisture_percentage < 30) {
    dirt_status = "Dry";
  }
  else if (moisture_percentage > 30 && moisture_percentage < 70) {
    dirt_status = "Normal"; //normal
  }
  else {
    dirt_status = "Wet"; //wet
  }
  
}

void irrigate_on() {
  digitalWrite(interupt_pin_irrigate, HIGH);
  Serial.println("Irrigate");
  delay(5000);
  digitalWrite(interupt_pin_irrigate, LOW);
  server.send(200, "text/html", page_HTML());
  
}

void page_root() {
  server.send(200, "text/html", page_HTML()); 
}

void page_404(){
  server.send(404, "text/plain", "Not found");
}

String page_HTML() {
  String html = R"(
<!DOCTYPE html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            font-family: Arial, Helvetica, sans-serif; display: flex;flex-direction: column;
        }
        h3 {
            color: white; background-color: #0B60B0; padding: 10px; text-align: center;
        }
        h4 {
            margin-bottom: 10px;
        }
        #main {
            flex: 1;
        }
        #irrigate_water {
            display: block;
        }
        #water_level {
            display: flex; flex-direction: row;
        }
        button {
            padding: 10px;
            margin-bottom: 10px;
        }
        hr {
            border-top: 3px solid #bbb;
        }
        progress {
            transform: rotate(-90deg); margin-top: 75px; margin-bottom: 75px; padding-top: 100px; width: 200px;
        }
        label {
            font-weight: bolder; font-size: large; margin-left: -50px; margin-top: 100px;
        }
        footer {
            text-align: end; display: block; height: 80px; padding: 10px;
        }
        #loader {
            border: 5px solid #f3f3f3; /* Light grey */
            border-top: 5px solid #3498db; /* Blue */
            border-radius: 50%;
            width: 15px;
            height: 15px;
            animation: spin 2s linear infinite;
            display: none;
        }

            @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
    </style>
    <title>Smart Watering Garden</title>
</head>
<body>
    <h3>Smart Watering Garden</h3>
    <div id="main">
        <div id="send_command">
            <h4>Send Manual Command</h4>
            <a href="/irrigate_on">
            <button id="irrigate_water" onclick='LoadingShow()' >Irrigate Water</button>
            </a>
            <div id="loader"></div>
        </div>
        <hr>
        <div>
            <h4>Dirt Status</h4>
            <p>)"; html+= dirt_status; html+= R"(</p>
        </div>
        <hr>
        <div>
            <h4>Water Tank Level</h4>
            <div id="water_level">
                <progress value=")"; html+= water_level; html+= R"(" max="350"></progress>
                <label>)"; html+=water_level; html+=R"( cm</label>
            </div>
        </div>

        <hr>
    </div>
    <footer>2024. Created by Gemini Team</footer>

    <script>
        function LoadingShow() {
            document.getElementById("loader").style.display = "block";
            document.getElementById("irrigate_water").style.display = "none";
            setTimeout(LoadingHide, 3000);
        }
        function LoadingHide() {
            document.getElementById("loader").style.display = "none";
            document.getElementById("irrigate_water").style.display = "block";
        }
    </script>

</body>
</html>

)";
  return html;
}

