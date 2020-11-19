class FotoDiodo
  {  private:
         int pinActuador; //Este es el numero del pin al que se va a conectar el actuador, si no se tiene un actuador se recomienda usar un LED
         int pinLectura; //Este es el número del pin por el que se va a realizar la lectura del voltaje para calcular la humedad a la que se expone el higrometro
         int voltajeLuminosidadLeida;
         int humbralAlerta;
     public: 
          //Constructores
          FotoDiodo(){
             pinActuador = 12;
             pinLectura = 32;
             voltajeLuminosidadLeida = 0;
             humbralAlerta = 30;
          }
          
          FotoDiodo(int pinActuadorP, int pinLecturaP, int humbralAlertaP){
             pinActuador = pinActuadorP;
             pinLectura = pinLecturaP;
             voltajeLuminosidadLeida = 0;
             humbralAlerta = humbralAlertaP;
          }
          
          //Getters y Setters  
          void setPinActuador(int pinActuadorP){
              pinActuador = pinActuadorP;
           }
            
          int getPinActuador(){
                return (pinActuador);
          }
          
          void setPinLectura(int pinLecturaP){
              pinLectura = pinLecturaP;
           }
            
          int getPinLectura(){
                return (pinLectura);
          }
          
          int getVoltajeLuminosidadLeida(){
                return (voltajeLuminosidadLeida);
          }

          int getHumbralAlerta(){
                return (humbralAlerta);
          }
          
          void setHumbralAlerta(int humbralAlertaP){
              humbralAlerta = humbralAlertaP;
           }
           
          //Metodos
         int16_t leerLuminosidad(){
            
            int16_t luminosidad =-1;
            voltajeLuminosidadLeida = analogRead(pinLectura);
            
            luminosidad = 100 - (100*79/(79-4096)-voltajeLuminosidadLeida*100/(79-4096));
            //comprobarLuminosidad(luminosidad);
            
            return luminosidad;
            
          }

         /* void comprobarLuminosidad(int16_t luminosidad){

            if(luminosidad < humbralAlerta){
              digitalWrite(pinActuador, HIGH);
            }else{
              digitalWrite(pinActuador, LOW);
            }
         }*/

        String estadoLuminosidad(int16_t luminosidad){
          String estadoLuminosidad = "Oscuridad total";
          if(luminosidad >= 85){
            estadoLuminosidad = "Alta luminosidad";
           } else if(luminosidad < 85 && luminosidad > 50){
              estadoLuminosidad = "Luminosidad moderada";
           } else if(luminosidad < 50 && luminosidad >30){
             estadoLuminosidad = "Luminosidad baja";
           }else{
             estadoLuminosidad = "Luminosidad muy baja";
           }

           return estadoLuminosidad;
       }

     void actuarSobreLuces(boolean estadoLuces){
            
            if(estadoLuces){
              digitalWrite(pinActuador, HIGH);
            }else{
              digitalWrite(pinActuador, LOW);
              Serial.print("Luces apagados");
            }
    }
} ;
