#define DS18S20_TEMPERATURE_SENSOR_PIN 5  // temperature sensor pin
#define H2_SENSOR_PIN 35                  // hydrogen sensor pin
#define WATER_PRESSURE_SENSOR2_PIN 34     // water pressure sensor pin
#define WATER_PRESSURE_SENSOR1_PIN 36     // water pressure sensor pin

#define PH_SENSOR1_PIN 15  // pH sensor pin
#define PH_SENSOR2_PIN 39  // pH sensor pin

// liquid pump pins
#define LIQUID_PUMP1 16
#define LIQUID_PUMP2 27
#define LIQUID_PUMP3 33

// gas pump pins
#define GAS_PUMP1 13
#define GAS_PUMP2 14

#define ELECTROVALVE 12

// methane sensor pins
#define CH4_SENSOR_TX_PIN 26
#define CH4_SENSOR_RX_PIN 25

// co2 sensors pins
#define CO2_SENSOR1_RX_PIN 4
#define CO2_SENSOR1_TX_PIN 23
#define CO2_SENSOR2_RX_PIN 18
#define CO2_SENSOR2_TX_PIN 19

// current and voltage sensor
#define CURRENT_SENSOR_PIN 0
#define VOLTAGE_SENSOR_PIN 0

bool calibrationMode = false;
bool manualMode = true;  // 0 mean auto (default), 1 mean manual

int CO2Sensor1Data = 0;
int CO2Sensor2Data = 0;
int CH4SensorData = 0;
float O2SensorData = 0.0;
int H2SensorData = 0;
float pHSensor1Data = 0.0;
float pHSensor2Data = 0.0;
float TempSensorData = 0.0;
float WPSensor1Data = 0.0;
float WPSensor2Data = 0.0;

float measuredCurrent = 0.0;
float measuredVoltage = 0.0;

// Parameters
#define LOWER_PH_THRESHOLD 6.5  // threshold
#define UPPER_PH_THRESHOLD 9.0  // threshold
#define CH4_THRESHOLD 95.0      // threshold


// calibration command
#define CMD_ZERO_ADJUSTMENT 1
#define CMD_ZERO_POINT 2
#define CMD_MIDDLE_POINT 3
#define CMD_FACTORY_RESET 4

// State variables
bool liquidPump1On = false;
bool liquidPump2On = false;
bool liquidPump3On = false;
bool gasPump1On = false;
bool gasPump2On = false;
bool electrovalveOn = false;
//unsigned long liquidPump1StartTime = 0;
//unsigned long liquidPump2StartTime = 0;
//unsigned long gasPump1StartTime = 0;
int speedGP1 = 0;
int speedGP2 = 0;
int speedLP1 = 0;
int speedLP2 = 0;
int speedLP3 = 0;

String calibrationInfo = R"(
    >>> Device in the Calibration Mode <<<
    --> "exit" : exit the calibration mode.
    --> "enterph1" : Enter pH sensor 1 calibration
    --> "calph1" : Calibrate pH sensor 1
    --> "exitph1" : Exit pH sensor 1
    --> "enterph2" : Enter pH sensor 2 calibration
    --> "calph2" : Calibrate pH sensor 2
    --> "exitph2" : Exit pH sensor 2
    --> "zeroAdjust" : CH4 sensor zero adjustement
    --> "zeroPoint" : CH4 sensor zero point calibration
    --> "middlePoint" : CH4 sensor middle point calibration
    --> "factoryReset" : CH4 sensor calibration factory reset
)";

void setPinsMode() {
  // set pins as input
  pinMode(DS18S20_TEMPERATURE_SENSOR_PIN, INPUT);
  pinMode(H2_SENSOR_PIN, INPUT);
  pinMode(WATER_PRESSURE_SENSOR1_PIN, INPUT);
  pinMode(WATER_PRESSURE_SENSOR2_PIN, INPUT);
  pinMode(PH_SENSOR1_PIN, INPUT);
  pinMode(PH_SENSOR2_PIN, INPUT);

  // set pins as output
  pinMode(LIQUID_PUMP1, OUTPUT);
  pinMode(LIQUID_PUMP2, OUTPUT);
  pinMode(LIQUID_PUMP3, OUTPUT);
  pinMode(GAS_PUMP1, OUTPUT);
  pinMode(GAS_PUMP2, OUTPUT);
  pinMode(ELECTROVALVE, OUTPUT);
  digitalWrite(ELECTROVALVE, LOW);

  // manually initialize pumps at the start
  /*speedGP1 = 130;
  speedGP2 = 130;
  speedGP3 = 130;
  speedLP1 = 130;
  speedLP2 = 130;

  analogWrite(GAS_PUMP1, speedGP1);
  gasPump1On = (speedGP1 != 0);
  analogWrite(GAS_PUMP2, speedGP2);
  gasPump2On = (speedGP2 != 0);
  analogWrite(LIQUID_PUMP1, speedLP1);
  liquidPump1On = (speedLP1 != 0);
  analogWrite(LIQUID_PUMP2, speedLP2);
  liquidPump2On = (speedLP2 != 0);
  analogWrite(LIQUID_PUMP3, speedLP3);
  liquidPump3On = (speedLP3 != 0);

  digitalWrite(ELECTROVALVE, LOW);*/
}
