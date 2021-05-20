#include <Wire.h>
#include <SPI.h>
#include "i2c.h"
#include "i2c_BMP280.h"
#include "i2c_MMA8451.h"
#include <LoRa.h>
BMP280 bmp280;
MMA8451 mma8451;

int txPower = 14; // from 0 to 20, default is 14
int spreadingFactor = 9; // from 7 to 12, default is 12
long signalBandwidth = 125E3; // 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3,41.7E3,62.5E3,125E3,250E3,500e3, default is 125E3
int codingRateDenominator=5; // Numerator is 4, and denominator from 5 to 8, default is 5
int preambleLength=8; // from 2 to 20, default is 8
float x = 0;
float y = 0;
float z = 0;
int t = 0;
float altitude = 0;

#define SS 10
#define RST 8
#define DI0 3
#define BAND 865E6  // Here you define the frequency carrier

void setup() {
  Serial.begin(115200);
  
      if (bmp280.initialize()) Serial.println("Sensor found");
    else
    {
        Serial.println("Sensor missing");
        while (1) {}
    }

   bmp280.setEnabled(0);
   bmp280.triggerMeasurement();
   bmp280.awaitMeasurement();
         
   float temperature;
    bmp280.getTemperature(temperature);

    float Pascal;
    bmp280.getPressure(Pascal);

    static float metres, metersolde;
    bmp280.getAltitude(metres);
    metersolde = (metersolde * 10 + metres)/11;

    bmp280.triggerMeasurement();

    Serial.print("La pression de référence est: ");
    Serial.print(Pascal);
    Serial.println(" Pa");

    altitude = Pascal;
     
    SPI.begin();
    if (mma8451.initialize());
    else
    {
        Serial.println("Sensor missing");
        while(1) {};              
}
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

void loop() 
{
     LoRa.beginPacket();
          
    bmp280.awaitMeasurement();

    float temperature;
    bmp280.getTemperature(temperature);

    float pascal;
    bmp280.getPressure(pascal);

    static float metres, metersolde;
    bmp280.getAltitude(metres);
    metersolde = (metersolde * 10 + metres)/11;

    bmp280.triggerMeasurement();

    Serial.print(" HeightPT1: ");
    Serial.print(metersolde);
    Serial.print(" m; Height: ");
    Serial.print(metres);
    Serial.print(" Pressure: ");
    Serial.println(pascal);
    Serial.print(" Pa; T: ");
    Serial.print(temperature);
    Serial.println(" C");

  if (altitude < pascal){
    LoRa.println("L'objet est plus bas que vous.");
  }
  else{
    LoRa.println("L'objet est plus haut que vous.");
  }

      static float xyz_g[3];
      mma8451.getMeasurement(xyz_g);
      
        if ((x!=xyz_g[0])|(y!=xyz_g[1])|(z!=xyz_g[2])) {
            LoRa.println("L'objet est en mouvement");
        }    
        x = xyz_g[0];  
        y = xyz_g[1];
        z = xyz_g[2];

        delay(1000);
        
  LoRa.endPacket();    
                     
}
