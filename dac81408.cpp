#include "dac81408.h"

// constructor 
DAC81408::DAC81408(int cspin, int rstpin, int ldacpin, SPIClass *spi, uint32_t spi_clock_hz) {
    _cs_pin = cspin;
    _spi = spi;

    //
    if(rstpin != -1) _rst_pin = rstpin;
    
    // LDAC operation is not implemented in the lib
    if(ldacpin != -1) _ldac_pin = ldacpin;
    
    _spi_settings = SPISettings(spi_clock_hz, MSBFIRST, SPI_MODE0);

    pinMode(_cs_pin, OUTPUT);
    digitalWrite(_rst_pin, HIGH);

    _spi->begin();
}

inline void DAC81408::cs_on() { 
    digitalWrite(_cs_pin, LOW); 
}

inline void DAC81408::cs_off() { 
    digitalWrite(_cs_pin, HIGH); 
}

int DAC81408::init() {
    // reset if the reset pin is defined
    // TODO: if not then issue a soft reset?
    if(_rst_pin!=-1) {
        pinMode(_rst_pin, OUTPUT);
        digitalWrite(_rst_pin, LOW);
        delay(1); 
        digitalWrite(_rst_pin, HIGH); 
        delay(1);
    } 

    // set SPICONFIG: DEV_PWDN=0, FSDO=1
    uint16_t def = TEMPALM_EN(1) | DACBUSY_EN(0) | CRCALM_EN(1) | DEV_PWDWN(0) | CRC_EN(0) | SDO_EN(1) | FSDO(1) | (0x2 << 6);
    write_reg(R_SPICONFIG, def);
    
    // reading SPICONFIG back
    return (read_reg(R_SPICONFIG) == def ? 0 : -1);

}

void DAC81408::write_reg(uint8_t reg, uint16_t wdata) {
    uint8_t lsb = ((uint16_t)wdata >> 0) & 0xFF;
    uint8_t msb = ((uint16_t)wdata >> 8) & 0xFF;

    _spi->beginTransaction(_spi_settings);
    cs_on();
    //delayMicroseconds(1);
    _spi->transfer(reg);
    _spi->transfer(msb);
    _spi->transfer(lsb);
    tcsh_delay();
    cs_off();
    _spi->endTransaction();
}

uint16_t DAC81408::read_reg(uint8_t reg) {
    uint8_t buf[3]; // 15-0

    _spi->beginTransaction(_spi_settings);
    cs_on();
    _spi->transfer( (RREG | reg) );
    _spi->transfer(0x00);
    _spi->transfer(0x00);
    tcsh_delay();
    cs_off();

    cs_on();
    buf[0] = _spi->transfer(0x00);
    buf[1] = _spi->transfer(0x00);
    buf[2] = _spi->transfer(0x00);
    tcsh_delay();
    cs_off();
    _spi->endTransaction();

    // check buf[0]

    uint16_t res = ((buf[1] << 8) | buf[2]);
    return res;
}

//******* Enable/Disable (Power up/down) a channel *******//
void DAC81408::set_ch_enabled(int ch, bool state) { // true/false = power ON/OFF
    uint16_t res = read_reg(R_DACPWDWN);
    uint16_t mask = 1 << (3 + ch);

    // if state==true, power up the channel
    if(state) write_reg(R_DACPWDWN, res &= ~mask);
    else write_reg(R_DACPWDWN, res |= mask);

    //Serial.print("dacpwdn = "); Serial.println(res, HEX);
}

bool DAC81408::get_ch_enabled(int ch) {
    uint16_t res = read_reg(R_DACPWDWN);
    return !(bool(((res >> ch) & 1)));
}

//************** Set/Get internal reference **************//
// Definir constantes para os valores de referência interna
const uint16_t INT_REF_ON = 0x0000;  // Ligar (0000000000000000)
const uint16_t INT_REF_OFF = 0x4000; // Desligar (0100000000000000)

void DAC81408::set_int_reference(bool state) {
    if (state) {
        write_reg(R_GENCONFIG, INT_REF_ON); // Ligar
    } else {
        write_reg(R_GENCONFIG, INT_REF_OFF); // Desligar
    }
}


// 0 => ref off, 1 => ref on, -1 => invalid
int DAC81408::get_int_reference() {
    int out = -1;
    uint16_t res = read_reg(R_GENCONFIG);
    if(res == 0x4000) out = 0;
    else if(res == 0) out = 1;
    return out;
}

//**************** Set Range of a channel ***************//
void DAC81408::set_range(int ch, ChannelRange range) {
    uint16_t mask = (0xffff >> (16-4)) << 4*ch;
    uint16_t write = (dacrange_reg & ~mask) | (( range << 4*ch )&mask);
    dacrange_reg = write;
    //Serial.print("range write -> "); Serial.println(write, HEX);

    write_reg(R_DACRANGE, write );
}


int DAC81408::get_range(int ch) {
    uint8_t val = (dacrange_reg >> 4*ch) & ((1UL << 4)-1);
    return val;
}

//**************** Setar um valor de saida no canal ***************//
void DAC81408::set_out(int ch, uint16_t val) {
    if (ch < 1 || ch > 8) {
        // Handle the error (e.g., return or throw an exception)
        return;
    }
    uint8_t address = 0x14 + (ch - 1);
    write_reg(address, val);
    
}

uint8_t DAC81408::get_out(uint8_t reg) {
    uint16_t val = read_reg(reg);
    return val;
}

//************* Set/get sync mode of a channel ************//
void DAC81408::set_sync(int ch, SyncMode mode) {
    uint16_t read = read_reg(R_SYNCCONFIG);
    //Serial.print("sync read -> "); Serial.println(read, HEX);
    if(mode==SYNC_LDAC) read |= 1UL << ch;
    else read &= ~(1UL << ch);
    write_reg(R_SYNCCONFIG, read);
    //Serial.print("sync wrote -> "); Serial.println(read, HEX);

}
