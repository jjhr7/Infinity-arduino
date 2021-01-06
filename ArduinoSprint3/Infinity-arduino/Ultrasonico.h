class Ultrasonico 
  {  private:
         int pinEcho;
         int triggerPin;
         int disTanciaPresencia;
     public: 
          //Constructores
          Ultrasonico() {
             pinEcho = 17;
             triggerPin = 16;
             disTanciaPresencia = 0;
          }
          
          Ultrasonico(int pinEchoP, int triggerPinP, double disTanciaPresenciaP){
             pinEcho = pinEchoP;
             triggerPin = triggerPinP;
             disTanciaPresencia = disTanciaPresenciaP;
          }
          
          //Getters y Setters  
          void setPinEcho(int pinEchoP){
              pinEcho = pinEchoP;
            }
            
            int getPinEcho(){
                return (pinEcho);
            }
             
          void setTriggerPin(int triggerPinP){
              triggerPin = triggerPinP;
           }
            
          int getTriggerPin(){
                return (triggerPin);
          }

          int getDisTanciaPresencia(){
                return (disTanciaPresencia);
          }
          
          void setDisTanciaPresencia(int disTanciaPresenciaP){
              disTanciaPresencia = disTanciaPresenciaP;
           }

         int distancia() {
            long duracion, distanciaCm;
            digitalWrite(triggerPin, LOW); //nos aseguramos señal baja al principio
            delayMicroseconds(10);
            digitalWrite(triggerPin, HIGH); //generamos pulso de 10us 
            delayMicroseconds(10);
            digitalWrite(triggerPin, LOW);
            duracion = pulseIn(pinEcho, HIGH); //medimos el tiempo del pulso 
            distanciaCm = duracion * 10 / 292 / 2; //convertimos a distancia 
            return distanciaCm;
        }
      boolean hayPresencia(){
        int distanciaLeida = distancia();
        if(distanciaLeida > disTanciaPresencia){
            return false;
          }else{
           return true;  
          }
        
        }
         
} ;
