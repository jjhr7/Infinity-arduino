class  SDHT : public Sensor
  {  private:
         int pinActuador; //Este es el numero del pin al que se va a conectar el actuador, si no se tiene un actuador se recomienda usar un LED
         float humbralAlerta;
     public: 
          //Constructores
          SDHT()
          : Sensor(25){
             pinActuador = 26;
             humbralAlerta = 16.00;
          }
          
          SDHT( int pinActuadorP, int pinLecturaP, int humbralAlertaP)
          : Sensor(pinLecturaP){
             pinActuador = pinActuadorP;
             humbralAlerta = humbralAlertaP;
          }
          
          //Getters y Setters  
          void setPinActuador(int pinActuadorP){
              pinActuador = pinActuadorP;
           }
            
          int getPinActuador(){
                return (pinActuador);
          }
                    

          float getHumbralAlerta(){
                return (humbralAlerta);
          }
          
          void setHumbralAlerta(float humbralAlertaP){
              humbralAlerta = humbralAlertaP;
           }
           
          //Metodos

          void comprobarTemperatura(float temperatura){

            if(temperatura>humbralAlerta){
              digitalWrite(pinActuador, HIGH);
            }else{
              digitalWrite(pinActuador, LOW);
            }
         }

         int16_t tomarLectura(){
            
            int16_t temperatura =-1;
      
            Serial.println("Este metodo no se usa en este sensor, el propio sensor ya hace los calculos necesarios para tomar las lecturas");
            
            return temperatura;
            
          }

} ;
