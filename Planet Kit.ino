#include <dht.h>
#include <OneWire.h>
#include <DallasTemperature.h>


#define DHT22_PIN 3
#define soil_moisture_pin A0
#define soil_temperature_pin 2
#define MQ7_pin A1
#define MQ8_pin A2

#define ONE_WIRE_BUS soil_temperature_pin


dht DHT;
OneWire oneWire(soil_temperature_pin);
DallasTemperature sensors(&oneWire);

//RO f0r mq7 and mq8
float mq7_R0 = 0.05;
float mq8_R0 = 0.25;

//genral global variable
float air_humidity;
float air_temp;
float soil_moisture;
float soil_temp;
float mq7; //in ppm
float mq8; //in ppm

//variables used in calculation to get the ppm for mq7 and mq8
float mq_sensor_volt = 0;
float mq_RS_gas = 0;
float mq_ratio = 0;
float mq_sensorValue = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(soil_moisture_pin, INPUT);
  pinMode(soil_temperature_pin, INPUT);
  pinMode(MQ7_pin, INPUT);
  pinMode(MQ8_pin, INPUT);
  sensors.begin();
}

void loop()
{
  int chk = DHT.read22(DHT22_PIN);
  air_humidity = (DHT.humidity);
  air_temp = (DHT.temperature);
  soil_moisture = analogRead(soil_moisture_pin); 
  soil_moisture =(1-(soil_moisture/1023))*100;
  soil_temp = analogRead(soil_temperature_pin);
  sensors.requestTemperatures();
  soil_temp=sensors.getTempCByIndex(0);

  ///reading for mq7
  mq_sensorValue = analogRead(MQ7_pin);
  mq_sensor_volt = mq_sensorValue / 1024 * 5.0;
  mq_RS_gas = (5.0 - mq_sensor_volt) / mq_sensor_volt;
  mq_ratio = mq_RS_gas / mq7_R0; //Replace R0 with the value found using the calibration code
  mq7 = 100 * pow(10, ((log10(mq_ratio) * -2.6) + 2.7)); //Formula for co 2 concentration
  delay(100);
  
  ///reading for mq8
  mq_sensorValue = analogRead(MQ7_pin);
  mq_sensor_volt = mq_sensorValue / 1024 * 5.0;
  mq_RS_gas = (5.0 - mq_sensor_volt) / mq_sensor_volt;
  mq_ratio = mq_RS_gas / mq8_R0; //Replace R0 with the value found using the calibration code
  mq8 = 100 * pow(10, ((log10(mq_ratio) * -2.6) + 2.7)); //Formula for co 2 concentration
  
  //reading of light sensor
  int light_sensor_value;
  light_sensor_value=analogRead(5);   //connect grayscale sensor to Analog 0
  //Serial.println(light_sensor_value, DEC);//print the value to serial        
  delay(100);
  
  //reading of UV sensor
  int UV_sensorValue;
  int analogValue = analogRead(7);//connect UV sensors to Analog 0    
  if (analogValue<20)
  {
    UV_sensorValue = 0;
  }
  else
  {
    UV_sensorValue= 0.05*analogValue-1;
  }
  //Serial.println(UV_sensorValue);//print the value to serial  
  delay(200);      
  
  Serial.println("mq7: "+String(mq7)+"ppm, mq8: "+String(mq8)+"ppm, air_humidity: "+String(air_humidity)+"%, air_temp: "+String(air_temp)+"*C, soil_moisture: "+String( soil_moisture)+"%, soil_temp: "+String(soil_temp)+"*C, light_sensor_value: "+String(light_sensor_value)+", UV_sensorValue: "+String(UV_sensorValue));
  
  delay(500);  //decreasing teh delay will result in higher percentage of garbage value from the dht22
}
