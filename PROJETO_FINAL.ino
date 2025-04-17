#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>
//---------------------------------WIFI---------------------------------//
  const char* SSID = "Wi-fi";  // SSID / nome da rede WiFi que deseja se conectar 
  const char* PASSWORD = "123password";    // Senha da rede WiFi que deseja se conectar 
  WiFiClient wifiClient;

//---------------------------------MQTT SERVER---------------------------------//
  const char* BROKER_MQTT = "mqtt.eclipseprojects.io";  //URL do broker MQTT que se deseja utilizar
  int BROKER_PORT = 1883;                               // Porta do Broker MQTT

////---------------------------------#DEFINE DO MQTT---------------------------------//
  #define ID_MQTT "esp"             // id único do esp
  #define TOPIC_PUBLISH_CARSENS "esp/faixa/carro"  // tópico onde será enviado o valor do sensor da entrada  
  PubSubClient MQTT(wifiClient);    // Instancia o Cliente MQTT passando o objeto EspClient
  
//---------------------------------DECLARAÇÃO DAS FUNÇÕES---------------------------------//
  void mantemConexoes();  //Garante que as conexoes com WiFi e MQTT Broker se mantenham ativas
  void conectaWiFi();     //Faz conexão com WiFi
  void conectaMQTT();     //Faz conexão com Broker MQTT
  void enviaPacotes();    // envia os valores para o python no pc

Servo servo;

int LED_VER = 27;
int LED_AM = 26;
int LED_VERM = 25;
int LED_PVERM = 19;
int LED_PVER = 18;

int SENSOR1 = 32; 
int SENSOR2 = 17;
int SENSOR3 = 34; 
int SENSOR4 = 35; 
int LDR = 15;

int n = 0;
int oldValue = 1;
int oldValue1 = 1;

int valorSENSOR1;
int valorSENSOR2;

void setup() {
  Serial.begin(9600);
  servo.attach(33); 
  pinMode(LED_VER, OUTPUT);
  pinMode(LED_AM, OUTPUT);
  pinMode(LED_VERM, OUTPUT);
  pinMode(LED_PVERM, OUTPUT);
  pinMode(LED_PVER, OUTPUT);
  pinMode(SENSOR1, INPUT);
  pinMode(SENSOR2, INPUT);
  pinMode(SENSOR3, INPUT);
  pinMode(SENSOR4, INPUT);
  pinMode(LDR, INPUT_PULLUP);
  
  conectaWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
}

void loop() {
  mantemConexoes();
  enviaPacotes();
  MQTT.loop();
  faixa();
}

void faixa(){
  int leituraS1 = digitalRead(SENSOR1);
  int leituraS2 = digitalRead(SENSOR2);
  digitalWrite(LED_VER, LOW);
  digitalWrite(LED_AM, LOW);
  digitalWrite(LED_VERM, HIGH);
  digitalWrite(LED_PVER, HIGH);
  digitalWrite(LED_PVERM, LOW);
  Serial.print("Numero de pedestres: ");
  Serial.print(n);
  Serial.print(" || LDR: ");
  Serial.println(analogRead(LDR));
  if(analogRead(LDR) > 10){
    if (leituraS1 == 0 || leituraS2 == 0 ){
      servo.write(0);   
    }        
    else {
      servo.write(90);
    }    
  }
  else{
    servo.write(0);
  }
  for(int i = 0; i < 70; i++){
    MQTT.loop();
    mantemConexoes();
    enviaPacotes();
    if (digitalRead(SENSOR3) == 0 && oldValue == 1) {      
      oldValue = 0;
      Count();
    }
    else if (digitalRead(SENSOR3) == 1 && oldValue == 0) {
      oldValue = 1;
    }
    if (digitalRead(SENSOR4) == 0 && oldValue1 == 1) {
      oldValue1 = 0;
      removeCount();
    }
    else if (digitalRead(SENSOR4) == 1 && oldValue1 == 0) {
      oldValue1 = 1;
    }  
    Serial.print("Numero de pedestres: ");
    Serial.print(n);       
    Serial.print(" || LDR: ");
    Serial.println(analogRead(LDR));
    delay(100);
   }  

  digitalWrite(LED_VER, HIGH);
  digitalWrite(LED_AM, LOW);
  digitalWrite(LED_VERM, LOW);
  digitalWrite(LED_PVER, LOW);
  digitalWrite(LED_PVERM, HIGH);

  for(int i = 0; i < 70; i++){ // 1s = 10  
    MQTT.loop();
    mantemConexoes();
    if(n >= 1){
      if (digitalRead(SENSOR3) == 0 && oldValue == 1) {
        oldValue = 0;
        Count();
      }
      else if (digitalRead(SENSOR3) == 1 && oldValue == 0) {
        oldValue = 1;
      }
      if (digitalRead(SENSOR4) == 0 && oldValue1 == 1) {
        oldValue1 = 0;
        removeCount();
      } else if (digitalRead(SENSOR4) == 1 && oldValue1 == 0) {
        oldValue1 = 1;
      }     
    }    
    if(analogRead(LDR) > 10){
     if(n >= 1){
       servo.write(90);
     }
     if(n == 0){
       servo.write(0);
     }
    }
    
    else{
       servo.write(0);
    }
  
    Serial.print("Numero de pedestres: ");
    Serial.print(n);    
    Serial.print(" || LDR: ");
    Serial.println(analogRead(LDR));
    delay(100);  
  }
  
  digitalWrite(LED_VER, LOW);
  digitalWrite(LED_AM, HIGH);
  digitalWrite(LED_VERM, LOW);
  Serial.print("Numero de pedestres: ");
  Serial.print(n);      
  Serial.print(" || LDR: ");
  Serial.println(analogRead(LDR));  
  delay(3000);
}

void mantemConexoes() {
  if (!MQTT.connected()) {
    conectaMQTT();
  }
  conectaWiFi();  //se não há conexão com o WiFI, a conexão é refeita
}

void conectaWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }
  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println("  Aguarde!");
  Serial.println();

  WiFi.begin(SSID, PASSWORD);  // Conecta na rede WI-FI
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(SSID);
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP());
}

void conectaMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Conectando ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado ao Broker com sucesso!");      
    } else {
      Serial.println("Não foi possivel se conectar ao broker.");
      Serial.println("Nova tentatica de conexao em 10s");
      delay(10000);
    }
  }
}

void enviaPacotes() {  
  valorSENSOR1 = digitalRead(SENSOR1);
  valorSENSOR2 = digitalRead(SENSOR2);
  if(valorSENSOR1 == false || valorSENSOR2 == false){
    MQTT.publish(TOPIC_PUBLISH_CARSENS, "1"); 
    Serial.println("Sensor do carro foi acionado, enviando sinal.");
  }  
  else{
    MQTT.publish(TOPIC_PUBLISH_CARSENS, "0");
    Serial.println("NÃO ENVIA SINAL");
  }
}

void Count() {
  n = n + 1;
}

void removeCount() {
  if(n > 0){
    n = n - 1;    
  }  
}

void Reset() {
  n = 0;
}