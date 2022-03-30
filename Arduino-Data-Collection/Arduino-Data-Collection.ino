#include <OneWire.h>
#include <DallasTemperature.h>

#define TURBIDITY_INPUT A0
#define DO_INPUT A1
#define PH_INPUT A2
#define TEMPERATURE_INPUT 2 // Digital pin for temperature sensor

#define PH_N_COLLECT 5
#define PH_COLLECT_TIME 100

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

OneWire temperature_sensor(TEMPERATURE_INPUT);


void setup() {
  /*
   * Initialize all I/O pins
   * Temperature Sensor pins : Digital 2
   * Turbidity Sensor pins : Analog 0
   * pH Sensor pins : None
   * Dissolved Oxygen pins : Analog 1
   * Electrical conductivity pins : None
   */
  pinMode(TURBIDITY_INPUT, INPUT);
  pinMode(DO_INPUT, INPUT);
  pinMode(PH_INPUT, INPUT);
  Serial.begin(9600); // start serial communication
}

double get_temperature() {

  /*
   * References
   * 
   * Wiring diagram: https://randomnerdtutorials.com/guide-for-ds18b20-temperature-sensor-with-arduino/
   * - We need a 4.7kOhm resistor incase the one in the bag breaks
   * 
   * OneWire Code: https://wiki.dfrobot.com/Waterproof_DS18B20_Digital_Temperature_Sensor__SKU_DFR0198_
   */
  byte data[12];
  byte addr[8];

  if ( !temperature_sensor.search(addr)) {
      //no more sensors on chain, reset search
      temperature_sensor.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  temperature_sensor.reset();
  temperature_sensor.select(addr);
  temperature_sensor.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = temperature_sensor.reset();
  temperature_sensor.select(addr);
  temperature_sensor.write(0xBE); // Read Scratchpad


  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = temperature_sensor.read();
  }

  temperature_sensor.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;
}

double get_turbidity() {
  double analog = analogRead(TURBIDITY_INPUT);
  double voltage = analog * 5 / 1024;
  //return voltage;
  double turbidity = -500*voltage + 2100;
  
  return turbidity;
}

double get_ph() {
  double ph_average = 0;

  long next_collect_time = millis();
  int i=0;
  while(i < PH_N_COLLECT) {
    if(millis() >= next_collect_time) {
      ph_average += analogRead(PH_INPUT) / PH_N_COLLECT * 3.5 * 5 / 1024;
      next_collect_time = millis() + (PH_COLLECT_TIME / PH_N_COLLECT);
      i++;
    } 
  }
  
  return ph_average;
}

double get_dissolved_oxygen() {
  double temperature = get_temperature();
  int raw = analogRead(DO_INPUT);
  int voltage = raw * 5 / 1024;
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
  data[4] = -10;
}


void loop() {
  int count = 1;
  while(true) {
    
  get_data();

  /*
   * Print the collected data over serial communication
   * Returned in CSV file format for easy data analysis.
   */
   
  String serialData = String(count);
  for(int i=0; i<5; i++) {
    serialData.concat(",");
    serialData.concat(data[i]);
  }
  count++;
   Serial.println(serialData);
  delay(100);
  }

}
