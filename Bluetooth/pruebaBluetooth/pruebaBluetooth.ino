//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
#include <WiFi.h>

BluetoothSerial SerialBT;
WiFiClient wifiClient;

int eeSSID = 0;
int eePassword = 1;
char ssid[50] = "";
char pass[50] = ""; 
int indice = 0;
int indice2 = 0;
const char msg[50] = "Introduce el ssid de tu red";
const char msg2[50] = "Introduce tu contrasenña";
int nIntentos = 0;



void setup() {
  Serial.begin(115200);
  SerialBT.begin("Infinitycrop"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {
  if(String(ssid).equals("")){

    for (int i = 0; i < 50; i++)
        {
            SerialBT.write(msg[i]);
        }
    
      while (SerialBT.available()) {
        ssid[indice] = SerialBT.read();
        indice++; 
      }
    }
   
  Serial.write(ssid);
    
  
  if (!String(ssid).equals("")) {
    for (int i = 0; i < 50; i++)
        {
            SerialBT.write(msg2[i]);
        }
      while (SerialBT.available()) {
        pass[indice2] = SerialBT.read();
        indice2++; 
      }  


        for (int i = 0; i < 50; i++)
        {
            SerialBT.write(ssid[i]);
        }
      Serial.write(pass);
  }
  if(!String(ssid).equals("")&&!String(pass).equals("")){
      Serial.print("Intentando conectar a la red con el SSID: ");
      Serial.println(ssid);
      Serial.println(pass);

      
      WiFi.begin(ssid, pass);

      Serial.println(WiFi.status());
      
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
           
        }
       
    
        
     Serial.println("You're connected to the network");
     Serial.println();
    }
  
  delay(5000);
}
