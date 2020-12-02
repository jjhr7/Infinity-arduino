class  Actuador
  {  private:
         int pinActuador; //Este es el numero del pin al que se va a conectar el actuador, si no se tiene un actuador se recomienda usar un LED
         
     public: 
          //Constructores
          Actuador(){
             pinActuador = 0;
          }
          
          Actuador( int pinActuadorP){
             pinActuador = pinActuadorP;
          }
          
          //Getters y Setters  
          void setPinActuador(int pinActuadorP){
              pinActuador = pinActuadorP;
           }
            
          int getPinActuador(){
                return (pinActuador);
          }
          
          //Metodos

          void cambiarEstadoActuador(boolean estadoActuador){

            if(estadoActuador){
              digitalWrite(pinActuador, HIGH);
            }else{
              digitalWrite(pinActuador, LOW);
            }
         }
} ;
