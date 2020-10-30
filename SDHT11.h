class  SDHT11
  {  private:
         int pinActuador; //Este es el numero del pin al que se va a conectar el actuador, si no se tiene un actuador se recomienda usar un LED
         int pinLectura; //Este es el número del pin por el que se va a realizar la lectura del voltaje para calcular la humedad a la que se expone el higrometro
         int voltajeHumedadLeido;
         float humbralAlerta;
     public: 
          //Constructores
          SDHT11(){
             pinActuador = 25;
             pinLectura = 26;
             voltajeHumedadLeido = 0;
             humbralAlerta = 25.00;
          }
          
          SDHT11( int pinActuadorP, int pinLecturaP, int humbralAlertaP){
             pinActuador = pinActuadorP;
             pinLectura = pinLecturaP;
             voltajeHumedadLeido = 0;
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
          
          int getVoltajeHumedadLeido(){
                return (voltajeHumedadLeido);
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
} ;
