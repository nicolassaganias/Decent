const float slope = 6.0 / 3.8;  // Slope calculated from calibration points
const float intercept = 3.5;    // y-intercept calculated from calibration points

#define ARRAY_LENGTH 40          // Number of readings to average
int pHArray1[ARRAY_LENGTH];      // Array to store readings for sensor 1
int pHArrayIndex1 = 0;           // Index for sensor 1 array
int pHArray2[ARRAY_LENGTH];      // Array to store readings for sensor 2
int pHArrayIndex2 = 0;           // Index for sensor 2 array


float ph1_data;
float ph2_data;

// Function to average readings for sensor 1
double averageArray1(int* arr, int number) {
  int i;
  int max, min;
  double avg;
  long amount = 0;
  if (number <= 0) {
    Serial.println("Error: Invalid number of elements for averaging.");
    return 0;
  }
  if (number < 5) {  // Less than 5 readings, calculate directly
    for (i = 0; i < number; i++) {
      amount += arr[i];
    }
    avg = amount / number;
    return avg;
  } else {
    if (arr[0] < arr[1]) {
      min = arr[0];
      max = arr[1];
    } else {
      min = arr[1];
      max = arr[0];
    }
    for (i = 2; i < number; i++) {
      if (arr[i] < min) {
        amount += min;  // arr[i] < min
        min = arr[i];
      } else if (arr[i] > max) {
        amount += max;  // arr[i] > max
        max = arr[i];
      } else {
        amount += arr[i];  // min <= arr[i] <= max
      }
    }
    avg = (double)amount / (number - 2);
  }
  return avg;
}

// Function to average readings for sensor 2
double averageArray2(int* arr, int number) {
  int i;
  int max, min;
  double avg;
  long amount = 0;
  if (number <= 0) {
    Serial.println("Error: Invalid number of elements for averaging.");
    return 0;
  }
  if (number < 5) {  // Less than 5 readings, calculate directly
    for (i = 0; i < number; i++) {
      amount += arr[i];
    }
    avg = amount / number;
    return avg;
  } else {
    if (arr[0] < arr[1]) {
      min = arr[0];
      max = arr[1];
    } else {
      min = arr[1];
      max = arr[0];
    }
    for (i = 2; i < number; i++) {
      if (arr[i] < min) {
        amount += min;  // arr[i] < min
        min = arr[i];
      } else if (arr[i] > max) {
        amount += max;  // arr[i] > max
        max = arr[i];
      } else {
        amount += arr[i];  // min <= arr[i] <= max
      }
    }
    avg = (double)amount / (number - 2);
  }
  return avg;
}

// Function to read pH from sensor 1
void readpH1() {
  pHArray1[pHArrayIndex1++] = analogRead(PH_SENSOR1_PIN);  // Read raw value
  if (pHArrayIndex1 == ARRAY_LENGTH) pHArrayIndex1 = 0;
  float voltage = averageArray1(pHArray1, ARRAY_LENGTH) * (3.3 / 4095.0);  // Convert raw value to voltage
  ph1_data = slope * voltage + intercept;  // Convert voltage to pH
}

// Function to read pH from sensor 2
void readpH2() {
  pHArray2[pHArrayIndex2++] = analogRead(PH_SENSOR2_PIN);  // Read raw value
  if (pHArrayIndex2 == ARRAY_LENGTH) pHArrayIndex2 = 0;
  float voltage = averageArray2(pHArray2, ARRAY_LENGTH) * (3.3 / 4095.0);  // Convert raw value to voltage
  ph2_data = slope * voltage + intercept;  // Convert voltage to pH
}
