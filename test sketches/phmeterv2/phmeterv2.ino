#define SensorPin 39  // pH meter Analog output to Arduino Analog Input 0
const float slope = 6.0 / 3.8; // slope calculated from calibration points
const float intercept = 3.5; // y-intercept calculated from calibration points

#define ArrayLenth 40           // times of collection
int pHArray[ArrayLenth];        // Store the average value of the sensor feedback
int pHArrayIndex = 0;

// average pH analog readings
double avergearray(int* arr, int number) {
  int i;
  int max, min;
  double avg;
  long amount = 0;
  if (number <= 0) {
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if (number < 5) {  //less than 5, calculated directly statistics
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
        amount += min;  //arr<min
        min = arr[i];
      } else {
        if (arr[i] > max) {
          amount += max;  //arr>max
          max = arr[i];
        } else {
          amount += arr[i];  //min<=arr<=max
        }
      }  //if
    }    //for
    avg = (double)amount / (number - 2);
  }  //if
  return avg;
}

void setup(){
  Serial.begin(115200);  pinMode(15, INPUT);
  Serial.println("Ready");    // Test the serial monitor
}

void loop() {
   pHArray[pHArrayIndex++] = analogRead(SensorPin);               // Read the raw value
   if (pHArrayIndex == ArrayLenth) pHArrayIndex = 0;
  float voltage = avergearray(pHArray, ArrayLenth) * (3.3 / 4095.0);   // Convert raw value to voltage
  float pHValue = slope * voltage + intercept;        // Convert voltage to pH using the equation
  
  Serial.print("pH: ");  
  Serial.println(pHValue, 2); // Print pH value with 2 decimal places
 // digitalWrite(13, HIGH);       
  delay(2000);
 // digitalWrite(13, LOW); 
}
