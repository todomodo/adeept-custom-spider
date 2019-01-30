int _index=0;


void setup() {
  Serial.begin(9600); 
}


void loop() { 
  Serial.print(_index++);
  Serial.print("\n");  
   delay(500);
}
