#include <Arduino.h>
#include <BH1750.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <MQUnifiedsensor.h>
#include <DHT.h>
#include <SPI.h>
#include <SimpleKalmanFilter.h>

#define MHPIN PA11
#define DHTPIN PA8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter(0x23);
int LoRaID = 1807;
float lux;
SimpleKalmanFilter simpleKalmanFilter(2, 2, 1);

boolean
runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    return true;
  }
  return false;
}
uint16_t calculateCRC(uint8_t *data, uint8_t length)
{
  uint16_t crc = 0xFFFF;

  for (int i = 0; i < length; i++)
  {
    crc ^= data[i];
    for (int j = 0; j < 8; j++)
    {
      if (crc & 0x0001)
      {
        crc >>= 1;
        crc ^= 0xA001;
      }
      else
      {
        crc >>= 1;
      }
    }
  }

  return crc;
}
void int16_to_byte(short int16, uint8_t &_return_h, uint8_t &_return_l)
{
  _return_h = highByte(int16);
  _return_l = lowByte(int16);
}
void setup()
{
  Serial2.begin(9600);
  Serial1.begin(9600);
  delay(100);
  dht.begin();
  Wire.begin();
  pinMode(MHPIN, INPUT);

  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE))
  {
    Serial.println(F("BH1750 Advanced begin"));
  }
  else
  {
    Serial.println(F("Error initialising BH1750"));
  }
}
void loop()
{
  uint8_t data[14];
  uint16_t crc;
  int error = 0;
  if (runEvery(1000))
  {
    float humi = dht.readHumidity();
    float temp = dht.readTemperature();
    if (isnan(humi) || isnan(temp))
    {
      Serial.println(F("Failed to read from DHT sensor!"));
      error = 1;
    }
    if (lightMeter.measurementReady())
    {
      lux = lightMeter.readLightLevel();
      lux = simpleKalmanFilter.updateEstimate(lux);
    }
    else
    {
      Serial.println(F("Failed to read from BH1750 sensor!"));
      error = 1;
    }
    int rainValue = digitalRead(MHPIN);

    int16_to_byte(LoRaID, data[0], data[1]);
    data[2] = 0x08;
    int16_to_byte(humi, data[3], data[4]);
    int16_to_byte(temp, data[5], data[6]);
    int16_to_byte(lux, data[7], data[8]);
    data[9] = rainValue;
    data[10] = error;
    crc = calculateCRC(data, 11);
    data[11] = crc & 0xFF;
    data[12] = crc >> 8;
    data[13] = 0x0D;

    String Message = String(LoRaID) + "|H: " + String(humi) + "|T: " + String(temp) + "|L: " + String(lux) + "|R: " + String(rainValue) + "|" + String(error) + "@";
    Serial2.write(data, 14);
    Serial1.write(data, 14);
    Serial1.println(Message);
  }
}