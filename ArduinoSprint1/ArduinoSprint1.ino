#include "Higrometro.h"
#include "FotoDiodo.h"
#include "SDHT11.h"
#include "DHT.h"
#define DHTTYPE DHT11

#include <ArduinoMqttClient.h>
#include <WiFi.h>


char ssid[] = "GUCCI Note";        // your network SSID (name)
char pass[] = "12345678";    // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);


const char broker[]    = "mqtt.eclipse.org";
int        port        = 1883;
const char willTopic[] = "infinity/senyal/will";
const char lucesTopic[]   = "infinity/senyal/luces";
const char ventiladoresTopic[]   = "infinity/senyal/ventiladores";
const char riegoTopic[]   = "infinity/senyal/riego";
const char datosTopic[]  = "infinity/senyal/lecturaDatos";

Higrometro sh;
FotoDiodo sl;
SDHT11 sht;
DHT dht(sht.getPinLectura(), DHTTYPE);



void setup() {
  
  Serial.begin(115200);
  Serial.println("Inicializando sensores ...");
  dht.begin();


  
  pinMode(sh.getPinActuador(), OUTPUT);
  pinMode(sl.getPinActuador(), OUTPUT);
  pinMode(sht.getPinActuador(),OUTPUT);


  //Wifi
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
    while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
      // failed, retry
      Serial.print(".");
      delay(5000);
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
  mqttClient.subscribe(ventiladoresTopic, subscribeQos);
  mqttClient.subscribe(riegoTopic, subscribeQos);
  
}

void loop() {

  bool retained = false;
  
  int qos = 1;
  bool dup = false;
  String payload = "";
  mqttClient.poll();
   
  int16_t humedad = sh.leerHumedad();
  int16_t luminosidad = sl.leerLuminosidad();
  float humedadAmbiente = leerHumedadAmbiente();
  float temperaturaAmbiente = leerTemperaturaAmbiente();


            
              Serial.print("Valor humedad leido: ");
              //Serial.println(sh.getVoltajeHumedadLeido());
              Serial.print("HU - Humedad en una Unidad (%): ");
              Serial.print(humedad);
              sh.comprobarHumedad(humedad);
              Serial.print("%");
              Serial.print(" HRA - Humedad Ambiente (%):");
              Serial.print(humedadAmbiente);
              Serial.println("%");
              
              Serial.print("Valor Luminosidad leido: ");
              Serial.print(luminosidad);
              Serial.println(" Estado luminosidad "+sl.estadoLuminosidad(luminosidad));
             
            
              Serial.print("Valor temperatura leido: ");
              Serial.print(temperaturaAmbiente);
              Serial.println(" Cº");
              sht.comprobarTemperatura(temperaturaAmbiente);
             

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

  if(cadena == "luces OFF"){
    Serial.print("Apagando luces...");
    sl.actuarSobreLuces(true);
    
  }else if(cadena == "luces ON"){
    Serial.print("Encendiendo luces...");
    sl.actuarSobreLuces(false);
    
  }
  
  Serial.println();

  Serial.println();


}
