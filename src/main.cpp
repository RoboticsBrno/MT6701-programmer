#include <Arduino.h>
#include <Wire.h>

uint8_t readRegister(uint8_t address) {
    Wire.beginTransmission(6);
    Wire.write(address);
    Wire.endTransmission();
    Wire.requestFrom(6, 1);
    return Wire.read();
}

void writeRegister(uint8_t address, uint8_t value) {
    Wire.beginTransmission(6);
    Wire.write(address);
    Wire.write(value);
    Wire.endTransmission();
}

uint16_t readAbzRes() {
    uint16_t reg1 = readRegister(0x30) & 0x3;
    uint16_t reg2 = readRegister(0x31);
    return (reg2 | (reg1 << 8)) + 1;
}

void programAbzRes(uint16_t resolution) {
    resolution -= 1;
    uint8_t reg30backup = readRegister(0x30) & 0xC;

    uint8_t bottom = resolution & 0xFF;
    uint8_t up = ((resolution >> 8) & 0x03) | reg30backup;
    writeRegister(0x30, up);
    writeRegister(0x31, bottom);

    writeRegister(0x09, 0xB3);
    writeRegister(0x0A, 0x05);
    delay(600);
}

void setup() {
    Serial.begin(9600);
    Serial.println("Start");

    // We use 6 and 7 as source of 5V
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);


    Wire.begin();

    Serial.print("ABZ reg1: ");
    Serial.println(readRegister(0x30), HEX);
    Serial.print("ABZ reg2: ");
    Serial.println(readRegister(0x31), HEX);

    if (readAbzRes() != 800) {
        programAbzRes(800);
        Serial.println("Finished, power cycle");
        while (true);
    }
}

void loop() {
    uint16_t reg1 = readRegister(3);
    uint16_t reg2 = readRegister(4);
    uint16_t pos = reg2 >> 2 | reg1 << 6;
    Serial.print("Pos: ");
    Serial.println(pos);

    delay(200);
}
