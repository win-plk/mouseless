import bluetooth
import pyautogui
# pip3 install pyautogui

device_name = "contactlessMouse"
# device_mac = "CC:50:E3:95:DF:7A"
device_mac = "0C:8B:95:A5:93:7A"
device_port = 1
distance_cursor = 50

def execute(cmd):
    if cmd[0] == "m":
        print(cmd[-1])
        if cmd[-1] == "u":
            pyautogui.moveRel(0, -distance_cursor, duration = 0.5)
        elif cmd[-1] == "d":
            pyautogui.moveRel(0, distance_cursor, duration = 0.5)
        elif cmd[-1] == "l":
            pyautogui.moveRel(-distance_cursor, 0, duration = 0.5)
        elif cmd[-1] == "r":
            pyautogui.moveRel(distance_cursor, 0, duration = 0.5)
    elif cmd[0] == "c":
        x,y = pyautogui.position()
        pyautogui.click(x, y)



# Find Address and Port
devices = bluetooth.discover_devices(lookup_names=True)
print("Finding device with name \"%s\"" % device_name)
# for device in devices:
#     if device[1] == device_name:
#         device_mac = device[0]
#         print("Found \"%s\" with MAC: %s" % (device_name, device[0]))
#         services = bluetooth.find_service(address=device_mac)
#         device_port = services[0]['port']
#         print("Found service port \"%s\" " % (device_port))

# Receive cmd from m5stickc via bluetooth
if device_mac != "":
    socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    socket.connect((device_mac, device_port))
    print("Connected! ")
    try:
        textcmd = ""
        while 1:
            data = socket.recv(1024)
            if data:
                # print(data)
                if data[-1] == 10:
                    textcmd = textcmd + data.decode()[:-1]
                    print(textcmd)
                    execute(textcmd)
                else:
                    textcmd = data.decode()
    except:	
        socket.close()
print("** END **")
