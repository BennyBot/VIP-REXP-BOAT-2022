import serial
import serial.tools.list_ports
import matplotlib.pyplot as plt
import time
import random as r

arduino_ports = [
    p.device
    for p in serial.tools.list_ports.comports()
    if 'Arduino' in p.description  # may need tweaking to match new arduinos
]
if not arduino_ports:
    raise IOError("No Arduino found")
if len(arduino_ports) > 1:
    warnings.warn('Multiple Arduinos found - using the first')

ser = serial.Serial(arduino_ports[0])

plt.ion()
fig = plt.figure()
ax1 = fig.add_subplot(231)
ax2 = fig.add_subplot(232)
ax3 = fig.add_subplot(233)
ax4 = fig.add_subplot(234)
ax5 = fig.add_subplot(236)

subplots = [ax1,ax2,ax3,ax4,ax5]
sensors = ["Timestamp","Temperature","Turbidity","pH","Dissolved Oxygen","Electrical Conductivity"]

response = input("Display Output? (Y/n)")
display = response.lower().startswith("y")

output_name = input("Output File name : ")

output = open(output_name+".csv","w")
output.write(",".join(sensors) + "\n")
time.sleep(1)
while True:
    line = ser.readline().decode('utf-8').rstrip()
    '''
    line = [
    str(h),
    str(r.randint(25,50)),
    str(r.randint(0,10)),
    str(r.randint(0,14)),
    str(r.randint(0,100)),
    str(r.randint(0,10))
    ]
    
    line = ",".join(line)

    '''
    if len(line) == 0:
        continue
    sensorData = dict()
    data = line.split(",")
    for i in range(len(sensors)):
        try:
            sensorData[sensors[i]] = float(data[i])
        except:
            continue
    if display:
        for i in range(1,len(sensors)):
            subplots[i-1].scatter(sensorData["Timestamp"], sensorData[sensors[i]], c="red")
            if sensors[i] == "Turbidity":
                subplots[i-1].set_ylim([0,2000])
        fig.canvas.draw()
        fig.canvas.flush_events()
    output.write(line + "\n")

    if not plt.get_fignums():
        output.close()
        exit()
    #time.sleep(.1)
    #Print dictionary with labels if not collecting data
    #Print raw values if collecting data so it can be used with >> data.csv