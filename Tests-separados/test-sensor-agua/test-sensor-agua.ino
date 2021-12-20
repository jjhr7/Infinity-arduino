void setup()
   {
       Serial.begin(9600);
   }
 
void loop()
   {
       Serial.println(analogRead(34));
       delay(2500);
   }
