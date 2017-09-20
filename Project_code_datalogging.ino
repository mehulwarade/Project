//Libraries
#include "SD.h"
#include <Wire.h>
#include "RTClib.h"
#include <DHT.h>
RTC_PCF8523 rtc;

#define LOG_INTERVAL  1000 
/* mills between entries. How many milliseconds before writing the logged data permanently to disk. 
 Set it to the LOG_INTERVAL to write each time (safest). Set it to 10*LOG_INTERVAL to write all data every 10 datareads, 
 you could lose up to the last 10 reads if power is lost but it uses less power and is much faster!
*/
#define SYNC_INTERVAL 1000 // mills between calls to flush() - to write data to the card
uint32_t syncTime = 0; // time of last sync()

/*
  Determines whether to send the stuff thats being written to the card also out to the Serial monitor.
  This makes the logger a little more sluggish and you may want the serial monitor for other stuff.
  On the other hand, its hella useful. We'll set this to 1 to keep it on. Setting it to 0 will turn it off.
*/
#define ECHO_TO_SERIAL   1 // echo data to serial port. 

RTC_PCF8523 RTC; // define the Real Time Clock object

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;

// the logging file
File logfile;

#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor for normal 16mhz Arduino
float temp; //Stores temperature value

void setup() {
  Serial.begin(9600);
  dht.begin();
 

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  // initialize the SD card
  initSDcard();

  // create a new file
  createFile();
  /*
     Connect to RTC
     Now we kick off the RTC by initializing the Wire library and poking the RTC to see if its alive.
  */
  initRTC();
  /*
     Now we print the header. The header is the first line of the file and helps your spreadsheet or math program identify whats coming up next.
     The data is in CSV (comma separated value) format so the header is too: "timestamp,datetime,temp" the first item timestamp is the timestamp, 
     datetime is the time and date from the RTC in human readable format and temp is the temperature read.
  */
  logfile.println("Date,Time,tempreture(in celsius)");
#if ECHO_TO_SERIAL
  Serial.println("Date,Time,tempreture(in celsius)");
#endif //ECHO_TO_SERIAL

}

void loop() {
  DateTime now;
   // delay for the amount of time we want between readings
   delay((LOG_INTERVAL - 1) - (millis() % LOG_INTERVAL));
  
   // fetch the time
  now = RTC.now();
// fetch the time
  now = RTC.now();
  // log time
  logfile.print(now.year(), DEC);
  logfile.print("/");
  logfile.print(now.month(), DEC);
  logfile.print("/");
  logfile.print(now.day(), DEC);
  logfile.print(", ");
  logfile.print(now.hour(), DEC);
  logfile.print(":");
  logfile.print(now.minute(), DEC);
  logfile.print(":");
  logfile.print(now.second(), DEC);
#if ECHO_TO_SERIAL
  Serial.print(now.year(), DEC);
  Serial.print("/");
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.print(now.day(), DEC);
  Serial.print(", ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
#endif //ECHO_TO_SERIAL

  //Read data and store it to variables hum and temp
    temp= dht.readTemperature();

    //Print temp and humidity values to log file
     logfile.print(", ");
     logfile.println(temp);

    //Print temp and humidity values to serial monitor
    Serial.print(", ");
    Serial.println(temp);
    logfile.flush();
    delay(150000);
    

}

/**
   The error() function, is just a shortcut for us, we use it when something Really Bad happened.
   For example, if we couldn't write to the SD card or open it.
   It prints out the error to the Serial Monitor, and then sits in a while(1); loop forever, also known as a halt
*/
void error(char const *str)
{
  Serial.print("error: ");
  Serial.println(str);

  while (1);
}

void initSDcard()
{
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

}

void createFile()
{
  //file name must be in 8.3 format (name length at most 8 characters, follwed by a '.' and then a three character extension.
  char filename[] = "MLOG00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[4] = i / 10 + '0';
    filename[5] = i % 10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE);
      break;  // leave the loop!
    }
  }

  if (! logfile) {
    error("couldnt create file");
  }

  Serial.print("Logging to: ");
  Serial.println(filename);
}

void initRTC()
{
  Wire.begin();
  if (!RTC.begin()) {
    logfile.println("RTC failed");
#if ECHO_TO_SERIAL
    Serial.println("RTC failed");
#endif  //ECHO_TO_SERIAL

  }
}

