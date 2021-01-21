#include "Sensor.h"
#include "Higrometro.h"
#include "FotoResistencia.h"
#include "SDHT.h"
#include "DHT.h"
#include "Actuador.h"
#include "Ultrasonico.h"
#include <ArduinoMqttClient.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

#define DHTTYPE DHT22
#define DHTTYPE11 DHT11

#define FIREBASE_HOST "https://infinity-crop-cc8b9.firebaseio.com"
#define FIREBASE_AUTH "rnkvZtImIx3hSjM915QwP7Yr5lJbQlWYwuAE53Zk"

//Configuración wifi
/*char ssid[] = "MiFibra-397F";        // your network SSID (name)
char pass[] = "oXr6gkNe";    // your network password (use for WPA, or use as key for WEP)*/
char ssid[] = "GUCCI Note";        // your network SSID (name)
char pass[] = "12345678";    // your network password (use for WPA, or use as key for WEP)
/*char ssid[] = "GTI-2020-2A-2-2";        // your network SSID (name)
char pass[] = "58912485";    // your network password (use for WPA, or use as key for WEP)*/

//Define FirebaseESP32 data object and variables
FirebaseData fbdo;



String machineID = "/infinitycrop18012021";
String pathG = "/Mediciones general";
String path1 = "/Mediciones nivel 1";
String path2 = "/Mediciones nivel 2";


//Configuración MQTT
const char broker[]    = "broker.hivemq.com";
int        port        = 1883;
const char willTopic[] = "infinity/senyal/will";
const char operacionesTopic[]   = "infinity/senyal/operaciones-infinitycrop18012021";
const char debugtopic[]  = "infinity/senyal/debug-infinitycrop18012021";

//PinActuadores
int pinActuadorLucesTapa = 12;
int pinActuadorLucesPuerta = 13;
int pinActuadorLuzP1 = 14;
int pinActuadorLuzP2 = 22;
int pinActuadorLuzP3 = 23;

//Declaración objetos
Higrometro sh;
FotoResistencia sl;
SDHT sht;
SDHT shtnv1(0,33,16);
Ultrasonico spres;

Actuador lucesTapa(pinActuadorLucesTapa);
Actuador lucesPuerta(pinActuadorLucesPuerta);

Actuador luzP1(pinActuadorLuzP1);
Actuador luzP2(pinActuadorLuzP2);
Actuador luzP3(pinActuadorLuzP3);

DHT dht(sht.getPinLectura(), DHTTYPE);
DHT dhtnv1(shtnv1.getPinLectura(),DHTTYPE11);
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);


void printResult(FirebaseData &data);

void setup() {
  
  Serial.begin(115200);
  dht.begin();
  dhtnv1.begin();
  //Configuración pines de los actuadores
  pinMode(sh.getPinActuador(), OUTPUT);
  pinMode(lucesTapa.getPinActuador(), OUTPUT);
  pinMode(sht.getPinActuador(),OUTPUT);
  pinMode(lucesPuerta.getPinActuador(), OUTPUT);
  pinMode(luzP1.getPinActuador(), OUTPUT);
  pinMode(luzP2.getPinActuador(), OUTPUT);
  pinMode(luzP3.getPinActuador(), OUTPUT);

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

  //Firebase ESP32
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(fbdo, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(fbdo, "tiny");

  //optional, set the decimal places for float and double data to be stored in database
  Firebase.setFloatDigits(2);
  Firebase.setDoubleDigits(2);

  
  //MQTT
  String willPayload = "Infinity crop!";
  bool willRetain = true;
  int willQos = 1;

  mqttClient.beginWill(willTopic, willPayload.length(), willRetain, willQos);
  mqttClient.print(willPayload);
  mqttClient.endWill();

  Serial.print("Conectando al MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("La conexión a fallado! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("Estas conectado al MQTT broker!");
  Serial.println();

  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);



  // subscribe to a topic
  // the second parameter set's the QoS of the subscription,
  // the the library supports subscribing at QoS 0, 1, or 2
  int subscribeQos = 1;

  mqttClient.subscribe(operacionesTopic, subscribeQos);
  
}

void loop() {

  mqttClient.poll();


  //Mediciones de los sensores
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



    Firebase.setDouble(fbdo, pathG + machineID+"/MediaHumedad", mediaHumedadaAmbiente);
    Firebase.setDouble(fbdo, pathG + machineID+"/MediaTemperatura", mediaTemperaturaAmbiente);
    Firebase.setDouble(fbdo, pathG + machineID+"/Luminosidad", luminosidad);
    Firebase.setDouble(fbdo, pathG + machineID+"/LuzTecho", digitalRead(lucesTapa.getPinActuador()));
    Firebase.setDouble(fbdo, pathG + machineID+"/LuzPuerta", digitalRead(lucesPuerta.getPinActuador()));
    Firebase.setDouble(fbdo, pathG + machineID+"/LuzPieza1", digitalRead(luzP1.getPinActuador()));
    Firebase.setDouble(fbdo, pathG + machineID+"/LuzPieza2", digitalRead(luzP2.getPinActuador()));
    Firebase.setDouble(fbdo, pathG + machineID+"/LuzPieza3", digitalRead(luzP3.getPinActuador()));

  
    Firebase.setDouble(fbdo, path1 + machineID+"/Humedad", humedad);
    Firebase.setDouble(fbdo, path1 + machineID+"/HumedadAmbiente", humedadAmbienteN1);
    Firebase.setDouble(fbdo, path1 + machineID+"/TemperaturaAmbiente", temperaturaAmbienteN1);
    Firebase.setDouble(fbdo, path1 + machineID+"/Presencia1", 0);
    Firebase.setDouble(fbdo, path1 + machineID+"/Presencia2", 0);
    Firebase.setDouble(fbdo, path1 + machineID+"/Presencia3", 0);

  
    Firebase.setDouble(fbdo, path2 + machineID+"/HumedadAmbiente", humedadAmbiente);
    Firebase.setDouble(fbdo, path2 + machineID+"/TemperaturaAmbiente", temperaturaAmbiente);
    Firebase.setDouble(fbdo, path2 + machineID+"/Presencia1", spres.hayPresencia());
    Firebase.setDouble(fbdo, path2 + machineID+"/Presencia2", 0);
    Firebase.setDouble(fbdo, path2 + machineID+"/Presencia3", 0);

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

  /*Modo debug MQTT*/
  String payload = "";
 /* mqttClient.poll();*/

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
      
       Serial.println("Mensaje recibido: ");
       Serial.print(mensajeLeido);
        switch (operacion)  {
        case 1:
                Serial.println("Operar con todas las luces");
                payload += " "+mensajeLeido+" ";
                 if(valorOperacion == "OFF"){
                  Serial.print("Apagando todas luces...");
                  lucesTapa.cambiarEstadoActuador(true);
                  lucesPuerta.cambiarEstadoActuador(true);
                  luzP1.cambiarEstadoActuador(true);
                  luzP2.cambiarEstadoActuador(true);
                  luzP3.cambiarEstadoActuador(true);
                  payload += "DENTRO LUCES GENERAL OFF";
                  
                }else if(valorOperacion == "ON"){
                  Serial.print("Encendiendo todas las luces ...");
                  lucesTapa.cambiarEstadoActuador(false);
                  lucesPuerta.cambiarEstadoActuador(false);
                  luzP1.cambiarEstadoActuador(false);
                  luzP2.cambiarEstadoActuador(false);
                  luzP3.cambiarEstadoActuador(false);
                  payload += "DENTRO LUCES GENERAL ON"; 
                }
            break;
    
        case 2:
              Serial.println("Operar con las luces del nivel 2");
              payload += " "+mensajeLeido+" ";
               if(valorOperacion == "ON"){
                Serial.print("Encendiendo luces del nivel 2...");
                lucesTapa.cambiarEstadoActuador(false);
                luzP1.cambiarEstadoActuador(false);
                luzP2.cambiarEstadoActuador(false);
                payload += "DENTRO LUCES NIVEL 2 ON"; 
              }else if(valorOperacion == "OFF"){
                Serial.print("Apagando luces del nivel 2...");
                lucesTapa.cambiarEstadoActuador(true);
                luzP1.cambiarEstadoActuador(true);
                luzP2.cambiarEstadoActuador(true);  
                payload += "DENTRO LUCES NIVEL 2 OFF";
              }
            break;
    
       case 3:
              Serial.println("Operar con las luces del nivel 1");
              payload += " "+mensajeLeido+" ";
               if(valorOperacion == "ON"){
                Serial.print("Encendiendo luces del nivel 1...");
                lucesPuerta.cambiarEstadoActuador(false);
                luzP3.cambiarEstadoActuador(false);
                payload += "DENTRO LUCES NIVEL 1 ON"; 
              }else if(valorOperacion == "OFF"){
                Serial.print("Apagando luces del nivel 1...");
                lucesPuerta.cambiarEstadoActuador(true);
                luzP3.cambiarEstadoActuador(true);  
                payload += "DENTRO LUCES NIVEL 1 OFF";
              }
            break;
            
          case 4:
              Serial.println("Operar cambiando el humbral de humedad a la que se activan la electrovalvula");
              payload += " "+mensajeLeido+" ";
              sh.setHumbralAlerta(valorOperacion.toFloat());
              Serial.println(sh.getHumbralAlerta());
              payload += "DENTRO EDITAR HUMBRAL HUMEDAD";
            break;
            
         case 5:
              Serial.println("Operar cambiando el humbral de temperatura a la que se activan los ventiladores");
              payload += " "+mensajeLeido+" ";
              sht.setHumbralAlerta(valorOperacion.toFloat());
              shtnv1.setHumbralAlerta(valorOperacion.toFloat());
              Serial.println(sht.getHumbralAlerta());
              Serial.println(shtnv1.getHumbralAlerta());
              payload += "DENTRO EDITAR HUMBRAL TEMPERATURA";
            break;
            
        default:
            Serial.println("Operación desconocida");
            payload += "OPERACIÓN DESCONOCIDA";
            break;
      }
    
      mqttClient.beginMessage(debugtopic, payload.length(), false, 1, false);
      mqttClient.print(payload);
      mqttClient.endMessage();
      
      Serial.println();
      Serial.println();


}

void printResult(FirebaseData &data)
{

  if (data.dataType() == "int")
    Serial.println(data.intData());
  else if (data.dataType() == "float")
    Serial.println(data.floatData(), 5);
  else if (data.dataType() == "double")
    printf("%.9lf\n", data.doubleData());
  else if (data.dataType() == "boolean")
    Serial.println(data.boolData() == 1 ? "true" : "false");
  else if (data.dataType() == "string")
    Serial.println(data.stringData());
  else
  {
    Serial.println(data.payload());
  }
}
