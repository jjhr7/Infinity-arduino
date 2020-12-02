class FotoResistencia : public Sensor
  {  private:
         int voltajeLuminosidadLeida;
     public: 
          //Constructores
          FotoResistencia()
          : Sensor(32){
             voltajeLuminosidadLeida = 0;
          }
          
          FotoResistencia(int pinLecturaP)
          : Sensor(pinLecturaP){
             voltajeLuminosidadLeida = 0;
          }
          
          //Getters y Setters  

          void setPinLectura(int pinLecturaP){
              pinLectura = pinLecturaP;
           }
            
          int getPinLectura(){
                return (pinLectura);
          }
          
          int getVoltajeLuminosidadLeida(){
                return (voltajeLuminosidadLeida);
          }

          //Metodos
         int16_t tomarLectura(){
            
            int16_t luminosidad =-1;
            voltajeLuminosidadLeida = analogRead(pinLectura);
            
            luminosidad = 100 - (100*79/(79-4096)-voltajeLuminosidadLeida*100/(79-4096));
            //comprobarLuminosidad(luminosidad);
            
            return luminosidad;
            
          }

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
};
