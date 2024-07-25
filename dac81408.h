#ifndef DAC81408_H
#define DAC81408_H

#include "Arduino.h"
#include "SPI.h"

// Registers
#define R_NOP        0x00
#define R_DEVICEID   0x01
#define R_STATUS     0x02
#define R_SPICONFIG  0x03
#define R_GENCONFIG  0x04
#define R_BRDCONFIG  0x05
#define R_SYNCCONFIG 0x06
#define R_DACPWDWN   0x09
#define R_DACRANGE   0x0B
#define R_TRIGGER    0x0E
#define R_BRDCAST    0x0F
#define R_DACA       0x14
#define R_DACB       0x15
#define R_DACC       0x16
#define R_DACD       0x17
#define R_DACE       0x18
#define R_DACF       0x19
#define R_DACG       0x1A
#define R_DACH       0x1B


// SPICONFIG masks
#define TEMPALM_EN(x) (x << 11)
#define DACBUSY_EN(x) (x << 10)
#define CRCALM_EN(x)  (x << 9)
#define DEV_PWDWN(x)  (x << 5)
#define CRC_EN(x)     (x << 4)
#define SDO_EN(x)     (x << 2)
#define FSDO(x)       (x << 1)

// read mask
#define RREG 0xC0

class DAC81408 {
private:
    SPIClass *_spi;
    SPISettings _spi_settings;
    
    int _cs_pin;
    int _rst_pin;
    int _ldac_pin;

    inline void cs_on();
    inline void cs_off();
    
    inline void tcsh_delay() {
        delayMicroseconds(1);
    }

    void write_reg(uint8_t reg, uint16_t wdata);
    uint16_t read_reg(uint8_t reg);

    uint16_t dacrange_reg = 0;

public:
    enum ChannelRange {U_5  = 0b0000, U_10 = 0b0001, U_20 = 0b0010, U_40 = 0b0100,
                       B_5  = 0b1001, B_10 = 0b1010, B_20 = 0b1100, B_2V5 = 0b1110};

    enum SyncMode {SYNC_SS = 0, SYNC_LDAC};

    DAC81408(int cspin, int rstpin = -1, int ldacpin = -1,
             SPIClass *spi = &SPI, uint32_t spi_clock_hz = 8000000);

    int init();
    void set_ch_enabled(int ch, bool state);
    bool get_ch_enabled(int ch);

    void set_int_reference(bool state);
    int get_int_reference();

    void set_range(int ch, ChannelRange range);
    int get_range(int ch);

    void set_out(int ch, uint16_t val);
    uint8_t get_out(uint8_t reg);

    void set_sync(int ch, SyncMode mode);
};

#endif // DAC81408_H


#endif // DAC81408_H

