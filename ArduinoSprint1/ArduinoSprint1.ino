#include "Higrometro.h"
#include "FotoDiodo.h"
#include "SDHT11.h"
#include "DHT.h"

#define DHTTYPE DHT11

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
  pinMode(sht.getPinActuador(), OUTPUT);
  
}

void loop() {
   
  int16_t humedad = sh.leerHumedad();
  int16_t luminosidad = sl.leerLuminosidad();
  float humedadAmbiente = leerHumedadAmbiente();
  float temperaturaAmbiente = leerTemperaturaAmbiente();


     if (Serial.available() > 0) {
      char command = (char) Serial.read(); 
      switch (command) {
         case 'H':
              Serial.print(" HRA - Humedad Ambiente (%):");
              Serial.print(humedadAmbiente);
              Serial.println("%");
              delay(1000);
            break;
         case 'D':
            Serial.print("Valor humedad leido: ");
            //Serial.println(sh.getVoltajeHumedadLeido());
            Serial.print("HU - Humedad en una Unidad (%): ");
            Serial.print(humedad);
            Serial.println("%");
          
            Serial.print("Valor Luminosidad leido: ");
            Serial.print(luminosidad);
            Serial.println(" Estado luminosidad "+sl.estadoLuminosidad(luminosidad));
            //Serial.println("LUX (%): ");
          
            Serial.print("Valor temperatura leido: ");
            Serial.print(temperaturaAmbiente);
            Serial.println(" Cº");
            delay(1000);
            break;
      }
      
   }else{
             
             Serial.print("Valor humedad leido: ");
            //Serial.println(sh.getVoltajeHumedadLeido());
            Serial.print("HU - Humedad en una Unidad (%): ");
            Serial.print(humedad);
            Serial.print("%");
            Serial.print(" HRA - Humedad Ambiente (%):");
            Serial.print(humedadAmbiente);
            Serial.println("%");
          
            Serial.print("Valor Luminosidad leido: ");
            Serial.print(luminosidad);
            Serial.println(" Estado luminosidad "+sl.estadoLuminosidad(luminosidad));
            //Serial.println("LUX (%): ");
          
            Serial.print("Valor temperatura leido: ");
            Serial.print(temperaturaAmbiente);
            Serial.println(" Cº");
            delay(1000);
     }
  
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
      sht.comprobarTemperatura(temperatura);
      return temperatura;
    }
            
}
