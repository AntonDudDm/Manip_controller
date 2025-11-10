import serial
import keyboard
import time

PORT = "COM3"   # ← поставь СВОЙ порт, как в Arduino IDE
BAUD = 9600

ser = serial.Serial(PORT, BAUD, timeout=0)

print("← = BACK, → = FORWARD, C = STOP, Esc = exit")

try:
    while True:
        sent = False

        if keyboard.is_pressed('left'):
            ser.write(b'B')
            sent = True

        if keyboard.is_pressed('right'):
            ser.write(b'F')
            sent = True

        if keyboard.is_pressed('c'):
            ser.write(b'S')
            sent = True

        if keyboard.is_pressed('esc'):
            ser.write(b'S')
            break

        # если ничего не жали, не спамим
        if not sent:
            time.sleep(0.02)
        else:
            # частота отправки команд (регулируй)
            time.sleep(0.08)

finally:
    ser.close()
