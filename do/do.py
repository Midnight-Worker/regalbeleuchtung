import serial
import threading

PORT = "COM39"
BAUD = 115200

ser = serial.Serial(PORT, BAUD, timeout=0.1)

def read_serial():
    while True:
        data = ser.readline()
        if data:
            print(data.decode(errors="replace").rstrip())

thread = threading.Thread(target=read_serial, daemon=True)
thread.start()

print("Serieller Monitor gestartet.")
print("Beispiele: r27,  y32,  g1,  o32,")
print("Beenden mit Strg+C")

while True:
    text = input("> ")
    ser.write((text + "\n").encode())
