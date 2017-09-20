//Libraries
#include <DHT.h>

//Constants
#define DHTPIN 2              // what pin we're connected to
#define DHTTYPE DHT22         // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);     // Initialize DHT sensor for normal 16mhz Arduino


//Variables
float hum;  //Stores humidity value
float temp; //Stores temperature value


void setup() {
  // put your setup code here, to run once:
      pinMode(13,OUTPUT);
      Serial.begin(9600);
      dht.begin();
}

void loop() {
    // put your main code here, to run repeatedly:
        
       
   
         temp= dht.readTemperature();    //Read data and store it to variables temp.    
   
          Serial.print("Temp: ");
          Serial.print(temp);           //Print temp values to serial monitor
          Serial.print(" Celsius");
          delay(2000);                  //Delay 2 sec.

       if(temp < 30.0){
        digitalWrite(13 ,LOW);     //completes the circuit
        Serial.println("\tThe heater is on");
        }
       else {
         digitalWrite(13 ,HIGH);    //breaks the circuit
         Serial.println(" ");
        }
      
}
