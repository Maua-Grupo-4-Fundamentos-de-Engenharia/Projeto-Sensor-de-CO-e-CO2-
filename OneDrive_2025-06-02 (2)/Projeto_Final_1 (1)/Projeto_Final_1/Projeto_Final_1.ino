 
//#include "BluetoothSerial.h"            // Biblioteca para Bluetooth Serial
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "ThingSpeak.h"
#include <WiFi.h>


#define I2C_SDA 21
#define I2C_SCL 22


LiquidCrystal_I2C lcd(0x27, 16, 2);     // Endereço I2C do LCD: 0x27, 16 colunas, 2 linhas


// Variáveis para controle do tempo
unsigned long lastTime = 0; // Tempo da última execução
const unsigned long interval = 30000; // Intervalo de 30 segundos (em milissegundos)

// Configurações do ThingSpeak
const char* apiKey = "8GLMCFTD50O81V6G";      // Substitua pela API Key do seu canal ThingSpeak
const long channelID = 2969800;               // Substitua pelo ID do seu canal ThingSpeak

const char* ssid = "iphonedecesar";                   // Roteador do seu celular
const char* password = "12345678";            // Senha do seu roteador

WiFiClient client; // Wifi simples

int buzzerCO = 25;
int buzzerCO2 = 26;
int led = 2;
int n; 
int cont;                                 // Variável inteira - Global
boolean pisca = false;


const int numCO2Readings = 20;
float CO2Readings[numCO2Readings] = {0};
int readCO2Index = 0;
float CO2Total = 0;

const int numCOReadings = 20;
float COReadings[numCOReadings] = {0};
int readCOIndex = 0;
float COTotal = 0;



void setup() {
     pinMode(led, OUTPUT);                // Configura pino 2  como saída
     Serial.begin(115200);              // Configura porta serial com velocidade de 115200 bps
     Serial.println(" IMT 2025 - Eng. Computação");

    pinMode(buzzerCO,OUTPUT);
    pinMode(buzzerCO2,OUTPUT);

     Wire.begin(I2C_SDA, I2C_SCL); // Inicializa a comunicação I2C com os pinos definidos
     lcd.begin(16, 2); // Inicializa o LCD com 16 colunas e 2 linhas
     lcd.backlight();  // Liga a luz de fundo
     lcd.setCursor(0, 0);
     lcd.print("IMT 2025");
     lcd.setCursor(0, 1);
     lcd.print("Projeto Final");
     delay(1000);
     lcd.clear();
  



    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    cont = 0;
    Serial.print("Setup - Iniciando a conexão WiFi");
    while (WiFi.status() != WL_CONNECTED) 
    {
      pisca = !pisca;
      digitalWrite(led, pisca); // Led Azul

      Serial.print("Setup - Tentando Conexão Wifi: "); // Depuração
      cont = cont + 1;                                 // Depuração
      Serial.println(cont);                            // Depuração

      lcd.clear();
      lcd.backlight();
      lcd.setCursor(0, 0);
      lcd.print("Conectando ");
      lcd.setCursor(0, 1);
      lcd.print("Tentativa: ");
      lcd.setCursor(12, 1);
      lcd.print(cont);
      delay(1000);

    }

    WiFi.setAutoReconnect(true); // Habilitar reconexão automática
    WiFi.persistent(true);       // Salvar configuração na memória
    Serial.println();
    Serial.println("WiFi conectado. ");
    Serial.print("WiFi SSID: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP Modulo: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Conectado");
    lcd.setCursor(0, 1);
    lcd.print("IP");
    lcd.setCursor(3, 1);
    lcd.print(WiFi.localIP());
    delay(3000);
    digitalWrite(led, LOW);

    // Inicializa o ThingSpeak
    ThingSpeak.begin(client);

}



void loop() {
  unsigned long currentTime = millis();
  digitalWrite(led, HIGH);             // Nivel lógico alto no pino I/O 2  
  delay(100);                          // Atraso de 100 ms 
  digitalWrite(led, LOW);              // Nivel lógico baixo no pino I/O 2  
  delay(100);                          // Atraso de 100 ms 
 
  // lcd.clear();
  // lcd.backlight();
  // lcd.setCursor(0, 0);
  // lcd.print("Lab 6_1: ");
  // lcd.setCursor(0, 1);
  // lcd.print("Sensor MQ4");


  //int leitura = analogRead(34);         // Lendo no pino GPIO34 
  float CO2 = analogRead(34);

  CO2Total -= CO2Readings[readCO2Index];
  CO2Readings[readCO2Index] = CO2;
  CO2Total += CO2;
  readCO2Index = (readCO2Index + 1) % numCO2Readings;  // Atualizar o índice
  float CO2Media = CO2Total / numCO2Readings;

  CO2Media = CO2Media * 5,67;

  if (CO2Media > 1000) {
    digitalWrite(buzzerCO2, HIGH);
  }
  else{
    digitalWrite(buzzerCO2, LOW);
  }


  Serial.print("Leitura Sensor: ");
  Serial.print(CO2Media);
  Serial.println(" ");


  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("CO2: ");
  lcd.setCursor(0, 1);
  lcd.print(CO2Media);

  //int leitura = analogRead(36);         // Lendo no pino GPIO34 
  float CO = analogRead(36);

  COTotal -= COReadings[readCOIndex];
  COReadings[readCOIndex] = CO;
  COTotal += CO;
  readCOIndex = (readCOIndex + 1) % numCOReadings;  // Atualizar o índice
  float COMedia = COTotal / numCOReadings;

  COMedia = COMedia /530;


  if (COMedia > 9) {
    digitalWrite(buzzerCO, HIGH);
  }
  else{
    digitalWrite(buzzerCO, LOW);
  }


  Serial.print("Leitura Sensor: ");
  Serial.print(COMedia);
  Serial.println(" ");



  lcd.backlight();
  lcd.setCursor(8, 0);
  lcd.print("CO: ");
  lcd.setCursor(8, 1);
  lcd.print(COMedia);


    // Verifica se o intervalo de 30 segundos passou
    if (currentTime - lastTime >= interval) {
        lastTime = currentTime; // Atualiza o tempo da última execução


  
  ThingSpeak.setField(1, COMedia);
  ThingSpeak.setField(2, CO2Media);


  int status = ThingSpeak.writeFields(channelID, apiKey);
  //int status = ThingSpeak.writeField(channelID, 1, dB, apiKey);
  if (status == 200) 
  {
    Serial.println("Dado enviado com sucesso!");
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Dado Enviado ");
    lcd.setCursor(0, 1);
    lcd.print("OK ");
    delay(1000);
  } 
  
  else 
    {
      Serial.print("Erro ao enviar dado: ");
      Serial.println(status);
      lcd.clear();
      lcd.backlight();
      lcd.setCursor(0, 0);
      lcd.print("Falha no Envio ");
      lcd.setCursor(0, 1);
      lcd.print(" ");
      delay(1000);
    }
  }



}



