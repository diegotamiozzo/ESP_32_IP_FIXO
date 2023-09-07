/*
Diego Tamiozzo
07/09/2023
Acionamento sem fio com ESP32 utilizando HTML
Necessita estar conectado na mesma rede WIFI
Este código permite controlar quatro saídas digitais e exibir informações de 
temperatura e umidade em uma página da web acessível via Wi-Fi. 
Quando o cliente faz uma solicitação HTTP, o ESP32 responde com a página HTML apropriada e realiza as ações de controle das saídas.
OBSERVAÇÃO:
Configure o IP fixo, gateway e máscara para o ESP32
Incluir "rede WIFi" e "Senha"
*/

// Bibliotecas utilizadas
#include <WiFi.h>
#include <ESPmDNS.h>
#include <DHT.h>

#define DHTPIN 14    // Pino onde o sensor DHT11 está conectado
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

IPAddress ip(192, 168, 3, 100);     // Defina o endereço IP fixo desejado para o ESP32
IPAddress gateway(192, 168, 3, 1);  // Defina o gateway, Escolha a opção “executar”, digite o comando “cmd" e tecle “Enter”, no “prompt de comando” digite o comando “ipconfig” e tecle “Enter”
IPAddress subnet(255, 255, 255, 0); // Defina a máscara de sub-rede da sua rede

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(26, OUTPUT);
  digitalWrite(13, LOW);
  digitalWrite(12, LOW);
  digitalWrite(27, LOW);
  digitalWrite(26, LOW);
  Serial.print("Conectando");

  WiFi.config(ip, gateway, subnet);       // Configure o IP fixo, gateway e máscara de sub-rede apenas para o ESP32
  WiFi.begin("REPETIDOR", "CORCEL1973");  // Incluir "rede WIFi", "Senha" de acordo com sua rede

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectou");

  server.begin();

  Serial.print("Server conectado em: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("mDNS responder started");
  }

  dht.begin();  // Inicializa o sensor DHT11
}

void loop() {
  WiFiClient client = server.available();

  if (!client) {
    return;
  }

  Serial.println("Novo cliente conectou");

  while (!client.available()) {
    delay(100);
  }

  String req = client.readStringUntil('\r');
  Serial.print("Requisição: ");
  Serial.println(req);

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  String dhtData = "<div id='dht-container' class='dht-container'>"
                   "<div class='dht-data'>"
                   "<p>Temperatura: <span id='temperature'>" + String(temperature, 1) + "&deg;C</span></p>"
                   "<p>Umidade: <span id='humidity'>" + String(humidity, 1) + "</span>%</p>"
                   "</div>"
                   "</div>";

  String html =
    "<html>"
    "<head>"
    "<meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'/>"
    "<title>ESP32</title>"
    "<style>"
    "body{"
    "text-align: center;"
    "font-family: sans-serif;"
    "font-size:14px;"
    "padding: 25px;"
    "}"
    "p{"
    "color:#444;"
    "}"
    "h1{"
    "color: #1fa3ec;"
    "}"
    ".button-container{"
    "display: inline-block;"
    "margin: 10px;"
    "}"
    ".button-container p{"
    "margin-bottom: 5px;"
    "}"
    "button{"
    "outline: none;"
    "border: 2px solid #1fa3ec;"
    "border-radius:18px;"
    "background-color:#FFF;"
    "color: #1fa3ec;"
    "padding: 10px 50px;"
    "}"
    "button:active{"
    "color: #fff;"
    "background-color:#1fa3ec;"
    "}"
    ".dht-data{"
    "border: 1px solid #ccc;"
    "padding: 10px;"
    "margin-top: 20px;"
    "}"
    ".update-button {"
    "margin-top: 20px;"
    "}"
    "</style>"
    "<script>"
    "function updateDHTValues(temperature, humidity) {"
    "    document.getElementById('temperature').textContent = temperature.toFixed(1);"
    "    document.getElementById('humidity').textContent = humidity.toFixed(1);"
    "}"
    "</script>"
    "</head>"
    "<body>"
    "<h1>Acionamento sem fio</h1>"
    "<h1>Diego Tamiozzo</h1>"
    "<div class='button-container'>"
    "<p>SAIDA 01</p>"
    "<p><a href='?acao=gpio0On'><button>ON</button></a></p>"
    "<p><a href='?acao=gpio0Off'><button>OFF</button></a></p>"
    "</div>"
    "<div class='button-container'>"
    "<p>SAIDA 02</p>"
    "<p><a href='?acao=gpio2On'><button>ON</button></a></p>"
    "<p><a href='?acao=gpio2Off'><button>OFF</button></a></p>"
    "</div>"
    "<div class='button-container'>"
    "<p>SAIDA 03</p>"
    "<p><a href='?acao=gpio4On'><button>ON</button></a></p>"
    "<p><a href='?acao=gpio4Off'><button>OFF</button></a></p>"
    "</div>"
    "<div class='button-container'>"
    "<p>SAIDA 04</p>"
    "<p><a href='?acao=gpio5On'><button>ON</button></a></p>"
    "<p><a href='?acao=gpio5Off'><button>OFF</button></a></p>"
    "</div>"
    + dhtData +
    "<div class='update-button'>"
    "<button onclick='location.reload();'>Atualizar Temperatura e Umidade</button>"
    "</div>"
    "</body>"
    "</html>";

  client.print(html);
  client.flush();

  if (req.indexOf("acao=gpio0On") != -1) {
    digitalWrite(13, HIGH);
  } else if (req.indexOf("acao=gpio0Off") != -1) {
    digitalWrite(13, LOW);
  } else if (req.indexOf("acao=gpio2On") != -1) {
    digitalWrite(12, HIGH);
  } else if (req.indexOf("acao=gpio2Off") != -1) {
    digitalWrite(12, LOW);
  } else if (req.indexOf("acao=gpio4On") != -1) {
    digitalWrite(27, HIGH);
  } else if (req.indexOf("acao=gpio4Off") != -1) {
    digitalWrite(27, LOW);
  } else if (req.indexOf("acao=gpio5On") != -1) {
    digitalWrite(26, HIGH);
  } else if (req.indexOf("acao=gpio5Off") != -1) {
    digitalWrite(26, LOW);
    client.stop();
    Serial.println("Cliente desconectado");
  }
}
