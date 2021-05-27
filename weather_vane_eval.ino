#define WIND_DIR_PIN 35
#define LED           2  //Diagnostics using built-in LED
#define RAIN_PIN     25  //reed switch based tick counter on tip bucket

volatile int rainTicks = 0;
// Variables used in software delay to supress spurious counts on rain_tip
volatile unsigned long timeSinceLastTip = 0;
volatile unsigned long validTimeSinceLastTip = 0;
volatile unsigned long lastTip = 0;


void setup() {
  // put your setup code here, to run once:
  pinMode(14, INPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 0);

  //Rainfall interrupt pin set up
  pinMode(RAIN_PIN, INPUT);     // Rain sensor
  delay(100); //possible settling time on pin to charge
  attachInterrupt(digitalPinToInterrupt(RAIN_PIN), rainTick, FALLING);
  Serial.begin(115200);
}

void loop() {
  int windPosition;
  int windPin;
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
  windPin = digitalRead(14);
  digitalWrite(LED, !windPin);
  Serial.printf("Wind speed switch %i\n", windPin);
  Serial.printf("Wind direction: %i - %i - %s\n", vin, windPosition, windDirText[windPosition]);
  Serial.printf("\nRain ticks: %i\n\n\n", rainTicks);
  delay(250);
}

//ISR
void rainTick(void)
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
