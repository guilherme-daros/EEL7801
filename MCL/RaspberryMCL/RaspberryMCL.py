from dbclass import Database
from parking import Parking
from serial import Serial
from time import sleep
import paho.mqtt.client as mqtt

fullParkingStateTopic = "0x0000ABCD"
updateParkingSpotStateTopic = "0xABCD0000"

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

client = mqtt.Client()
client.on_connect = on_connect
client.connect("mqtt.eclipseprojects.io", 1883, 60)

db = Database("AngeloniPark")
db.executeScript("schema.sql")
ser = Serial('COM4')

parkingSpots = Parking(2**3)

while True:
    if not client.is_connected:
        client.connect("mqtt.eclipseprojects.io", 1883, 60)
        
    line = str(ser.readline(), "UTF-8")
    # print(line)

    if ";" in line:
        line = line.split(";")
        [id, state] = line[0],line[1]
        # print(id,state)
        id = int(id)
        state = int(state)
        line = ""
    print(f"[MCL] Event: {state} on Spot {id}")
    parkingSpots.parkEvent(id, state)
    fullParkingStateMessage = f"0;{';'.join(str(e) for e in parkingSpots.spots)}"
    db.registerParkingEvent(id,state)
    client.publish(fullParkingStateTopic,fullParkingStateMessage)
    client.publish(updateParkingSpotStateTopic,f"{id};{state}")
    client.loop()

    

