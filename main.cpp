#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 10  //Pin de datos del sensor DHT22
#define DHTTYPE DHT22  //Especificamos el tipo de sensor 

DHT dht(DHTPIN, DHTTYPE);  //Creamos el objeto que representa al sensor

const char * ssid = "Humillate_ante_mi"; //Nombre de la red con acceso a internet
const char * pass = "1234554321@@@"; //Contraseña de la red
const char* host = "dweet.io"; //Pagina de dweet.io
float humedad;
float temperatura;
float indice;


String getTipoCifrado(wifi_auth_mode_t tipoCifrado) {
  switch (tipoCifrado) {
    case (WIFI_AUTH_OPEN):
      return "Open";
    case (WIFI_AUTH_WEP):
      return "WEP";
    case (WIFI_AUTH_WPA_PSK):
      return "WPA_PSK";
    case (WIFI_AUTH_WPA2_PSK):
      return "WPA2_PSK";
    case (WIFI_AUTH_WPA_WPA2_PSK):
      return "WPA_WPA2_PSK";
    case (WIFI_AUTH_WPA2_ENTERPRISE):
      return "WPA2_ENTERPRISE";
  }
}
 
void scanNetworks() {
 
  int numberOfNetworks = WiFi.scanNetworks();
 
  Serial.print("Numero de redes encontradas: ");
  Serial.println(numberOfNetworks);
 
  for (int i = 0; i < numberOfNetworks; i++) {
 
    Serial.print("Nombre de red: ");
    Serial.println(WiFi.SSID(i));
 
    Serial.print("Fuerza de la señal: ");
    Serial.println(WiFi.RSSI(i));
 
    Serial.print("Direccion MAC: ");
    Serial.println(WiFi.BSSIDstr(i));
 
    Serial.print("Tipo de cifrado: ");
    String tipoCifradoDescription = getTipoCifrado(WiFi.encryptionType(i));
    Serial.println(tipoCifradoDescription);
    Serial.println("-----------------------");
 
  }
}
 
void connectToNetwork() {
  WiFi.begin(ssid, pass);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Estableciendo conexion WiFi..");
  }
 
  Serial.println("Conectado a la red");
 
}
 
void conexion_dht(){
   Serial.println("DHT22 en testeo"); //Mensaje para indicar el testeo del sensor
   humedad = dht.readHumidity();
   temperatura = dht.readTemperature();
   if(isnan(humedad) || isnan(temperatura)){
    Serial.println("Ha ocurrido un problema al conectar con el sensor");
    return;
   }
   indice = dht.computeHeatIndex(temperatura, humedad, false);
} 

void setup() {
  Serial.begin(115200);
  scanNetworks(); //Escanear las redes WiFi disponibles
  connectToNetwork();  //Conectarse a la red que especificamos arriba
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP()); 
  Serial.println("Procediendo con la conexion del DHT22");
}
 
void loop() {
  conexion_dht(); //Metodo para la configuracion del DHT22
   Serial.println("Conectandose a:");
   Serial.println(host);
   String tempenv= String(temperatura);
   String humeenv= String(humedad);
   WiFiClient client;

   const int httpPort = 80;
   
   if(!client.connect(host, httpPort)){
     Serial.println("Conexion Fallida");
     delay(1000);
     return;
   }

  //String cadena = "hola"+String("mundo")+String(variable);
  String url = "/dweet/for/Alejog99?temperatura="+String(temperatura)+"&humedad="+String(humedad)+"&indice="+String(indice);

  //Se pueden leer los dweets en  https://dweet.io/get/latest/dweet/for/Alejog99

  Serial.print("Solicitando el recurso: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" 
                    + "Connection: close\r\n\r\n");
  unsigned long timeout = millis(); //Leo la hora
  while(client.available() == 0){   //Mientras hayan cero letras disponibles...
      if(millis() - timeout > 5000){ //Resto la hora actual a la hora inicial, si es mas de 5s
        Serial.println("Expiro el tiempo de espera");
        client.stop();
        return;
      }
  }

  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.println(line);
  }
  
  Serial.println("Fin de conexion");
  client.stop();
  delay(20000);
}