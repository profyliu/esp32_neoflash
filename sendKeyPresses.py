import socket
import keyboard
PORT = 2880
remoteIP = "192.168.4.1"
remotePort = 2880
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(('', PORT))
def kbdCallback(e):
    val = e.scan_code
    if val not in [1, 14, 27, 26, 72, 75, 77, 80, 29, 42, 54]:  # ESC [ ] arrow keys
        val = 8
    s.sendto(val.to_bytes(1,'little'), (remoteIP,remotePort))

keyboard.on_press(kbdCallback)
keyboard.wait()