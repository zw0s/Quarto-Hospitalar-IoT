#include <WiFi.h>
#include <DHT.h>
#include <ESPAsyncWebSrv.h>
#include <AsyncTCP.h>
#include <ESP32Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Login e senha wifi
const char* ssid = "Redmi 9";
const char* password = "12345678";

// Cria instância do objeto DHT (sensor hum/temp)
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Cria instância do objeto Piezo (sensor piezoelet)
#define PIEZOPIN 32

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -10800; // Ajuste de fuso horário em segundos (GMT-3, Brasília)
const int daylightOffset_sec = 0; // Ajuste de horário de verão em segundos

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);
String eventMessage = "Nenhum evento";
String eventTime = "N/A";
String eventList = "";

// Define os pinos do led
#define LED_PIN 4
#define LED_PIN_2 5
#define LED_PIN_3 18

Servo servo;

// LED estado
bool ledState = false;
bool ledState2 = false;
bool ledState3 = false;
bool buttonPressed = false;

// Cria a instância do webserver
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  // Conecta o Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Inicializa o sensor DHT
  dht.begin();
  // Inicializa o servidor NTP
  timeClient.begin();

  // Seta os pinos no Led como saída e o piezo entrada
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(PIEZOPIN, INPUT);

  // Seta o servo
  servo.attach(12); 
  servo.write(90); // Defina o ângulo inicial do servo (90 graus é uma posição central)

  // Pág inicial HTML
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><head><title>Hospital Engenharia Unificada II</title><style>";
    html += "body {font-family: Arial, sans-serif; margin: 0; padding: 0; background-color: #f4f4f4;}";
    html += "header {background-color: #3498db; color: #fff; text-align: center; padding: 20px;}";
    html += "nav {background-color: #555; color: #fff; text-align: center; padding: 10px;}";
    html += "nav a {text-decoration: none; color: #fff; margin: 0 10px;}";
    html += "main {padding: 20px;}";
    html += "footer {background-color: #3498db; color: #fff; text-align: center; padding: 10px;}";
    html += "</style></head><body>";
    html += "<header><h1>Hospital Engenharia Unificada II</h1></header>";
    html += "<nav>";
    html += "<a href='/temperatura'>Sensor de Temperatura e Umidade</a>";
    html += "<a href='/iluminacao'>Iluminac&atilde;o</a>";
    html += "<a href='/cama'>Cama</a>";
    html += "<a href='/queda'>Queda</a>";
    html += "</nav>";
    html += "<main>";
    html += "<h2>Bem-vindo ao Hospital Engenharia Unificada II</h2>";
    html += "<p>Esse &eacute o prot&oacute;tipo de quarto de hospital inteligente desenvolvido para a disciplina Engenharia Unificada II, o projeto consiste em controlar o quarto remotamente utilizando uma comunicac&atilde;o Wi-Fi onde &eacute poss&iacute;vel verificar a temperatura do quarto, ajustar a cama, controlar a iluminac&atilde;o e saber se o paciente saiu ou caiu da cama.</p>";
    html += "</main>";
    html += "<footer><p>&copy; 2023 Hospital Engenharia Unificada II</p></footer>";
    html += "</body></html>";

    request->send(200, "text/html", html);
  });

//Pág sensor DHT
  server.on("/temperatura", HTTP_GET, [](AsyncWebServerRequest *request){
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    
    String html = "<html><head><title>Sensor de Temperatura e Umidade</title><style>";
    html += "body {font-family: Arial, sans-serif; margin: 0; padding: 0; background-color: #f4f4f4;}";
    html += "header {background-color: #3498db; color: #fff; text-align: center; padding: 20px;}";
    html += "nav {background-color: #555; color: #fff; text-align: center; padding: 10px;}";
    html += "nav a {text-decoration: none; color: #fff; margin: 0 10px;}";
    html += "main {padding: 20px;}";
    html += "footer {background-color: #3498db; color: #fff; text-align: center; padding: 10px;}";
    html += "</style></head><body>";
    html += "<header><h1>Hospital Engenharia Unificada II</h1></header>";
    html += "<nav>";
    html += "<a href='/'>P&aacute;gina Inicial</a>";
    html += "<a href='/temperatura'>Sensor de Temperatura e Umidade</a>";
    html += "<a href='/iluminacao'>Iluminac&atilde;o</a>";
    html += "<a href='/cama'>Cama</a>";
    html += "<a href='/queda'>Queda</a>";
    html += "</nav>";
    html += "<main>";
    html += "<h2>Sensor de Temperatura e Umidade</h2>";
    html += "<p><b>Dados do Sensor:</b></p>";
    html += "<p>Umidade: " + String(humidity) + "%</p>";
    html += "<p>Temperatura: " + String(temperature) + "&#8451;</p>";
    html += "</main>";
    html += "<footer><p>&copy; 2023 Hospital Engenharia Unificada II</p></footer>";
    html += "</body></html>";

    request->send(200, "text/html", html);
  });

//Pág leds
   server.on("/iluminacao", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><head><title>Iluminac&atilde;o</title><style>";
    html += "body {font-family: Arial, sans-serif; margin: 0; padding: 0; background-color: #f4f4f4;}";
    html += "header {background-color: #3498db; color: #fff; text-align: center; padding: 20px;}";
    html += "nav {background-color: #555; color: #fff; text-align: center; padding: 10px;}";
    html += "nav a {text-decoration: none; color: #fff; margin: 0 10px;}";
    html += "main {padding: 20px;}";
    html += "footer {background-color: #3498db; color: #fff; text-align: center; padding: 10px;}";
    html += "</style></head><body>";
    html += "<header><h1>Hospital Engenharia Unificada II</h1></header>";
    html += "<nav>";
    html += "<a href='/'>P&aacute;gina Inicial</a>";
    html += "<a href='/temperatura'>Sensor de Temperatura e Umidade</a>";
    html += "<a href='/iluminacao'>Iluminac&atilde;o</a>";
    html += "<a href='/cama'>Cama</a>";
    html += "<a href='/queda'>Queda</a>";
    html += "</nav>";
    html += "<main>";
    html += "<h2>Iluminac&atilde;o</h2>";
    
    // Botão para ligar/desligar o LED
    html += "<p><b>Controle da Iluminac&atilde;o:</b></p>";
    html += "<form action='/toggle' method='POST'>";
    html += "<button name='toggle' value='on'>Ligar</button>";
    html += "<button name='toggle' value='off'>Desligar</button>";
    html += "</form>";
    
    // Adiciona algum espaço vertical entre os botões
    html += "<br>";

    // Botão para ligar/desligar o LED_PIN_2
    html += "<p><b>Controle da Iluminac&atilde;o 2:</b></p>";
    html += "<form action='/toggle2' method='POST'>";
    html += "<button name='toggle' value='on'>Ligar</button>";
    html += "<button name='toggle'' value='off'>Desligar</button>";
    html += "</form>";
    
    html += "<br>";
    
    // Botão para ligar/desligar o LED_PIN_3
    html += "<p><b>Controle da Iluminac&atilde;o 3:</b></p>";
    html += "<form action='/toggle3' method='POST'>";
    html += "<button name='toggle' value='on'>Ligar</button>";
    html += "<button name='toggle' value='off'>Desligar</button>";
    html += "</form>";

    html += "</main>";
    html += "<footer><p>&copy; 2023 Hospital Engenharia Unificada II</p></footer>";
    html += "</body></html>";

    request->send(200, "text/html", html);
  });

//Lógica para comunicação dos leds
  server.on("/toggle", HTTP_POST, [](AsyncWebServerRequest *request){
    String toggleState;
    if (request->hasParam("toggle", true)) {
      toggleState = request->getParam("toggle", true)->value();
    }

    if (toggleState == "on") {
      digitalWrite(LED_PIN, HIGH); // Ligar o LED
    } else if (toggleState == "off") {
      digitalWrite(LED_PIN, LOW); // Desligar o LED
    }

    request->redirect("/iluminacao"); // Redirecionar de volta para a página de iluminação
  });

  // Manipulador para o LED_PIN_2
  server.on("/toggle2", HTTP_POST, [](AsyncWebServerRequest *request){
    String toggleState;
    if (request->hasParam("toggle", true)) {
      toggleState = request->getParam("toggle", true)->value();
    }

    if (toggleState == "on") {
      digitalWrite(LED_PIN_2, HIGH); // Ligar o LED_PIN_2
    } else if (toggleState == "off") {
      digitalWrite(LED_PIN_2, LOW); // Desligar o LED_PIN_2
    }

    request->redirect("/iluminacao"); // Redirecionar de volta para a página de iluminação
  });

   // Manipulador para o LED_PIN_3
  server.on("/toggle3", HTTP_POST, [](AsyncWebServerRequest *request){
    String toggleState;
    if (request->hasParam("toggle", true)) {
      toggleState = request->getParam("toggle", true)->value();
    }

    if (toggleState == "on") {
      digitalWrite(LED_PIN_3, HIGH); // Ligar o LED_PIN_3
    } else if (toggleState == "off") {
      digitalWrite(LED_PIN_3, LOW); // Desligar o LED_PIN_3
    }

    request->redirect("/iluminacao"); // Redirecionar de volta para a página de iluminação
  });

//Pág servo cama
  server.on("/cama", HTTP_GET, [](AsyncWebServerRequest *request){
  String html = 
    "<html><head><title>Controle da Cama</title></head><style>"
    "body {font-family: Arial, sans-serif; margin: 0; padding: 0; background-color: #f4f4f4;}"
    "header {background-color: #3498db; color: #fff; text-align: center; padding: 20px;}"
    "nav {background-color: #555; color: #fff; text-align: center; padding: 10px;}"
    "nav a {text-decoration: none; color: #fff; margin: 0 10px;}"
    "main {padding: 20px;}"
    "footer {background-color: #3498db; color: #fff; text-align: center; padding: 10px;}"
    "</style></head><body>"
    "<header><h1>Hospital Engenharia Unificada II</h1></header>"
    "<nav>"
    "<a href='/'>P&aacute;gina Inicial</a>"
    "<a href='/temperatura'>Sensor de Temperatura e Umidade</a>"
    "<a href='/iluminacao'>Iluminac&atilde;o</a>"
    "<a href='/cama'>Cama</a>"
    "<a href='/queda'>Queda</a>"
    "</nav>"
    "<main>"
    "<h1>Controle da Cama</h1>"
    "<div id='joystick' style='width: 200px; height: 200px; background: lightgray; border: 2px solid black; position: relative;'>"
    "  <div id='knob' style='width: 50px; height: 50px; background: red; border: 2px solid black; border-radius: 50%; position: absolute;'></div>"
    "</div>"
    "<script>"
    "  const joystick = document.getElementById('joystick');"
    "  const knob = document.getElementById('knob');"
    "  joystick.addEventListener('mousemove', (e) => {"
    "    const x = e.offsetX - knob.offsetWidth / 2;"
    "    const y = e.offsetY - knob.offsetHeight / 2;"
    "    knob.style.transform = `translate(${x}px, ${y}px`;"
    "    fetch(`/servo?x=${x}&y=${y}`);"
    "  });"
    "</script>"
    "</main>"
    "<footer><p>&copy; 2023 Hospital Engenharia Unificada II</p></footer>"
    "</body></html>";

  request->send(200, "text/html", html);
});

//Lógica comunicação servo
  server.on("/servo", HTTP_GET, [](AsyncWebServerRequest *request){
    int x = request->arg("x").toInt();
    int y = request->arg("y").toInt();
    
    // Mapeie as coordenadas x e y para os ângulos do servo (ajuste conforme necessário)
    int servoXAngle = map(x, 0, 200, 0, 180);
    int servoYAngle = map(y, 0, 200, 0, 180);
    
    servo.write(servoXAngle);

    request->send(200, "text/plain", "Servo position set");
  });

//Pág queda
  server.on("/queda", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><head><title>Queda</title><style>";
    html += "body {font-family: Arial, sans-serif; margin: 0; padding: 0; background-color: #f4f4f4;}";
    html += "header {background-color: #3498db; color: #fff; text-align: center; padding: 20px;}";
    html += "nav {background-color: #555; color: #fff; text-align: center; padding: 10px;}";
    html += "nav a {text-decoration: none; color: #fff; margin: 0 10px;}";
    html += "main {padding: 20px;}";
    html += "footer {background-color: #3498db; color: #fff; text-align: center; padding: 10px;}";
    html += "</style></head><body>";
    html += "<header><h1>Hospital Engenharia Unificada II</h1></header>";
    html += "<nav>";
    html += "<a href='/'>P&aacute;gina Inicial</a>";
    html += "<a href='/temperatura'>Sensor de Temperatura e Umidade</a>";
    html += "<a href='/iluminacao'>Iluminac&atilde;o</a>";
    html += "<a href='/cama'>Cama</a>";
    html += "<a href='/queda'>Queda</a>";
    html += "</nav>";
    html += "<main>";
    html += "<h2>Queda</h2>";
    html += "<p>&Uacute;ltimo evento: " + eventMessage + "</p>";
      if (!eventList.isEmpty()) {
        html += "<p>Eventos anteriores:</p>";
        html += "<ul>" + eventList + "</ul>";
    }
      if (!eventTime.isEmpty()) {
        html += "<p>Hora do &uacute;ltimo evento: " + eventTime + "</p>";
    }
    html += "<p>Hora atual: " + timeClient.getFormattedTime() + "</p>";
    html += "</main>";
    html += "<footer><p>&copy; 2023 Hospital Engenharia Unificada II</p></footer>";
    html += "</body></html>";

    request->send(200, "text/html", html);
  });

  server.begin();
}

void loop() {
  // Lê os dados do sensor DHT 
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  delay(100); // Ajuste o atraso conforme necessário

   // Verifica se o sensor piezoelétrico foi pressionado
  int piezoValue = digitalRead(PIEZOPIN);
  if (piezoValue == HIGH && !buttonPressed) {
    // Sensor piezoelétrico foi pressionado
    eventMessage = "O Paciente saiu da cama";
    eventTime = timeClient.getFormattedTime();
    eventList = "<li>" + eventMessage + " - " + eventTime + "</li>" + eventList;
    buttonPressed = true;
    Serial.println(eventMessage);
    Serial.println("Hora do evento: " + eventTime);
  
  } else if (piezoValue == LOW) {
    // Resetar a variável quando o botão não está pressionado
    buttonPressed = false;
  }
  timeClient.update();
}