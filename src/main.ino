#include <ESP32PWM.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>
#include <math.h>
#include <string>

// Setup
const char Ultrasonik_Input = 3;
const char Ultrasonik_Output = 2;
String dirt_status;
String water_level;
Servo servo1;
Servo servo2;

short distance;
long duration;
int moisture_signal_pin = 19;
float moisture_percentage = 100;

// Wifi Setup
const char* ssid = "Gemini Team";
const char* password = "Terbaik12345";
IPAddress local_ip(192,168,0,1);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);
WebServer server(80);

void setup()
{
  Serial.begin(9600);
  // pinMode(LED_BUILTIN, OUTPUT);
  pinMode(Ultrasonik_Output, OUTPUT);
  pinMode(Ultrasonik_Input, INPUT);
  pinMode(moisture_signal_pin, INPUT);
  servo1.attach(5);
  servo2.attach(6);

  // Start Wifi
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  server.on("/", page_root);
  server.on("/irrigate_on", irrigate_on);
  server.onNotFound(page_404);
  server.begin();
  Serial.println("Server started at 192.168.0.1");
}

void loop()
{
  server.handleClient();

  // Ketinggian air dalam cm
  digitalWrite(Ultrasonik_Output, LOW);
  delayMicroseconds(2);
  digitalWrite(Ultrasonik_Output, HIGH);
  delayMicroseconds(10);
  digitalWrite(Ultrasonik_Output, LOW);
  duration = pulseIn(Ultrasonik_Input, HIGH);
  duration = abs((duration*0.034/2) - 100); // TO:DO needed to convert range to percentage
  water_level = (String)duration;
  // Serial.print(water_level);
  // Serial.print('\n');
  // Servo Control Based on Water Level; If Water level is low, servo turn on until water level is high
  // tandon
  if (duration < 30) {
    servo1.write(90);
  }
  if(duration > 70) {
    servo1.write(0);
  }
  // Servo Control Based on ground moisture; if it low, open the servo
  // keran penyiram
  irrigate();
  
  
  // Moisture Reading;
  moisture_percentage = (100 - ((analogRead(moisture_signal_pin)/1023.00) * 100 ));
  // Serial.print("Moisture = ");
  if (moisture_percentage < 30) {
    dirt_status = "DRY";
  }
  else if (moisture_percentage > 30 && moisture_percentage < 70) {
    dirt_status = "NORMAL";
  }
  else {
    dirt_status = "WET";
  }
  // Serial.print(moisture_percentage);
  // Serial.print('\n');
  
}

void irrigate() {
    if (moisture_percentage < 20) {
    servo2.write(90);
    // delay 5 detik
    delay(5000);
  }
  if (moisture_percentage > 70) {
    servo2.write(0);
  }
}

void irrigate_on() {
  servo2.write(90);
  Serial.println("Water is Irrigated!!!!!!!!!!!!");
  delay(5000);
  server.send(200, "text/html", page_HTML());
  servo2.write(0);
  
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
                <progress value=")"; html+= water_level; html+= R"(" max="100"></progress>
                <label>)"; html+=water_level; html+=R"(%</label>
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

