class Higrometro
  {  private:
         int valorSeco;  // Valor leido con el higrometro en seco
         int valorHumedo;// Valor leido con el higrometro humedo
         int pinActuador; //Este es el numero del pin al que se va a conectar el actuador, si no se tiene un actuador se recomienda usar un LED
         int pinLectura; //Este es el número del pin por el que se va a realizar la lectura del voltaje para calcular la humedad a la que se expone el higrometro
         int voltajeHumedadLeido;
         int humbralAlerta;
     public: 
          //Constructores
          Higrometro(){
             valorSeco = 4096;
             valorHumedo = 1182;
             pinActuador = 27;
             pinLectura = 34;
             voltajeHumedadLeido = 0;
             humbralAlerta = 30;
          }
          
          Higrometro(int valorSecoP, int valorHumedoP, int pinActuadorP, int pinLecturaP, int humbralAlertaP){
             valorSeco = valorSecoP;
             valorHumedo = valorHumedoP;
             pinActuador = pinActuadorP;
             pinLectura = pinLecturaP;
             voltajeHumedadLeido = 0;
             humbralAlerta = humbralAlertaP;
          }
          
          //Getters y Setters  
          void setValorSeco(int valorSecoP){
              valorSeco = valorSecoP;
            }
            
            int getValorSeco(){
                return (valorSeco);
            }
             
          void setValorHumedo(int valorHumedoP){
              valorHumedo = valorHumedoP;
           }
            
          int getValorHumedo(){
                return (valorHumedo);
          }

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
          
          int getVoltajeHumedadLeido(){
                return (voltajeHumedadLeido);
          }

          int getHumbralAlerta(){
                return (humbralAlerta);
          }
          
          void setHumbralAlerta(int humbralAlertaP){
              humbralAlerta = humbralAlertaP;
           }
           
          //Metodos
         int16_t leerHumedad(){
            int16_t humedad;
            voltajeHumedadLeido = analogRead(pinLectura);
            
            humedad = 100*valorSeco/(valorSeco-valorHumedo)-voltajeHumedadLeido*100/(valorSeco-valorHumedo);
            //comprobarHumedad(humedad);
            
            return humedad;
            
          }

          void comprobarHumedad(int16_t humedad){

            if(humedad>humbralAlerta){
              digitalWrite(pinActuador, HIGH);
            }else{
              digitalWrite(pinActuador, LOW);
            }
         }
          void actuarBombaDeRiego(boolean actuadorBomba){

            if(actuadorBomba){
              digitalWrite(pinActuador, HIGH);
            }else{
              digitalWrite(pinActuador, LOW);
            }
         }
         
} ;
