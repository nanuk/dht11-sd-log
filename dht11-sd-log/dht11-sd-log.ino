// Incluimos librería
#include <DHT.h>
//SD Libs
#include <SD.h>
#include <Wire.h>
#include <SPI.h>
//reloj
#include "RTClib.h" 
// Definimos el pin digital donde se conecta el sensor
#define DHTPIN 2
// Dependiendo del tipo de sensor
#define DHTTYPE DHT11
//inicia DHT
DHT dht(DHTPIN, DHTTYPE);
// Definimos el reloj
RTC_DS1307 RTC; 
// SD card pin select
const int chipSelect = 10; 
 // Fichero a escribir en la SD 
File logfile;
const bool eco = true ;
// Controla cada cuanto tiemepo se vuelcan los datos a la SD
int count = 0 ;       

void setup() {
   // Inicializamos comunicación serie
  Serial.begin(9600);
  // Comenzamos el sensor DHT
  dht.begin();
  // SD card pin select
  pinMode(chipSelect, OUTPUT);        
  //verificacion SD
  if (!SD.begin(chipSelect))
     Serial.print("No hay tarjeta SD.");
  else
     Serial.println("Tarjeta SD inicializada.");
  // Creamos el fichero de registro
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) 
     {  
        filename[6] = i/10 + '0';
        filename[7] = i%10 + '0';
        if (! SD.exists(filename))      // Si no existe el fichero, lo creamos
            { 
              logfile = SD.open(filename, FILE_WRITE); 
              break;  // leave the loop!
            }
     }
  if (! logfile) {
      Serial.print("No se pudo crear el fichero de registro");
    }
  Serial.print("Registrando en: ");   Serial.println(filename);
// connect to RTC
  Wire.begin();  
  if (!RTC.begin()) 
      logfile.println("No hay RTC.");   
  else
      Serial.println("RTC correcto. Iniciando captura de datos");
  //registro
  logfile.print("Fecha/Hora") ; 
  logfile.print(", ");
  //logfile.print(" ID ") ; 
  //logfile.print(", ");  
  logfile.print("Humedad") ; 
  logfile.print(", ");
  logfile.println("Temperatura") ; 
  logfile.print(", ");
  logfile.println("Índice de calor") ;
}

void loop() {
   //fecha y hora
  DateTime now;
  int err  ;
  //DHT
  // Leemos la humedad relativa
  float h = dht.readHumidity();
  // Leemos la temperatura en grados centígrados (por defecto)
  float t = dht.readTemperature();
  // Comprobamos si ha habido algún error en la lectura
  if (isnan(h) || isnan(t)) {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    return;
  }
  else{
    now = RTC.now();
    // Calcular el índice de calor en grados centígrados
    float hic = dht.computeHeatIndex(t, h, false);
    //escritura archivo
    logfile.print(now.unixtime()); // seconds since 1/1/1970
    logfile.print(", ");
    //logfile.print("Humedad:");
    logfile.print(h);
    logfile.print(" %\t");
    logfile.print(", ");
    //logfile.print("Temperatura DHT:");
    logfile.print(t);
    logfile.print(" *C ");
    //logfile.print("Índice de calor: ");
    logfile.print(hic);
    logfile.print(" *C ");
    logfile.print("\r\n");
    //escritura
    if ( count++ > 24 ){   logfile.flush(); // Para forzar la escritura en la SD
                    count = 0 ;       // Cada 64 lecturas
    }
     if (eco){
       Serial.print(now.unixtime()); // seconds since 1/1/1970
       Serial.print("Temperatura DHT:");
       Serial.print(t);
     }
  }
   // Esperamos X segundos entre medidas
  delay(10000);
}
void error(char *str)
{
  Serial.print("error: ");
  Serial.println(str);
  
  while(1);
}
