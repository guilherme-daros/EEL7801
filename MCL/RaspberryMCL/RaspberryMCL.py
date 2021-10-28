from dbclass import Database
from parking import Parking
from serial import Serial
from time import sleep
import paho.mqtt.client as mqtt

print("[MCL] Inicializando")
fullParkingStateTopic = "0x0000ABCD"
updateParkingSpotStateTopic = "0xABCD0000"

def on_connect(client, userdata, flags, rc):
    # print("Connected with result code "+str(rc))
    pass
# def on_publish(client, userdata, mid):
#  print(f"onPublish: {mid}")

client = mqtt.Client()
client.on_connect = on_connect
#client.on_publish = on_publish
client.connect("mqtt.eclipseprojects.io", 1883, 60)
client.loop()
db = Database("AngeloniPark")
ser = Serial('/dev/ttyUSB0')

parkingSpots = Parking(2**3)
print("[MCL] Inicialização Completa")
while True:
    if not client.is_connected:
        client.connect("mqtt.eclipseprojects.io", 1883, 60)

    line = str(ser.readline(),"UTF-8")
    #print(line)

    if ";" in line:
        line = line.split(";")
        [id, state] = line[0],line[1]
        # print(id,state)
        id = int(id)
        state = int(state)
        line = ""
        event = "Estacionou" if state == 1 else "Saiu"
        print(f"[MCL] Evento: {event} na Vaga {id}")
        parkingSpots.parkEvent(id, state)
        fullParkingStateMessage = f"0;{';'.join(str(e) for e in parkingSpots.spots)}"
        db.registerParkingEvent(id,state)
        message1 = client.publish(fullParkingStateTopic,fullParkingStateMessage,qos=1)
        message2 = client.publish(updateParkingSpotStateTopic,f"{id};{state}",qos=1)
        # print(f"message1.is_published: {message1.is_published()}")
        # print(f"message2.is_published: {message2.is_published()}")
        client.loop()
