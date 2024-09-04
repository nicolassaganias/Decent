#define DS18S20_TEMPERATURE_SENSOR_PIN     5        // set temperature sensor pin
#define H2_SENSOR_PIN                      35       // set hydrogen sensor pin

#define WATER_PRESSURE_SENSOR2_PIN         34       // set water pressure sensor pin
#define WATER_PRESSURE_SENSOR1_PIN         36       // set water pressure sensor pin

#define PH_SENSOR1_PIN                     39       // set pH sensor pin
#define PH_SENSOR2_PIN                     15       // set pH sensor pin

#define ELECTROVALVE1                      12
#define ELECTROVALVE2                      13
#define ELECTROVALVE3                      14

#define LIQUID_PUMP1                       27
#define LIQUID_PUMP2                       33

#define CH4_SENSOR_TX_PIN                  26
#define CH4_SENSOR_RX_PIN                  25

#define CO2_SENSOR1_RX_PIN                 4
#define CO2_SENSOR1_TX_PIN                 23
#define CO2_SENSOR2_RX_PIN                 18
#define CO2_SENSOR2_TX_PIN                 19

#define RELAY_PIN                          2 

#define ph_valve_on_time                    10000  // 10s
#define pump1_on_time                       5000  // 5s
#define pump2_on_time                       5000  // 5s

unsigned long last_valve1_update = 0;
unsigned long last_pump1_update = 0;
unsigned long last_pump2_update = 0;
bool valve1 = false, pump1 = false, pump2 = false;
unsigned long last_data_checked_time = 0;
unsigned long last_update = 0;
bool calibration_mode = false;

void pinConfiguration(){
  // set pins as input
  pinMode(DS18S20_TEMPERATURE_SENSOR_PIN, INPUT);
  pinMode(H2_SENSOR_PIN, INPUT);
  pinMode(WATER_PRESSURE_SENSOR1_PIN, INPUT);
  pinMode(WATER_PRESSURE_SENSOR2_PIN, INPUT);
  pinMode(PH_SENSOR1_PIN, INPUT);
  pinMode(PH_SENSOR2_PIN, INPUT);
  
  // set pins as output
  pinMode(ELECTROVALVE1, OUTPUT);
  pinMode(ELECTROVALVE2, OUTPUT);
  pinMode(ELECTROVALVE3, OUTPUT);
  pinMode(LIQUID_PUMP1, OUTPUT);
  pinMode(LIQUID_PUMP2, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
}