#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char* ssid = "wifi";
const char* password = "password";

WebServer server(80);
Servo myservo;

const int servoPin = 13;
const char* correctPassword = "open@123";

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  ESP32PWM::allocateTimer(0);
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 500, 2400);
  myservo.write(0);

  // web server
  server.on("/", handleRoot);
  server.on("/move", HTTP_POST, handleMove);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>ESP32 Servo Control</h1>";
  html += "<form action='/move' method='POST'>";
  html += "Password: <input type='password' name='password'><br>";
  html += "<input type='submit' value='Move Servo'>";
  html += "</form></body></html>";
  server.send(200, "text/html", html);
}

void handleMove() {
  String inputPassword = server.arg("password");
  
  if (inputPassword == correctPassword) {
    myservo.write(90);
    server.send(200, "text/plain", "Servo moved to 90 degrees. Will close after 5 seconds.");
    delay(5000);  // Wait for 5 seconds
    myservo.write(0);  // Move servo back to 0 degrees
    Serial.println("Servo moved back to 0 degrees");
  } else {
    server.send(401, "text/plain", "Incorrect password");
  }
}