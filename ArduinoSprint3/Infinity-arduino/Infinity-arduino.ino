#include "Sensor.h"
#include "Higrometro.h"
#include "FotoResistencia.h"
#include "SDHT.h"
#include "DHT.h"
#include "Actuador.h"
#include <ArduinoMqttClient.h>
#include <WiFi.h>

#define DHTTYPE DHT22


//Configuración wifi
char ssid[] = "MiFibra-397F";        // your network SSID (name)
char pass[] = "oXr6gkNe";    // your network password (use for WPA, or use as key for WEP)

//Configuración MQTT
const char broker[]    = "broker.hivemq.com";
int        port        = 1883;
const char willTopic[] = "infinity/senyal/will";
const char lucesTopic[]   = "infinity/senyal/luces";
const char luzPuertaTopic[]   = "infinity/senyal/luzPuerta/POWER";
/*const char ventiladoresTopic[]   = "infinity/senyal/ventiladores";
const char riegoTopic[]   = "infinity/senyal/riego";*/
const char datosTopic[]  = "infinity/senyal/lecturaDatos";

//PinActuadores
int pinActuadorLucesTapa = 12;
int pinActuadorLucesPuerta = 13;

//Declaración objetos
Higrometro sh;
FotoResistencia sl;
SDHT sht;
Actuador lucesTapa(pinActuadorLucesTapa);
Actuador lucesPuerta(pinActuadorLucesPuerta);
DHT dht(sht.getPinLectura(), DHTTYPE);
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

 
void setup() {
  
  Serial.begin(115200);
  dht.begin();

  //Configuración pines de los actuadores
  pinMode(sh.getPinActuador(), OUTPUT);
  pinMode(lucesTapa.getPinActuador(), OUTPUT);
  pinMode(sht.getPinActuador(),OUTPUT);
  pinMode(lucesPuerta.getPinActuador(), OUTPUT);


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

  mqttClient.subscribe(lucesTopic, subscribeQos);
  mqttClient.subscribe(luzPuertaTopic, subscribeQos);
  /*mqttClient.subscribe(ventiladoresTopic, subscribeQos);
  mqttClient.subscribe(riegoTopic, subscribeQos);*/
  
}

void loop() {

  bool retained = false;
  
  int qos = 1;
  bool dup = false;
  String payload = "i7oLSgyX0kx6c75oMKBP";
  mqttClient.poll();
   
  int16_t humedad = sh.tomarLectura();
  int16_t luminosidad = sl.tomarLectura();
  float humedadAmbiente = leerHumedadAmbiente();
  float temperaturaAmbiente = leerTemperaturaAmbiente();


            
              Serial.println("<----- Humedad ----->");
              Serial.print("Humedad en la unidad 1 (%): ");
              //Serial.println(sh.getVoltajeHumedadLeido());
              Serial.print(humedad);
              sh.comprobarHumedad(humedad);
              Serial.println("%");

              
              Serial.print("Humedad ambiente en el nivel 2(%):");
              Serial.print(humedadAmbiente);
              Serial.println("%");
              
              Serial.println("<----- Luminosidad ----->");
              Serial.print("Valor Luminosidad leido: ");
              Serial.print(luminosidad);
              Serial.println(" Estado luminosidad "+sl.estadoLuminosidad(luminosidad));
             
            
              Serial.println("<----- Temperatura (Cº) ----->");            
              Serial.print("Valor temperatura leida en el Nivel 2: ");
              Serial.print(temperaturaAmbiente);
              Serial.println(" Cº");

              Serial.println(" ");
              Serial.println(" ");
              sht.comprobarTemperatura(temperaturaAmbiente);
             
              payload += "-";
              payload += String(humedad);
              payload += "-";
              payload += String(luminosidad);
              payload += "-";
              payload += String(humedadAmbiente,2);
              payload += "-";
              payload += String(temperaturaAmbiente,2);
              
              mqttClient.beginMessage(datosTopic, payload.length(), retained, qos, dup);
              mqttClient.print(payload);
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


void onMqttMessage(int messageSize) {
  String cadena = "";
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
    cadena += (char)mqttClient.read();
  }
  Serial.println(cadena);

  if(cadena == "Superior OFF"){
    Serial.print("Apagando luces de la parte superior...");
    lucesTapa.cambiarEstadoActuador(true);
    
  }else if(cadena == "Superior ON"){
    Serial.print("Encendiendo luces de la parte superior...");
    lucesTapa.cambiarEstadoActuador(false); 
  }else if(cadena == "ON"){
    Serial.print("Encendiendo luces de la puerta...");
    lucesPuerta.cambiarEstadoActuador(true); 
  }else if(cadena == "OFF"){
    Serial.print("Apagando luces de la puerta...");
    lucesPuerta.cambiarEstadoActuador(false);
    
  }
  
  Serial.println();
  Serial.println();


}
