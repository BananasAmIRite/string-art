import serial
import time


arduino = serial.Serial("COM5", baudrate=115200)
time.sleep(2) # wait for arduino to reset


with open("./bin/commands.txt", "r") as file:
    commands = [line.strip() for line in file.readlines()]

num = 0

for command in commands:
    print(f"Sending # {num}: {command}")
    arduino.write((command + "\n").encode())
    while True:
        aLine = arduino.readline().decode().strip()
        print("Arduino: " + aLine)
        if aLine == "DONE":
            break
    num += 1


arduino.close()
print("All commands sent")


