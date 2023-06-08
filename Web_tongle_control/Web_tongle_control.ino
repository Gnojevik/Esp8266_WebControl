/*esp8266 виступає у ролі веб сервера через який можна керувати на даний момент
можна керувати:
-світлодіодом
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// Задайте ім'я та пароль вашої Wi-Fi мережі
const char* ssid = "gnojevik";
const char* password = "19377391";

// Ініціалізуйте об'єкт ESP8266WebServer
ESP8266WebServer server(80);

// Пін для керування світлодіодом
const int ledPin = 2;
bool ledState = false;

// HTML сторінка
const char* html = R"html(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta charset="UTF-8">
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 20px;
    }

    .toggle {
      position: relative;
      display: inline-block;
      width: 60px;
      height: 34px;
    }

    .toggle input {
      display: none;
    }

    .slider {
      position: absolute;
      cursor: pointer;
      top: 0;
      left: 0;
      right: 0;
      bottom: 0;
      background-color: #ccc;
      transition: .4s;
    }

    .slider:before {
      position: absolute;
      content: "";
      height: 26px;
      width: 26px;
      left: 4px;
      bottom: 4px;
      background-color: white;
      transition: .4s;
    }

    input:checked + .slider {
      background-color: #4CAF50;
    }

    input:checked + .slider:before {
      transform: translateX(26px);
    }

    .text {
      display: inline-block;
      vertical-align: middle;
      margin-left: 10px;
    }
  </style>
</head>
<body>
  <h1>Керування світлодіодом</h1>
  <label class="toggle">
    <input type="checkbox" id="ledToggle" onclick="toggleLed(this.checked)">
    <span class="slider"></span>
  </label>
  <span class="text" id="ledStatus">Вимкнути</span>

  <script>
    function toggleLed(state) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          // Оновити статус світлодіода
          document.getElementById("ledStatus").textContent = (this.responseText === "true") ? "Увімкнуто" : "Вимкнуто";
        }
      };
      xhttp.open("GET", "/led?state=" + (state ? "1" : "0"), true);
      xhttp.send();
    }
  </script>
</body>
</html>

)html";

// Функція обробки запиту на кореневу сторінку
void handleRoot() {
  server.send(200, "text/html", html);
}

// Функція обробки запиту на керування світлодіодом
void handleLed() {
  if (server.arg("state") == "1") {
    digitalWrite(ledPin, LOW);  // Увімкнути світлодіод
    ledState = true;
  } else {
    digitalWrite(ledPin, HIGH);   // Вимкнути світлодіод
    ledState = false;
  }

  server.send(200, "text/plain", String(ledState));
}

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);     // Вимкнути світлодіод за замовчуванням

  // Підключення до Wi-Fi мережі
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  server.on("/", handleRoot);    // Обробник запиту на кореневу сторінку
  server.on("/led", handleLed);  // Обробник запиту на керування світлодіодом

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
