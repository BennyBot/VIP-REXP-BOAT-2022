double data[5] = {0,0,0,0,0};

void setup() {
  /*
   * Initialize all I/O pins
   * Temperature Sensor pins : None
   * Turbidity Sensor pins : None
   * pH Sensor pins : None
   * Dissolved Oxygen pins : None
   * Electrical conductivity pins : None
   */
  
  Serial.begin(9600); // start serial communication
}

double get_temperature() {

  return 0;
}

double get_turbidity() {

  return 0;
}

double get_ph() {

  return 0;
}

double get_dissolved_oxygen() {

  return 0;
}

double get_electrical_conductivity() {

  return 0;
}

void get_data() {
  data[0] = get_temperature();
  data[1] = get_turbidity();
  data[2] = get_ph();
  data[3] = get_dissolved_oxygen();
  data[4] = get_electrical_conductivity();
}


void loop() {
  get_data();

  /*
   * Print the collected data over serial communication
   * Returned in CSV file format for easy data analysis.
   */
   
  String serialData = String(data[0]);
  for(int i=1; i<5; i++) {
    serialData.concat(",");
    serialData.concat(data[i]);
  }

  Serial.println(serialData);
  delay(100);
}
