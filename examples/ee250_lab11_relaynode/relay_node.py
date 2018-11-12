from __future__ import print_function
import sys
import re
from time import sleep, time
from serial import Serial
import os
import paho.mqtt.client as paho
import math
import threading

quit = False
lock = threading.Lock()

# Default port that openmote connects to.
port_usb = '/dev/ttyUSB2'
try:
    conn = Serial(port_usb,
                    '115200',
                    dsrdtr=0,
                    rtscts=0,
                    timeout=0.1)
except IOError:
    print("Error opening serial port.", file=sys.stderr)
    sys.exit(2)

broker = "eclipse.usc.edu"
port = 11000

def on_publish(client,userdata,result):             #create function for callback
    print("Data Published \n")
    pass

def write(conn, command):
    global lock
    #print("LOCKED")
    lock.acquire()
    conn.write(bytearray(command, 'utf-8'))
    lock.release()
    #print("RELEASED")

def _tx_thread():
    global conn
    global quit
    send_addr = "ff02::1"
    send_port = 8850
    secs = 2
    send_data = "sleep"
    command = ""
    count = 0
    while(not quit):
        sleep(0.5);
        count += 1
        if(count == 10):
            command = "udp send " + send_addr + " " + str(send_port) + " " + send_data +"\n"
            write(conn, command)
            print("Multicasted nodes to stop for " + str(2) + " seconds")
            count = 0

def main():
    global conn
    global quit
    global lock

    conn.write(b'reboot\n')

    mcast_thread = threading.Thread(target=_tx_thread)
    mcast_thread.start()

    client1= paho.Client("client")                           #create client object
    client1.on_publish = on_publish                          #assign function to callback
    client1.connect(broker,port)                                 #establish connection

    # Rebooting node for safety.
    sleep(0.5)

    conn.write(b'ifconfig\n')

    line = ""
    while True:
        lock.acquire()
        line = conn.readline()
        lock.release()
        if b'HWaddr' in line:
            hwaddr = str(line).split('HWaddr: ')[1][:5]
            print("Hardware address is " + hwaddr)
            break
    
    data_recieved = False;
    command = ""

    sleep (0.5)

    command = "ifconfig 7 set power 7\n"
    write(conn, command)
    print("Power set to max")

    sleep(0.5);

    command = "ifconfig 7 set channel 26\n"
    write(conn, command)
    print("Channel set to 26")

    sleep(0.5);

    command = "udp server start 8808\n"
    write(conn, command)

    slash_split_data=[]
    colon_split_data=[]
    data=""

    # Line processing.
    try:
        while True:
            #lock.acquire()
            line = conn.readline()[:-1]
            #lock.release()
            # TDoA
            if b'SNIP  0' in line:
                data_recieved = True
                data = ""
            if data_recieved and not b'SNIP  1' in line:
                print(line.decode('utf-8'))
                data += line.decode('utf-8')[74:].strip()
                slash_split_data = data.split("/");
                colon_split_data = data.split(":");
            if data_recieved and b'SNIP  1' in line:
                print(line.decode('utf-8'))
                print("Data Recieved: " + data)
                if(len(slash_split_data) == 2 and len(colon_split_data)==2):  #check if valid
                    ret= client1.publish(colon_split_data[0].strip(),colon_split_data[1])       #publish
                data_recieved = False
    except KeyboardInterrupt:
        print("Quitting program.. closing all threads")
        quit = True
        mcast_thread.join()
        print("Everything successfully closed")
    

if __name__ == "__main__":
    main()
