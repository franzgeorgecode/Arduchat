#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi101.h>
#include <ArduinoBearSSL.h>
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char ssid[] = ""; // tu SSID de red (nombre)
char pass[] = "";    // tu contraseña de red (usa para WPA, o como clave para WEP)
int keyIndex = 0;            // tu número de índice de clave de red (solo necesario para WEP)
char server[] = "api.openai.com"; // dirección del servidor OpenAI API (usando DNS)
const char* apiKey = ""; // clave de la API de OpenAI

WiFiClient client;
BearSSLClient sslClient(client);

unsigned long getTime() {
  return WiFi.getTime();
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display(); // Mostrar el logotipo de Adafruit (opcional)
  delay(2000); // Esperar 2 segundos
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("I am : )");
  display.setCursor(16, 16);
  display.println("Arduchat");
  display.display();

  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    delay(1000);
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Connecting to WiFi...");
    display.display();
    Serial.println("Connecting to WiFi...");
    delay(10000);
    
  }
  delay(1000);
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("connected to wifi:");
  display.setCursor(16,16);
  display.println(ssid);
  display.display();
  Serial.println("connected to wifi:");
  Serial.println(ssid);
  delay(10000);
  
  ArduinoBearSSL.onGetTime(getTime);
}



void loop() {

display.clearDisplay(); // limpia la pantalla
display.setCursor(0, 0); // establece el cursor para escribir en la pantalla
display.println("Ask Me"); // escribe en la pantalla OLED
display.setCursor(16, 16);
display.println("I have super powers!!");
display.display(); // actualiza la pantalla
delay(5000);

display.clearDisplay();
display.setCursor(0, 0);
display.println("What is");
display.setCursor(16, 16);
display.println("Arduino?");
display.display();
delay(5000);


if (sslClient.connect("api.openai.com", 443)) {
Serial.println("Connected to server: api.openai.com");
display.clearDisplay();
display.setCursor(0,0);
display.println("connected to server:");
display.setCursor(16,16);
display.println("api.openai.com");
display.display();
Serial.println("connected to server:");
Serial.println("api.openai.com");
delay(1000);
DynamicJsonDocument doc(2048);
doc["prompt"] = "What is Arduino?";
doc["temperature"] = 0.7;
doc["max_tokens"] = 79;
doc["top_p"] = 1;
doc["frequency_penalty"] = 0;
doc["presence_penalty"] = 0;
String requestBody;
serializeJson(doc, requestBody);
sslClient.print("POST /v1/engines/davinci/completions HTTP/1.1\r\n");
sslClient.print("Host: api.openai.com\r\n");
sslClient.print("Content-Type: application/json\r\n");
sslClient.print("Authorization: Bearer ");
sslClient.print(apiKey);
sslClient.print("\r\n");
sslClient.print("Content-Length: ");
sslClient.print(requestBody.length());
sslClient.print("\r\n\r\n");
sslClient.print(requestBody);
delay(1000);
char response[2048] = {0};
int responseIndex = 0;
memset(response, 0, sizeof(response));
while (sslClient.connected()) {
while (sslClient.available()) {
char c = sslClient.read();
if (responseIndex < sizeof(response) - 1) {
response[responseIndex++] = c;
}
}
}
Serial.println("Size of Answer: ");
Serial.println(strlen(response));
Serial.println("This is the answer from OpenAI: ");
Serial.println(response);
delay(1000);
display.clearDisplay();
display.setCursor(0,0);
display.println("Open Ai Answer:");
display.setCursor(0,16);
display.println(response);
display.display();
delay(25000);
response[responseIndex] = '\0';
DynamicJsonDocument doc2(2048);
DeserializationError error = deserializeJson(doc2, response);
if (error) {
Serial.print("Failed to parse JSON: ");
Serial.println(error.c_str());
} else {
if (!doc2.containsKey("choices") || !doc2["choices"].is<JsonArray>() || doc2["choices"].size() == 0 || !doc2["choices"][0]["text"].is<String>()) {
Serial.println("Error: Invalid JSON response from OpenAI API");
} else {
String completionText = doc2["choices"][0]["text"].as<String>();
Serial.println("ChatGPT response: " + completionText);
display.clearDisplay();
display.setCursor(0,0);
display.println("Open Ai Answer:");
display.setCursor(0,16);
display.println(completionText);
display.display();
delay(15000);
}
}
sslClient.stop();
}
}
