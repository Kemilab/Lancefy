void setup(){ 
  Serial.begin(115200);
}

void loop()
{  
  int sensorValue;

 sensorValue=analogRead(A2);
 Serial.print("The voltage value:");
 Serial.print(sensorValue*5000/1023.0);
 Serial.println("mV");
 delay(1000);
 Serial.print("\n");
}