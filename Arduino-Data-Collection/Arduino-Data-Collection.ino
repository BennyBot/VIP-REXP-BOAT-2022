#define TURBIDITY_INPUT A0
#define DO_INPUT A1

/*
 * DO sensor documents/code needed : https://wiki.dfrobot.com/Gravity__Analog_Dissolved_Oxygen_Sensor_SKU_SEN0237
 * 
 */
#define CAL_DO_HIGH_T 30
#define CAL_DO_HIGH_V 2000
#define CAL_DO_LOW_T 25
#define CAL_DO_LOW_V 1500


double data[5] = {0,0,0,0,0};
const uint16_t DO_table[41] = {
    14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
    11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
    9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
    7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410};

 
void setup() {
  /*
   * Initialize all I/O pins
   * Temperature Sensor pins : None
   * Turbidity Sensor pins : None
   * pH Sensor pins : None
   * Dissolved Oxygen pins : None
   * Electrical conductivity pins : None
   */
  pinMode(TURBIDITY_INPUT, INPUT);
  pinMode(DO_INPUT, INPUT);
  Serial.begin(9600); // start serial communication
}

double get_temperature() {

  return 25;
}

double get_turbidity() {
  double analog = analogRead(TURBIDITY_INPUT);
  double voltage = analog * 5 / 1024
  double turbidity = -1120.4*voltage*voltage + 5742.3*voltage - 4352.9
  
  return turbidity;
}

double get_ph() {

  return 0;
}

double get_dissolved_oxygen() {
  double temperature = get_temperature();
  int raw = analogRead(DO_INPUT);
  int voltage = raw * 5000 / 1024;

  double saturation = (temperature - CAL_DO_LOW_T) * (CAL_DO_HIGH_V - CAL_DO_LOW_V) / (CAL_DO_HIGH_T - CAL_DO_LOW_T) + CAL_DO_LOW_V;
  double dissolved_oxygen = voltage * DO_table[(uint16_t)temperature] / saturation; 
  return dissolved_oxygen;
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
  unsigned long timestamp = millis();
  get_data();

  /*
   * Print the collected data over serial communication
   * Returned in CSV file format for easy data analysis.
   */
   
  String serialData = String(timestamp);
  for(int i=0; i<5; i++) {
    serialData.concat(",");
    serialData.concat(data[i]);
  }

  Serial.println(serialData);
  delay(100);
}
