#include <Arduino.h>
#include <SPI.h>
#include <constants.h>
#include <ad5940.h>

volatile uint32_t uCInterrupt = 0;

void IRAM_ATTR interruptISR(void) {
    uCInterrupt = 1;
}

void AD5940_RstClr(void) {
    digitalWrite(RESET, LOW);
}

void AD5940_RstSet(void) {
    digitalWrite(RESET, HIGH);
}

void AD5940_CsClr(void) {
    digitalWrite(CS, LOW);
}

void AD5940_CsSet(void) {
    digitalWrite(CS, HIGH);
}

void AD5940_Delay10us(uint32_t iTime) {
    if(iTime < 1638)
        delayMicroseconds(iTime * 10);
    else {
        delay(iTime / 100);
        delayMicroseconds((iTime % 100) * 10);
    }
}

void AD5940_ReadWriteNBytes(uint8_t *pWrData, uint8_t *pRdData, uint32_t iLen) {
    SPI.beginTransaction(SPISettings(CLCK, MSBFIRST, SPI_MODE0));
    for(uint32_t i = 0; i < iLen; i++) {
        pRdData[i] = SPI.transfer(pWrData[i]);
    }
    SPI.endTransaction();
}

uint32_t AD5940_MCUResourceInit(void) {
    Serial.begin(SERIAL_BAUD);
    SPI.begin(SCK, MISO, MOSI, CS);
    
    pinMode(CS, OUTPUT);
    pinMode(RESET, OUTPUT);
    pinMode(ESP32_INTERRUPT, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(ESP32_INTERRUPT), interruptISR, FALLING);
    
    AD5940_RstSet();
    AD5940_CsSet();
    
    return 0;
}

uint32_t AD5940_GetMCUIntFlag(void) {
    return uCInterrupt;
}

uint32_t AD5940_ClrMCUIntFlag(void) {
    uCInterrupt = 0;
    return 0;
}
