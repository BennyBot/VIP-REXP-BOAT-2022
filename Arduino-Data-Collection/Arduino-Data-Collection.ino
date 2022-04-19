#include <OneWire.h>
#include <DallasTemperature.h>
#include <Servo.h>

#define MOTOR_ONE 13
#define MOTOR_TWO 10

#define DATA_ARRAY_SIZE 7

#define TDS_INPUT A1
#define ORP_INPUT A2
#define PH_INPUT A3
#define TURBIDITY_INPUT A4

#define TEMPERATURE_INPUT 2 // Digital pin for temperature sensor

#define PH_N_COLLECT 10
#define PH_COLLECT_TIME 200

/*
 * DO sensor documents/code needed : https://wiki.dfrobot.com/Gravity__Analog_Dissolved_Oxygen_Sensor_SKU_SEN0237
 * 
 */
#define CAL_DO_HIGH_T 30
#define CAL_DO_HIGH_V 2000
#define CAL_DO_LOW_T 25
#define CAL_DO_LOW_V 1500


double data[DATA_ARRAY_SIZE];

OneWire temperature_sensor(TEMPERATURE_INPUT);

Servo motor1;
Servo motor2;
void setup() {
  /*
   * Initialize all I/O pins
   */
  motor1.attach(MOTOR_ONE);
  motor2.attach(MOTOR_TWO);
  
  motor1.writeMicroseconds(1500);
  motor2.writeMicroseconds(1500);
  
  pinMode(TDS_INPUT, INPUT);
  pinMode(ORP_INPUT, INPUT);
  pinMode(PH_INPUT, INPUT);
  pinMode(TURBIDITY_INPUT, INPUT);
  pinMode(TEMPERATURE_INPUT, INPUT);
  delay(7000);
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

  //Serial.println("Got Temperature!");
  return TemperatureSum;
}


/*
 * Get the TDS sensor's input
 * 
 */
double get_tds() {
  double tds_v_average = 0;

  long next_collect_time = millis();
  int i=0;
  while(i < PH_N_COLLECT) {
    if(millis() >= next_collect_time) {
      tds_v_average += analogRead(PH_INPUT) / PH_N_COLLECT * 3.5 * 5 / 1024;
      next_collect_time = millis() + (PH_COLLECT_TIME / PH_N_COLLECT);
      i++;
    } 
  }
  // adjust TDS value based on temperature
  float tmpComp=1.0+0.02*(data[0]-25.0);
  float cmpV=tds_v_average/tmpComp;
  
  //Serial.println("Got TDS!");
  return (double)(133.42*cmpV*cmpV*cmpV - 255.86*cmpV*cmpV + 857.39*cmpV)*0.5; //convert voltage value to tds value

}

double get_orp() {
  return (double)(1.0);
}


/*
 * Get the turbidity of the water from the sensor
 */
double get_turbidity() {
  double analog = analogRead(TURBIDITY_INPUT);
  double voltage = analog * 5 / 1024;
  //return voltage;
  double turbidity = -500*voltage + 2100;

  //Serial.println("Got Turbidity!");
  return turbidity;
}


/*
 * Get the PH of the water from the sensor
 */
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
  //Serial.println("Got PH!");
  return ph_average;
}

/*
double get_dissolved_oxygen() {
  double temperature = get_temperature();
  int raw = analogRead(DO_INPUT);
  int voltage = raw * 5 / 1024;
  double saturation = (temperature - CAL_DO_LOW_T) * (CAL_DO_HIGH_V - CAL_DO_LOW_V) / (CAL_DO_HIGH_T - CAL_DO_LOW_T) + CAL_DO_LOW_V;
  double dissolved_oxygen = voltage * DO_table[(uint16_t)temperature] / saturation; 
  return dissolved_oxygen;
}
*/


/*
 * Get all of our data and store it in our data array
 */
void get_data() {
  data[0] = (double)(millis() - data[6]);
  data[6] = data[0] + data[6]; 
  data[1] = get_temperature();
  data[2] = get_turbidity();
  data[3] = get_tds();
  data[4] = get_orp();
  data[5] = get_ph();
}


void loop() {
  motor1.writeMicroseconds(1450);
  motor2.writeMicroseconds(1550);
  
  get_data();

  /*
   * Print the collected data over serial communication
   * Returned in CSV file format for easy data analysis.
   */
  
  String serialData = String(data[0]);
  for(int i=1; i<DATA_ARRAY_SIZE; i++) {
    serialData.concat(",");
    serialData.concat(data[i]);
  }
  Serial.println(serialData);

  

}
