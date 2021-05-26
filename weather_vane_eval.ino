#define WIND_DIR_PIN 35

void setup() {
  // put your setup code here, to run once:
  pinMode(14, INPUT);
  Serial.begin(115200);
}

void loop() {
  int windPosition;
  String windDirection = "N";
  int analogCompare[15] = {150, 300, 450, 600, 830, 1100, 1500, 1700, 2250, 2350, 2700, 3000, 3200, 3400, 3900};
  //String windDirText[15] = {"SSW", "S", "WSW", "3", "SW", "W", "6", "ESE", "SE", "NNW", "NW", "ENE", "E", "NNE", "NE"}; //BLYNK does not seem to allow text
  String windDirText[15] = {"202.5", "180", "247.5", "000", "225", "270", "000", "112.5", "135", "337.5", "315", "67.5", "90", "22.5", "45"};

  int vin = analogRead(WIND_DIR_PIN);

  for (windPosition = 0; windPosition < 15; windPosition++)
  {
    if (vin < analogCompare[windPosition])
    {
      windDirection = windDirText[windPosition];
      break;
    }
  }
  Serial.printf("Wind direction: %i - %i - %s\n", vin, windPosition, windDirText[windPosition]);
  delay(250);
}
