#include <SPI.h>
#include <LoRa.h>

int counter = 0;

// Parameters you can play with :

int txPower = 14; // from 0 to 20, default is 14
int spreadingFactor = 9; // from 7 to 12, default is 12
long signalBandwidth = 125E3; // 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3,41.7E3,62.5E3,125E3,250E3,500e3, default is 125E3
int codingRateDenominator=5; // Numerator is 4, and denominator from 5 to 8, default is 5
int preambleLength=8; // from 2 to 20, default is 8
String payload = "hello"; // you can change the payload
float distance = 0;
float x = 10;
float FSPL = 22.9; //Perte de chemin d'espace libre, constante calculé avec les données de la carte UCA.
float K = 27.55; //Constante du FSPL.
float f = 860; //Fréquence d'envoi de la carte. (En MHz).


#define SS 10
#define RST 8
#define DI0 3
#define BAND 865E6  // Here you define the frequency carrier

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Receiver");
  Serial.print("SetFrequency : ");
  Serial.print(BAND);
  Serial.println("Hz");
  Serial.print("SetSpreadingFactor : SF");
  Serial.println(spreadingFactor);
  

  SPI.begin();
  LoRa.setPins(SS,RST,DI0);

  

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
 LoRa.setTxPower(txPower,1);
 LoRa.setSpreadingFactor(spreadingFactor);
 LoRa.setSignalBandwidth(signalBandwidth);
 LoRa.setCodingRate4(codingRateDenominator);
 LoRa.setPreambleLength(preambleLength);
 
}

void loop() {
   // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }
   distance = pow(x,(FSPL-K-(20*log10(f))-LoRa.packetRssi())/20); //LoRa.packetRssi en dBm (décibel par milliwatt).
   Serial.print("L'objet est à environ ");
   Serial.print(distance); 
   Serial.println(" m.");
} 
}



 
