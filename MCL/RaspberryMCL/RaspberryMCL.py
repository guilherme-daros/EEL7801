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

parkingSpots = Parking(8)

while True:
    line = str(ser.readline(), "UTF-8")

    if ";" in line:
        [id, state] = line[0],line[2]
        id = int(id,base=16) # - 43981
        state = int(state,base=2)
        line = ""
    event = "Park" if state == 1 else "Leave"
    print(f"[MCL] Event: {event} on Spot {id}")
    parkingSpots.parkEvent(id, event)
    fullParkingStateMessage = f"0;{';'.join(str(e) for e in parkingSpots.spots)}"
    db.registerParkingEvent(id,event)
    client.publish(fullParkingStateTopic,fullParkingStateMessage)
    client.publish(updateParkingSpotStateTopic,f"{id};{event}")
    client.loop()

    

