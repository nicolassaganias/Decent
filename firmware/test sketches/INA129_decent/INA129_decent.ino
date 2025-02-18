#include <Wire.h>
#include <Adafruit_INA219.h>

// Crear un objeto INA219
Adafruit_INA219 ina219;

void setup(void) 
{
  // Configurar el monitor serial
  Serial.begin(115200);
  while (!Serial) {
    delay(1); // Esperar a que el monitor serial esté listo
  }

  Serial.println("Inicializando INA219...");

  // Configurar I²C en los pines 17 (SDA) y 32 (SCL)
  Wire.begin(17, 32);

  // Inicializar el INA219
  if (!ina219.begin()) {
    Serial.println("Error: no se pudo encontrar el chip INA219");
    while (1) {
      delay(10); // Bloquear si no se detecta el sensor
    }
  }

  Serial.println("INA219 inicializado correctamente.");
}

void loop(void) 
{
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

  // Leer los valores del sensor INA219
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);

  // Imprimir los resultados en el monitor serial
  Serial.print("Tensión en el bus:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Tensión en el shunt: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Tensión de carga:    "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Corriente:           "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Potencia:            "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println("");

  delay(2000); // Esperar 2 segundos antes de la siguiente lectura
}
