import serial
import matplotlib.pyplot as plt
import time
import random as r
plt.ion()
fig = plt.figure()
ax1 = fig.add_subplot(231)
ax2 = fig.add_subplot(232)
ax3 = fig.add_subplot(233)
ax4 = fig.add_subplot(234)
ax5 = fig.add_subplot(236)

subplots = [ax1,ax2,ax3,ax4,ax5]
ser = serial.Serial('/dev/ttyACM0', baudrate=9600,timeout=None)
sensors = ["Timestamp","Temperature","Turbidity","pH","Dissolved Oxygen","Electrical Conductivity"]

response = input("Display Output? (Y/n)")
display = response.lower().startswith("y")

output_name = input("Output File name : ")

output = open(output_name+".csv","w")
output.write(",".join(sensors) + "\n")
h = 0
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
    sensorData = dict()
    if not len(line) == 0:
        data = line.split(",")
        for i in range(len(sensors)):
            sensorData[sensors[i]] = float(data[i])
    if display:
        print("displaying")
        for i in range(1,len(sensors)):
            subplots[i-1].scatter(sensorData["Timestamp"], sensorData[sensors[i]], c="red")
        fig.canvas.draw()
        fig.canvas.flush_events()
    output.write(line + "\n")

    if not plt.get_fignums():
        output.close()
        exit()
    h+=1
    #time.sleep(.1)
    #Print dictionary with labels if not collecting data
    #Print raw values if collecting data so it can be used with >> data.csv