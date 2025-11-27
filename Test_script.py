import serial
import keyboard
import time

PORT = "COM4"   # свой COM
BAUD = 9600

KEEPALIVE_MS = 60  # частота посылки команд при удержании

ser = serial.Serial(PORT, BAUD, timeout=0)

def send(id_char, cmd_char):
    ser.write(f"{id_char}{cmd_char}".encode("ascii"))

print("←/→ = мотор1, A/D=2, J/L=3, Z/X=4, SPACE=всем стоп, ESC=выход")

try:
    while True:
        start = time.time() * 1000

        # ---- мотор 1 ----
        if   keyboard.is_pressed('left'):  send('1','B')
        elif keyboard.is_pressed('right'): send('1','F')

        # ---- мотор 2 ----
        if   keyboard.is_pressed('a'): send('2','B')
        elif keyboard.is_pressed('d'): send('2','F')

        # ---- мотор 3 ----
        if   keyboard.is_pressed('j'): send('3','B')
        elif keyboard.is_pressed('l'): send('3','F')

        # ---- мотор 4 ----
        if   keyboard.is_pressed('z'): send('4','B')
        elif keyboard.is_pressed('x'): send('4','F')

        # ---- всем стоп ----
        if keyboard.is_pressed('space'):
            send('0','S')

        if keyboard.is_pressed('esc'):
            send('0','S')
            break

        # посылаем команды каждые KEEPALIVE_MS мс
        while (time.time() * 1000 - start) < KEEPALIVE_MS:
            time.sleep(0.001)

finally:
    send('0','S')
    ser.close()
