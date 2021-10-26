// Bibliotecas necessárias para funcionamento dos
// módulos de comunicação e medição
#include <EEPROM.h>
#include <SPI.h>
#include <mcp2515.h>

#include "Adafruit_VL53L0X.h"

#define MVE_HEIGHT 300  // mm
#define HALT_TIME 5000  // ms
#define BASE_ADDR 0x00000000
#define GreenLED 5
#define RedLED 6

union ID {
    uint32_t full;
    byte partial[4];
};

// Instanciação dos objetos do sensor de distância e modulo de comunicação
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
MCP2515 comModule(10);

// Declaração do tipo da estrutura que irá armazenar o frame da comunicação CAN
struct can_frame bufferMessage;

// aux stuff
bool isConfigured = false;
bool stringComplete = false;
String inString;
ID moduleId;

enum STATE { OCUPADO, LIVRE };
STATE state = OCUPADO;
STATE last_state = LIVRE;

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
        state = getState(Measurement);
        // Serial.print("[MVE] Distancia medida: ");
        // Serial.println(Measurement.RangeMilliMeter);
        // Serial.print(last_state);
        // Serial.print(" | ");
        // Serial.println(state);
        delay(100);
        if (last_state != state) {
            last_state = state;
            if (state == LIVRE) {
                digitalWrite(GreenLED, HIGH);
                digitalWrite(RedLED, LOW);
                bufferMessage.can_id = moduleId.full;
                bufferMessage.can_dlc = 1;
                bufferMessage.data[0] = 0;
                comModule.sendMessage(&bufferMessage);
                Serial.println("[MVE] Estado da vaga: Livre");
            } else {
                digitalWrite(GreenLED, LOW);
                digitalWrite(RedLED, HIGH);
                bufferMessage.can_id = moduleId.full;
                bufferMessage.can_dlc = 1;
                bufferMessage.data[0] = 1;
                comModule.sendMessage(&bufferMessage);
                Serial.println("[MVE] Estado da vaga: Ocupado");
            }
        }
    }
    // delay(10);
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

STATE getState(VL53L0X_RangingMeasurementData_t Measurement) {
    if (Measurement.RangeStatus != 4) {
        return OCUPADO;
    } else {
        return LIVRE;
    }
}