class Sensor
  {  protected:
         int pinLectura; //Este es el número del pin por el que se va a realizar la lectura del voltaje para calcular la humedad a la que se expone el higrometro
     public: 
          //Constructores
          Sensor(){
             pinLectura = 0;
          }
          
          Sensor(int pinLecturaP){
             pinLectura = pinLecturaP;
          }
          
          //Getters y Setters  
          
          void setPinLectura(int pinLecturaP){
              pinLectura = pinLecturaP;
           }
            
          int getPinLectura(){
                return (pinLectura);
          }
          
          //Metodos abstractos
         virtual int16_t tomarLectura();

         
} ;
