#include <SPI.h>
#include <MFRC522.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <Servo.h>

#define RST_PIN         0          // Configurable, see typical pin layout above
#define SS_PIN          2         // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

Servo servo;
int vel = 0;

// Sustituir por los datos de vuestro WiFi
const char* ssid = "COMTECO-96017932";
const char* password = "YZISP55131";
String url = "http://192.168.1.8:8000/api/historials";
String RFID = "";

void setup() {
	Serial.begin(9600);		// Initialize serial communications with the PC
	while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
   }
   Serial.println("");
   Serial.print("Connected to WiFi network with IP Address: ");
   Serial.println(WiFi.localIP());

    servo.attach(15); //D1
    servo.write(0);
}

void loop() {

  int httpResponseCode = 0;

  vel = 90;
  servo.write(vel); 
  
	// Si n hay una targeta en el lector
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// si la tergeta se puede leer
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}

  HTTPClient http;
  WiFiClient client;
  
  if (http.begin(client, url)) //Iniciar conexión
  {
     Serial.print("[HTTP] GET...\n");
     http.addHeader("Content-Type", "application/x-www-form-urlencoded");
   
    Serial.print("UID:");
    for(byte i = 0; i < mfrc522.uid.size; i++){
      Serial.print(mfrc522.uid.uidByte[i]);
      RFID = RFID+mfrc522.uid.uidByte[i];
    }
    Serial.println();
    Serial.println("UID RFID:"+RFID);

    String httpRequestData = "RFID="+RFID;           
    
    httpResponseCode = http.POST(httpRequestData);
    
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    
    http.end();
  }
  else {
     Serial.printf("[HTTP} Unable to connect\n");
  }

  RFID = "";
  mfrc522.PICC_HaltA();

  if(httpResponseCode == 200){
    vel = 0;
    servo.write(vel);              
    delay(500);
    vel = 90;
    servo.write(vel);              
    delay(1000);      
    vel = 180;
    servo.write(vel);              
    delay(500);
    vel = 90;
    servo.write(vel);              
    delay(500);
  }
}
