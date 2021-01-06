#include "Sensor.h"
#include "Higrometro.h"
#include "FotoResistencia.h"
#include "SDHT.h"
#include "DHT.h"
#include "Actuador.h"
#include "Ultrasonico.h"
#include <ArduinoMqttClient.h>
#include <WiFi.h>


#define DHTTYPE DHT22
#define DHTTYPE11 DHT11

//Configuración wifi
/*char ssid[] = "TP-LINK_9942";        // your network SSID (name)
char pass[] = "58912485";    // your network password (use for WPA, or use as key for WEP)*/
char ssid[] = "Onefy_RENGAR";        // your network SSID (name)
char pass[] = "04101973";    // your network password (use for WPA, or use as key for WEP)

//Configuración MQTT
const char broker[]    = "broker.hivemq.com";
int        port        = 1883;
const char willTopic[] = "infinity/senyal/will";
const char operacionesTopic[]   = "infinity/senyal/operaciones";
const char datosTopic[]  = "infinity/senyal/lecturaDatos";
const char datosTopic1[]  = "infinity/senyal/lecturaDatos1";
const char datosTopic2[]  = "infinity/senyal/lecturaDatos2";

//PinActuadores
int pinActuadorLucesTapa = 12;
int pinActuadorLucesPuerta = 13;

//Declaración objetos
Higrometro sh;
FotoResistencia sl;
SDHT sht;
SDHT shtnv1(0,33,25);
Actuador lucesTapa(pinActuadorLucesTapa);
Actuador lucesPuerta(pinActuadorLucesPuerta);
Ultrasonico spres;
DHT dht(sht.getPinLectura(), DHTTYPE);
DHT dhtnv1(shtnv1.getPinLectura(),DHTTYPE11);
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

 
void setup() {
  
  Serial.begin(115200);
  dht.begin();
  dhtnv1.begin();
  //Configuración pines de los actuadores
  pinMode(sh.getPinActuador(), OUTPUT);
  pinMode(lucesTapa.getPinActuador(), OUTPUT);
  pinMode(sht.getPinActuador(),OUTPUT);
  pinMode(lucesPuerta.getPinActuador(), OUTPUT);

  //Presencia
  pinMode(spres.getTriggerPin(), OUTPUT);
  pinMode(spres.getPinEcho(), INPUT);


  //Wifi
  Serial.print("Intentando conectar a la red con el SSID: ");
  Serial.println(ssid);
  
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
      // failed, retry
      Serial.print(".");
      delay(2000);
  }
    
  Serial.println("You're connected to the network");
  Serial.println();


  //MQTT
  String willPayload = "oh no!";
  bool willRetain = true;
  int willQos = 1;

  mqttClient.beginWill(willTopic, willPayload.length(), willRetain, willQos);
  mqttClient.print(willPayload);
  mqttClient.endWill();

  Serial.print("Conectando al MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("La conexion a fallado! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("Estas conectado al MQTT broker!");
  Serial.println();

  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);

  Serial.print("Suscribiendote al topic: ");
  Serial.println(datosTopic);
  Serial.println();

  // subscribe to a topic
  // the second parameter set's the QoS of the subscription,
  // the the library supports subscribing at QoS 0, 1, or 2
  int subscribeQos = 1;

  mqttClient.subscribe(operacionesTopic, subscribeQos);
  
}

void loop() {

  bool retained = false;
  
  int qos = 1;
  bool dup = false;
  String payload = "i7oLSgyX0kx6c75oMKBP";
  String payload1 = "i7oLSgyX0kx6c75oMKBP";
  String payload2 = "i7oLSgyX0kx6c75oMKBP";
  mqttClient.poll();
   
  int16_t humedad = sh.tomarLectura();
  int16_t luminosidad = sl.tomarLectura();
  
  float humedadAmbiente = leerHumedadAmbiente();
  float humedadAmbienteN1 = leerHumedadAmbienteN1();
  float mediaHumedadaAmbiente = calcularMedia(humedadAmbiente,humedadAmbienteN1);
  
  float temperaturaAmbiente = leerTemperaturaAmbiente();
  float temperaturaAmbienteN1 = leerTemperaturaAmbienteN1();
  float mediaTemperaturaAmbiente = calcularMedia(temperaturaAmbiente,temperaturaAmbienteN1);

  int distanciaLeida = spres.distancia();
  

            
              Serial.println("<----- Humedad ----->");
              Serial.print("Humedad en la unidad 1 (%): ");
              //Serial.println(sh.getVoltajeHumedadLeido());
              Serial.print(humedad);
              if(spres.hayPresencia()){
                sh.comprobarHumedad(humedad);
               }
              Serial.println("%");

              
              Serial.print("Humedad ambiente en el nivel 1(%):");
              Serial.print(humedadAmbienteN1);
              Serial.println("%");
              
              Serial.print("Humedad ambiente en el nivel 2(%):");
              Serial.print(humedadAmbiente);
              Serial.println("%");

              Serial.print("Humedad ambiente en la maquina:");
              Serial.print(mediaHumedadaAmbiente);
              Serial.println("%");
              
              Serial.println("<----- Luminosidad ----->");
              Serial.print("Valor Luminosidad leido: ");
              Serial.print(luminosidad);
              Serial.println(" Estado luminosidad "+sl.estadoLuminosidad(luminosidad));
             
            
              Serial.println("<----- Temperatura (Cº) ----->");    
              Serial.print("Valor temperatura leida en el Nivel 1: ");
              Serial.print(temperaturaAmbienteN1);
              Serial.println(" Cº");     
                
              Serial.print("Valor temperatura leida en el Nivel 2: ");
              Serial.print(temperaturaAmbiente);
              Serial.println(" Cº");

              Serial.print("Valor temperatura ambiente en la maquina: ");
              Serial.print(mediaTemperaturaAmbiente);
              Serial.println(" Cº");

              Serial.println(" ");
             
              sht.comprobarTemperatura(temperaturaAmbiente);

              Serial.println("<----- Distancia  ----->");
              Serial.println(distanciaLeida);
              Serial.println(" ");
              Serial.println(" ");
              
              payload += "-";
              payload += String(humedad);
              payload += "-";
              payload += String(luminosidad);
              payload += "-";
              payload += String(mediaHumedadaAmbiente,2);
              payload += "-";
              payload += String(mediaTemperaturaAmbiente,2);
              
              
              
              payload1 += "-";
              payload1 += String(humedadAmbienteN1,2);
              payload1 += "-";
              payload1 += String(temperaturaAmbienteN1,2);

              payload2 += "-";
              payload2 += String(humedadAmbiente,2);              
              payload2 += "-";
              payload2 += String(temperaturaAmbiente,2);
              
              mqttClient.beginMessage(datosTopic, payload.length(), retained, qos, dup);
              mqttClient.print(payload);
              mqttClient.endMessage();

              mqttClient.beginMessage(datosTopic1, payload1.length(), retained, qos, dup);
              mqttClient.print(payload1);
              mqttClient.endMessage();

              mqttClient.beginMessage(datosTopic2, payload2.length(), retained, qos, dup);
              mqttClient.print(payload2);
              mqttClient.endMessage();
              
              delay(2500);
            

            
  
}

 float leerHumedadAmbiente(){
      float humedadAmbiente = dht.readHumidity();

      if (isnan(humedadAmbiente)) {
          Serial.println(F("Error al leer humedad!"));
             return -1;
      }else{
        return humedadAmbiente;
      }
            
}
          
float leerTemperaturaAmbiente(){
     float temperatura = dht.readTemperature();

    if (isnan(temperatura)) {
       Serial.println(F("Error al leer temperatura!"));
       return -1;
    }else{
      return temperatura;
    }
            
}

 float leerHumedadAmbienteN1(){
      float humedadAmbiente = dhtnv1.readHumidity();

      if (isnan(humedadAmbiente)) {
          Serial.println(F("Error al leer con el sensor DHT11humedad!"));
             return -1;
      }else{
        return humedadAmbiente;
      }
            
}
          
float leerTemperaturaAmbienteN1(){
     float temperatura = dhtnv1.readTemperature();

    if (isnan(temperatura)) {
       Serial.println(F("Error al leer con el sensor DHT11 temperatura!"));
       return -1;
    }else{
      return temperatura;
    }
            
}

float calcularMedia(float medida1, float medida2){
     float media = (medida1+medida2)/2;
     return media;
            
}


void onMqttMessage(int messageSize) {
  String mensajeLeido = "";
  char sz[] = "12345678";
  char buf[sizeof(sz)];
  String delimitador = "-";
  int operacion = 0;
  String valorOperacion = "";
  int nOperacion = 0;

  
  // we received a message, print out the topic and contents
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', duplicate = ");
  Serial.print(mqttClient.messageDup() ? "true" : "false");
  Serial.print(", QoS = ");
  Serial.print(mqttClient.messageQoS());
  Serial.print(", retained = ");
  Serial.print(mqttClient.messageRetain() ? "true" : "false");
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    mensajeLeido += (char)mqttClient.read();
  }
  
  //Serial.println(mensajeLeido);
  
    mensajeLeido.toCharArray(buf, sizeof(buf));
    char *p = buf;
    char *str;
    while ((str = strtok_r(p, "-", &p)) != NULL) // delimiter is the semicolon
      if(nOperacion == 0){
        operacion = atoi(str);
        nOperacion++;
      }else if(nOperacion == 1){
        valorOperacion = str;
      }
      
  

  switch (operacion)  {
    case 1:
            Serial.println("Operar con las luces de la parte superior");
             if(valorOperacion == "OFF"){
              Serial.print("Apagando luces de la parte superior...");
              lucesTapa.cambiarEstadoActuador(true);
              
            }else if(valorOperacion == "ON"){
              Serial.print("Encendiendo luces de la parte superior...");
              lucesTapa.cambiarEstadoActuador(false); 
            }
        break;

    case 2:
          Serial.println("Operar con las luces de una de las puertas");
           if(valorOperacion == "ON"){
            Serial.print("Encendiendo luces de la puerta...");
            lucesPuerta.cambiarEstadoActuador(true); 
          }else if(valorOperacion == "OFF"){
            Serial.print("Apagando luces de la puerta...");
            lucesPuerta.cambiarEstadoActuador(false);  
          }
        break;
        
      case 3:
          Serial.println("Operar cambiando el humbral de temperatura a la que se activan los ventiladores");
           sht.setHumbralAlerta(valorOperacion.toFloat());
           Serial.println(sht.getHumbralAlerta());
        break;
    default:
        Serial.println("Operación desconocida");
  }

  /*if(mensajeLeido == "Superior OFF"){
    Serial.print("Apagando luces de la parte superior...");
    lucesTapa.cambiarEstadoActuador(true);
    
  }else if(mensajeLeido == "Superior ON"){
    Serial.print("Encendiendo luces de la parte superior...");
    lucesTapa.cambiarEstadoActuador(false); 
  }else if(mensajeLeido == "ON"){
    Serial.print("Encendiendo luces de la puerta...");
    lucesPuerta.cambiarEstadoActuador(true); 
  }else if(mensajeLeido == "OFF"){
    Serial.print("Apagando luces de la puerta...");
    lucesPuerta.cambiarEstadoActuador(false);
    
  }*/
  
  Serial.println();
  Serial.println();


}
