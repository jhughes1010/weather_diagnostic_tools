#include <Wire.h>
#include <BME280I2C.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Set web server port number to 80
//WiFiServer server(80);

#define WIND_DIR_PIN 35
#define LED           2  //Diagnostics using built-in LED
#define RAIN_PIN     25  //reed switch based tick counter on tip bucket
#define VOLT_PIN     33

void IRAM_ATTR rainTick(void);


volatile int rainTicks = 0;
// Variables used in software delay to supress spurious counts on rain_tip
volatile unsigned long timeSinceLastTip = 0;
volatile unsigned long validTimeSinceLastTip = 0;
volatile unsigned long lastTip = 0;


BME280I2C bme;

//data struct for instantaneous sensor values
struct sensorData
{
  float temperatureC;
  float temperatureF;
  float windSpeed;
  float windDirection;
  float barometricPressure;
  float BMEtemperature;
  float humidity;
  float UVIndex;
  float lux;
  float batteryVoltage;
};


void setup() {
  // put your setup code here, to run once:
  pinMode(14, INPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 0);
  Wire.begin();
  bme.begin();

  //Rainfall interrupt pin set up
  pinMode(RAIN_PIN, INPUT);     // Rain sensor
  delay(100); //possible settling time on pin to charge
  attachInterrupt(digitalPinToInterrupt(RAIN_PIN), rainTick, FALLING);
  Serial.begin(115200);
}

void loop() {

  struct sensorData environment;
  int windPosition;
  int windPin;
  int vbat;
  String windDirection = "N";
  int analogCompare[15] = {150, 300, 450, 600, 830, 1100, 1500, 1700, 2250, 2350, 2700, 3000, 3200, 3400, 3900};
  String windDirText[15] = {"xxx", "S", "WSW", "SSW", "SW", "W", "yyy", "ESE", "SE", "NNW", "NW", "ENE", "E", "NNE", "NE"}; //BLYNK does not seem to allow text
  //String windDirText[15] = {"202.5", "180", "247.5", "000", "225", "270", "000", "112.5", "135", "337.5", "315", "67.5", "90", "22.5", "45"};


  vbat = analogRead(VOLT_PIN);
  int vin = analogRead(WIND_DIR_PIN);


  for (windPosition = 0; windPosition < 15; windPosition++)
  {
    if (vin < analogCompare[windPosition])
    {
      windDirection = windDirText[windPosition];
      break;
    }
  }
  windPin = digitalRead(14);
  digitalWrite(LED, windPin);
  Serial.printf("Wind speed switch %i\n", windPin);
  Serial.printf("Wind direction: %i - %i - %s\n", vin, windPosition, windDirText[windPosition]);
  Serial.printf("\nValue at ADC pin for vbat%i\n", vbat);
  Serial.printf("\nRain ticks: %i\n\n\n", rainTicks);

  readBME(&environment);

  delay(500);
}

//ISR
void IRAM_ATTR rainTick(void)
{


  timeSinceLastTip = millis() - lastTip;
  //software debounce attempt
  if (timeSinceLastTip > 300)
  {
    validTimeSinceLastTip = timeSinceLastTip;
    rainTicks++;
    lastTip = millis();
  }
}

////BME sensor readout
void readBME(struct sensorData *environment)
{

  float temp(NAN), hum(NAN), pres(NAN);
  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_Pa);

  bme.read(pres, temp, hum, tempUnit, presUnit);
  //bme.read(environment->barometricPressure, environment->BMEtemperature, environment->humidity, BME280::TempUnit_Celsius, BME280::PresUnit_Pa);
  //bme.read(environment->barometricPressure, environment->BMEtemperature, environment->humidity, BME280::TempUnit_Fahrenheit, BME280::PresUnit_inHg);
  Serial.printf("BME barometric pressure: %6.2f  BME temperature: %6.2f  BME humidity: %6.2f\n", pres, temp, hum);
}
