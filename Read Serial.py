import serial
ser = serial.Serial('/dev/ttyACM0', baudrate=9600,timeout=None)
sensors = ["Temperature","Turbidity","pH","Dissolved Oxygen","Electrical Conductivity"]
print("Data collection (DC) or print output (PO)")
response = input()
datacollect = response.startswith("D")
if datacollect:
    print(sensors.join(","))
while True:
    line = ser.readline().decode('utf-8').rstrip()
    sensorData = dict()
    if len(line) not 0:
        data = line.split(",")
        for i in range(len(sensors)):
            sensorData[sensors[i]] = data[i]

    print(line) if datacollect else print(sensorData)
    #Print dictionary with labels if not collecting data
    #Print raw values if collecting data so it can be used with >> data.csv
    

    