
#include <SoftwareSerial.h>

int bluetoothTx = 2;
int bluetoothRx = 3;
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);
void setup()
{
 Serial.begin(9600);
 bluetooth.begin(9600);
}
void loop(){
 if(bluetooth.available()){
   Serial.print((char)bluetooth.read());
 }
 if(Serial.available()){
   bluetooth.print((char)Serial.read());
 }
}
