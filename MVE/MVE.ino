// Bibliotecas necessárias para funcionamento dos
// módulos de comunicação e medição
#include <EEPROM.h>
#include <SPI.h>
#include <mcp2515.h>

#include "Adafruit_VL53L0X.h"

#define MVE_HEIGHT 2500  // mm
#define HALT_TIME 5000   // ms
#define BASE_ADDR 0x0000ABCD
#define GreenLED 5
#define RedLED 6

typedef union ID {
    uint32_t full;
    byte partial[4];
};

// Instanciação dos objetos do sensor de distância e modulo de comunicação
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
MCP2515 comModule(10);

// Declaração do tipo da estrutura que irá armazenar o frame da comunicação CAN
struct can_frame bufferMessage;

//
bool isConfigured = false;
bool stringComplete = false;
String inString;
ID moduleId;

void setup() {
    Serial.begin(9600);
    Serial.println("[MVE] Inicializando");
    for (int i = 0; i < 4; i++) {
        moduleId.partial[i] = EEPROM.read(i);
    }

    // Inicializa e configura o módulo de comunicação
    comModule.reset();
    comModule.setBitrate(CAN_125KBPS);
    comModule.setNormalMode();

    if (moduleId.full != 0xFFFFFFFF) {
        isConfigured = true;
        Serial.print("[MVE] Dispositivo ja configurado com ID: ");
        Serial.println(moduleId.full, HEX);
    } else {
        Serial.println("[MVE] Dispositivo não configurado");
    }

    // Inicializa o Sensor de distância
    while (!lox.begin()) {
    };

    // LEDs
    pinMode(GreenLED, OUTPUT);
    pinMode(RedLED, OUTPUT);

    digitalWrite(GreenLED, LOW);
    digitalWrite(RedLED, LOW);

    Serial.println("[MVE] Inicialização Completa");
}

void loop() {
    if (!isConfigured && stringComplete) {
        moduleId.full = BASE_ADDR + inString.toInt();
        // Estabelece o ID da vaga de estacionamento
        bufferMessage.can_id = moduleId.full;
        bufferMessage.can_dlc = 1;

        for (int i = 0; i < 4; i++) {
            EEPROM.write(i, moduleId.partial[i]);
        }

        Serial.print("[MVE] Modulo configurado com ID: ");
        Serial.println(moduleId.full, HEX);

        stringComplete = false;
        isConfigured = true;
    }

    if (isConfigured) {
        VL53L0X_RangingMeasurementData_t Measurement;
        lox.rangingTest(&Measurement, false);

        if (Measurement.RangeStatus != 4 && isConfigured) {
            if (Measurement.RangeMilliMeter <= MVE_HEIGHT) {
                // Se tiver carro
                digitalWrite(GreenLED, LOW);
                digitalWrite(RedLED, HIGH);
                if (bufferMessage.data[0] ==
                    0b00000000) {  // Se não tinha carro
                    bufferMessage.data[0] = 0b00000001;
                    comModule.sendMessage(&bufferMessage);
                } else {
                    // Se tinha carro
                    bufferMessage.data[0] = 0b00000000;
                    delay(HALT_TIME);
                }
            } else {
                // Se não tiver carro
                digitalWrite(GreenLED, HIGH);
                digitalWrite(RedLED, LOW);
                if (bufferMessage.data[0] == 0b00000001) {
                    // Se não tinha carro
                    bufferMessage.data[0] = 0b00000001;
                    delay(HALT_TIME);
                } else {
                    // Se tinha carro
                    bufferMessage.data[0] = 0b00000000;
                    comModule.sendMessage(&bufferMessage);
                }
            }
        }
    }
}

void serialEvent() {
    while (Serial.available()) {
        char inChar = (char)Serial.read();
        inString += inChar;
        if (inChar == '\n') {
            stringComplete = true;
        }
    }
}
