const brokerIp = "mqtt.eclipseprojects.io/mqtt";
const brokerPort = 80;
const username = "";
const password = "";
const fullParkingStateTopic = "0x0000ABCD";
const updateParkingSpotStateTopic = "0xABCD0000";

const client = new Paho.MQTT.Client(brokerIp, brokerPort, username);

const Colors = { red: "#ff0000", green: "#00ca00" };

const spotIdList = ["V1", "V2", "V3", "V4", "V5", "V6", "V7", "V8"];

window.onload = () => {
  spotList = spotIdList.reduce((accumulator, currentId) => {
    accumulator[currentId] = document.getElementById(currentId);
    return accumulator;
  }, {});
};

client.connect({
  onSuccess: () => {
    console.log("MQTT Connected Succesfully");
    client.subscribe(fullParkingStateTopic);
    client.subscribe(updateParkingSpotStateTopic);
  },
});

client.onConnectionLost = (responseObject) => {
  if (responseObject.errorCode !== 0) {
    console.log("onConnectionLost:" + responseObject.errorMessage);
  }
};

client.onMessageArrived = (message) => {
  const topic = message.destinationName;
  const data = message.payloadString.split(";");
  console.log(topic, data);
  const { V1, V2, V3, V4, V5, V6, V7, V8 } = spotList;
  const { red, green } = Colors;

  if (topic == fullParkingStateTopic) {
    client.unsubscribe(fullParkingStateTopic);
    V1.style.backgroundColor = data[1] == "1" ? red : green;
    V2.style.backgroundColor = data[2] == "1" ? red : green;
    V3.style.backgroundColor = data[3] == "1" ? red : green;
    V4.style.backgroundColor = data[4] == "1" ? red : green;
    V5.style.backgroundColor = data[5] == "1" ? red : green;
    V6.style.backgroundColor = data[6] == "1" ? red : green;
    V7.style.backgroundColor = data[7] == "1" ? red : green;
    V8.style.backgroundColor = data[8] == "1" ? red : green;
  } else {
    switch (data[0]) {
      case "1":
        V1.style.backgroundColor = data[1] == "1" ? red : green;
        break;
      case "2":
        V2.style.backgroundColor = data[1] == "1" ? red : green;
        break;
      case "3":
        V3.style.backgroundColor = data[1] == "1" ? red : green;
        break;
      case "4":
        V4.style.backgroundColor = data[1] == "1" ? red : green;
        break;
      case "5":
        V5.style.backgroundColor = data[1] == "1" ? red : green;
        break;
      case "6":
        V6.style.backgroundColor = data[1] == "1" ? red : green;
        break;
      case "7":
        V7.style.backgroundColor = data[1] == "1" ? red : green;
        break;
      case "8":
        V8.style.backgroundColor = data[1] == "1" ? red : green;
        break;
      default:
        break;
    }
  }
};
