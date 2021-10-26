#include <SPI.h>
#include <mcp2515.h>

union ID {
    uint32_t full;
    byte partial[4];
};

MCP2515 comModule(10);

struct can_frame bufferMessage;
struct can_frame newIdFrame;

void setup() {
    Serial.begin(9600);

    // Inicializa e configura o módulo de comunicação
    comModule.reset();
    comModule.setBitrate(CAN_125KBPS);
    comModule.setNormalMode();
}

void loop() {
    if (comModule.readMessage(&bufferMessage) == MCP2515::ERROR_OK) {
        Serial.print(bufferMessage.can_id);
        Serial.print(";");
        Serial.print(bufferMessage.data[0]);
        Serial.print("\n");
    }
}
