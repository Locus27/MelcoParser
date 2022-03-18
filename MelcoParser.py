import serial
import csv
import time
import os
# import shutil



configFile = open("d:/python/config.txt","r")
COMport = configFile.read()

print(COMport)

ser = serial.Serial()
ser.baudrate = 115200
ser.port = COMport
ser.timeout = 0.1
ser.dsrdtr = None

if ser.isOpen == True:
    ser.close()
ser.open()


# ser = serial.Serial(port = COMport, baudrate = 115200, timeout = 0.1, dsrdtr=None)
time.sleep(5)
with open('d:/python/data.csv', newline='') as csvfile:
    csvreader = csv.reader(csvfile, delimiter=',', quotechar='"')
    ser.write('open'.encode())
    print('open')
    time.sleep(1)
    echo = ser.readline().decode('utf-8')
    print(echo)
    if echo == 'ready':
        print('sending')
        time.sleep(2)
        for row in csvreader:
            tstring=str("#"+row[9]) #this is the msgTopic string
            print('topic: ' + tstring)
            ser.write(tstring.encode())
            time.sleep(1)
            echo = ser.readline().decode('utf-8')
            if echo == 'payload\r\n':
                pstring=str("!" + row[10]) #this is the msgPayload string
                print('payload: ' + pstring)
                ser.write(pstring.encode())
                time.sleep(1)
                ser.read_until(expected='next').decode('utf-8')
                echo = ser.readline().decode('utf-8')
                print(echo)
            time.sleep(1)
# rename and move the old file
fileTime = time.strftime("%Y%m%d%H%M")
newFileName = fileTime+'.csv'
os.rename("d:/python/data.csv", "d:/python/old/"+newFileName)

ser.close()