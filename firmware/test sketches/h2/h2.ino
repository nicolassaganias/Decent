///Arduino Sample Code
void setup()
{
  Serial.begin(115200); //Set serial baud rate to 9600 bps
}
void loop(){
int val;
val=analogRead(35);//Read Gas value from analog 0
Serial.println(val,DEC);//Print the value to serial port
delay(100);
}