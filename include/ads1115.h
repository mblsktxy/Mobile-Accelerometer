/* 
 * Library for TI ADS1115 Analog-To-Digital Converter
 * Modified from https://github.com/adafruit/Adafruit_ADS1X15
 * Development environment specifics: Odroid XU4
 * Communication protocol: I2C
 * 
 * Author: Hao Zhou <zhh@umich.edu>
 * Date:   2020-12-7
 */

#ifndef ADS1115_H
#define ADS1115_H

#include <stdio.h>

// I2C ADDRESS/BITS
#define ADS1115_ADDRESS_DEFAULT 0x48  // 1001 000 (ADDR = GND)
#define ADS1115_ADDRESS_VDD     0x49  // 1001 001 (ADDR = VDD)
#define ADS1115_ADDRESS_SDA     0x4A  // 1001 010 (ADDR = SDA)
#define ADS1115_ADDRESS_SCL     0x4B  // 1001 011 (ADDR = SCL)

// CONVERSION DELAY (in microsecond)
#define ADS1115_CONVERSION_DELAY 100  // Conversion delay

// POINTER REGISTER
#define ADS1115_REG_POINTER_CONVERSION 0x00  // Conversion
#define ADS1115_REG_POINTER_CONFIG     0x01  // Configuration
#define ADS1115_REG_POINTER_LOWTHRESH  0x02  // Low threshold
#define ADS1115_REG_POINTER_HITHRESH   0x03  // High threshold

// CONFIG REGISTER
#define ADS1115_REG_CONFIG_OS_MASK      0x8000  // OS Mask
#define ADS1115_REG_CONFIG_OS_NOEFFECT  0x0000  // Write: Bit = 0 No effect
#define ADS1115_REG_CONFIG_OS_SINGLE    0x8000  // Write: Bit = 1 Begin a single conversion
#define ADS1115_REG_CONFIG_OS_BUSY      0x0000  // Read: Bit = 0 Device is performing a conversion
#define ADS1115_REG_CONFIG_OS_NOTBUSY   0x8000  // Read: Bit = 1 Device is not performing a conversion

#define ADS1115_REG_CONFIG_MUX_MASK     0x7000  // Input multiplexer configuration
#define ADS1115_REG_CONFIG_MUX_DIFF_0_1 0x0000  // Differential P = AIN0, N = AIN1 (default)
#define ADS1115_REG_CONFIG_MUX_DIFF_0_3 0x1000  // Differential P = AIN0, N = AIN3
#define ADS1115_REG_CONFIG_MUX_DIFF_1_3 0x2000  // Differential P = AIN1, N = AIN3
#define ADS1115_REG_CONFIG_MUX_DIFF_2_3 0x3000  // Differential P = AIN2, N = AIN3
#define ADS1115_REG_CONFIG_MUX_SINGLE_0 0x4000  // Single-ended P = AIN0, N = GND
#define ADS1115_REG_CONFIG_MUX_SINGLE_1 0x5000  // Single-ended P = AIN1, N = GND
#define ADS1115_REG_CONFIG_MUX_SINGLE_2 0x6000  // Single-ended P = AIN2, N = GND
#define ADS1115_REG_CONFIG_MUX_SINGLE_3 0x7000  // Single-ended P = AIN3, N = GND

#define ADS1115_REG_CONFIG_PGA_MASK     0x0E00  // Programmable gain amplifier configuration
#define ADS1115_REG_CONFIG_PGA_6_144V   0x0000  // +/-6.144V range = Gain 2/3, 1 bit = 0.1875mV
#define ADS1115_REG_CONFIG_PGA_4_096V   0x0200  // +/-4.096V range = Gain 1,   1 bit = 0.125mV
#define ADS1115_REG_CONFIG_PGA_2_048V   0x0400  // +/-2.048V range = Gain 2,   1 bit = 0.0625mV (default)
#define ADS1115_REG_CONFIG_PGA_1_024V   0x0600  // +/-1.024V range = Gain 4,   1 bit = 0.03125mV
#define ADS1115_REG_CONFIG_PGA_0_512V   0x0800  // +/-0.512V range = Gain 8,   1 bit = 0.015625mV
#define ADS1115_REG_CONFIG_PGA_0_256V   0x0A00  // +/-0.256V range = Gain 16,  1 bit = 0.0078125mV

#define ADS1115_REG_CONFIG_MODE_MASK    0x0100  // Device operating mode
#define ADS1115_REG_CONFIG_MODE_CONTIN  0x0000  // Continuous conversion mode
#define ADS1115_REG_CONFIG_MODE_SINGLE  0x0100  // Power-down single-shot mode (default)

#define ADS1115_REG_CONFIG_DR_MASK      0x00E0  // Data rate
#define ADS1115_REG_CONFIG_DR_8SPS      0x0000  // 8 samples per second
#define ADS1115_REG_CONFIG_DR_16SPS     0x0020  // 16 samples per second
#define ADS1115_REG_CONFIG_DR_32SPS     0x0040  // 32 samples per second
#define ADS1115_REG_CONFIG_DR_64SPS     0x0060  // 64 samples per second
#define ADS1115_REG_CONFIG_DR_128SPS    0x0080  // 128 samples per second (default)
#define ADS1115_REG_CONFIG_DR_250SPS    0x00A0  // 250 samples per second
#define ADS1115_REG_CONFIG_DR_475SPS    0x00C0  // 475 samples per second
#define ADS1115_REG_CONFIG_DR_860SPS    0x00E0  // 860 samples per second

#define ADS1115_REG_CONFIG_CMODE_MASK   0x0010  // Comparator mode
#define ADS1115_REG_CONFIG_CMODE_TRAD   0x0000  // Traditional comparator with hysteresis (default)
#define ADS1115_REG_CONFIG_CMODE_WINDOW 0x0010  // Window comparator

#define ADS1115_REG_CONFIG_CPOL_MASK    0x0008  // Comparator polarity
#define ADS1115_REG_CONFIG_CPOL_ACTVLOW 0x0000  // ALERT/RDY pin is low when active (default)
#define ADS1115_REG_CONFIG_CPOL_ACTVHI  0x0008  // ALERT/RDY pin is high when active

#define ADS1115_REG_CONFIG_CLAT_MASK    0x0004  // Latching comparator
#define ADS1115_REG_CONFIG_CLAT_NONLAT  0x0000  // Non-latching comparator (default)
#define ADS1115_REG_CONFIG_CLAT_LATCH   0x0004  // Latching comparator

#define ADS1115_REG_CONFIG_CQUE_MASK    0x0003  // Comparator queue and disable
#define ADS1115_REG_CONFIG_CQUE_1CONV   0x0000  // Assert ALERT/RDY after one conversions
#define ADS1115_REG_CONFIG_CQUE_2CONV   0x0001  // Assert ALERT/RDY after two conversions
#define ADS1115_REG_CONFIG_CQUE_4CONV   0x0002  // Assert ALERT/RDY after four conversions
#define ADS1115_REG_CONFIG_CQUE_NONE    0x0003  // Disable the comparator and put ALERT/RDY in high state (default)

#define ADS1115_DEVICE "/dev/i2c-1"  // I2C device
#define ADS1115_DRDY 2  // Data ready interrupt pin

typedef enum {
    OSMODE_SINGLE   = ADS1115_REG_CONFIG_OS_SINGLE,
    OSMODE_BUSY     = ADS1115_REG_CONFIG_OS_BUSY,
    OSMODE_NOTBUSY  = ADS1115_REG_CONFIG_OS_NOTBUSY
} adsOSMode_t;

typedef enum {
    GAIN_TWOTHIRDS  = ADS1115_REG_CONFIG_PGA_6_144V,
    GAIN_ONE        = ADS1115_REG_CONFIG_PGA_4_096V,
    GAIN_TWO        = ADS1115_REG_CONFIG_PGA_2_048V,
    GAIN_FOUR       = ADS1115_REG_CONFIG_PGA_1_024V,
    GAIN_EIGHT      = ADS1115_REG_CONFIG_PGA_0_512V,
    GAIN_SIXTEEN    = ADS1115_REG_CONFIG_PGA_0_256V
} adsGain_t;

typedef enum {
    MODE_CONTIN     = ADS1115_REG_CONFIG_MODE_CONTIN,
    MODE_SINGLE     = ADS1115_REG_CONFIG_MODE_SINGLE
} adsMode_t;

typedef enum {
    RATE_8          = ADS1115_REG_CONFIG_DR_8SPS,
    RATE_16         = ADS1115_REG_CONFIG_DR_16SPS,
    RATE_32         = ADS1115_REG_CONFIG_DR_32SPS,
    RATE_64         = ADS1115_REG_CONFIG_DR_64SPS,
    RATE_128        = ADS1115_REG_CONFIG_DR_128SPS,
    RATE_250        = ADS1115_REG_CONFIG_DR_250SPS,
    RATE_475        = ADS1115_REG_CONFIG_DR_475SPS,
    RATE_860        = ADS1115_REG_CONFIG_DR_860SPS
} adsRate_t;

typedef enum {
    COMPMODE_TRAD   = ADS1115_REG_CONFIG_CMODE_TRAD,
    COMPMODE_WINDOW = ADS1115_REG_CONFIG_CMODE_WINDOW
} adsCompMode_t;

typedef enum {
    COMPPOL_LOW     = ADS1115_REG_CONFIG_CPOL_ACTVLOW,
    COMPPOL_HIGH    = ADS1115_REG_CONFIG_CPOL_ACTVHI
} adsCompPol_t;

typedef enum {
    COMPLAT_NONLAT  = ADS1115_REG_CONFIG_CLAT_NONLAT,
    COMPLAT_LATCH   = ADS1115_REG_CONFIG_CLAT_LATCH
} adsCompLat_t;

typedef enum {
    COMPQUE_ONE     = ADS1115_REG_CONFIG_CQUE_1CONV,
    COMPQUE_TWO     = ADS1115_REG_CONFIG_CQUE_2CONV,
    COMPQUE_FOUR    = ADS1115_REG_CONFIG_CQUE_4CONV,
    COMPQUE_NONE    = ADS1115_REG_CONFIG_CQUE_NONE
} adsCompQue_t;


class ADS1115 {
public:
    ADS1115(const char *device = ADS1115_DEVICE);

    void setOSMode(adsOSMode_t osmode);
    adsOSMode_t getOSMode(void);
    void setGain(adsGain_t gain);
    adsGain_t getGain(void);
    void setMode(adsMode_t mode);
    adsMode_t getMode(void);
    void setRate(adsRate_t rate);
    adsRate_t getRate(void);
    void setCompMode(adsCompMode_t compmode);
    adsCompMode_t getCompMode(void);
    void setCompPol(adsCompPol_t comppol);
    adsCompPol_t getCompPol(void);
    void setCompLat(adsCompLat_t complat);
    adsCompLat_t getCompLat(void);
    void setCompQue(adsCompQue_t compque);
    adsCompQue_t getCompQue(void);
    // The ALERT/RDY pin is configured as a conversion ready pin when MSB (bit) of the high threshold register is set to
    // '1' and the MSB (bit) of the low threshold register is set to '0'.
    // Lo_thresh default = 8000h, Hi_thresh default = 7FFFh.
    void setLowThreshold(int16_t threshold = 0x0000);
    void setHighThreshold(int16_t threshold = 0xFFFF);
    int16_t getLowThreshold();
    int16_t getHighThreshold();

    uint16_t measureSingleEnded(uint8_t channel = 0);
    int16_t measureDifferential(uint8_t channel = 1);
    
    void configureRegisters();
    void setupDrdyISR();
    void stopContinuousRead();
    void acquire();
    static void drdy();
    void loop();

private:
    int fd;
    FILE *fp;
    uint8_t i2cAddress;
    uint8_t conversionDelay;
    int16_t lowThreshold;
    int16_t highThreshold;
    adsOSMode_t adsOSMode;
    adsGain_t adsGain;
    adsMode_t adsMode;
    adsRate_t adsRate;
    adsCompMode_t adsCompMode;
    adsCompPol_t adsCompPol;
    adsCompLat_t adsCompLat;
    adsCompQue_t adsCompQue;

    volatile bool continuousConversionReady = false;
};

// Globally available instance of ADS1115 (for statical interrupt handler)
extern ADS1115 *adPtr;

#endif
