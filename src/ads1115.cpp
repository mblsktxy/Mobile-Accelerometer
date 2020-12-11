/*
 * Library for TI ADS1115 Analog-To-Digital Converter
 * Modified from https://github.com/adafruit/Adafruit_ADS1X15
 * Development environment specifics: Odroid XU4
 * Communication protocol: I2C
 *
 * Author: Hao Zhou <zhh@umich.edu>
 * Date:   2020-12-7
 */

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "ads1115.h"

ADS1115::ADS1115(const char *device):
    // All initialized to default values
    i2cAddress(ADS1115_ADDRESS_DEFAULT), conversionDelay(ADS1115_CONVERSION_DELAY),
    lowThreshold(0x8000), highThreshold(0x7FFF),
    adsOSMode(OSMODE_SINGLE), adsGain(GAIN_TWO), adsMode(MODE_SINGLE), adsRate(RATE_128),
    adsCompMode(COMPMODE_TRAD), adsCompPol(COMPPOL_LOW), adsCompLat(COMPLAT_NONLAT), adsCompQue(COMPQUE_NONE)
{
    fd = wiringPiI2CSetupInterface(device, i2cAddress);
    if (fd < 0) {
        printf("[ADS1115] Unable to open I2C interface! Freezing.\n");
        while (1)
            ;
    }

    fp = fopen("./data/adc_samples.txt", "w");
    if (fp == NULL) {
        printf("[ADS1115] Unable to open file! Freezing.\n");
        while (1)
            ;
    }

    // When configured as a conversion ready pin, ALERT/RDY continues to require a pull-up resistor.
    pinMode(ADS1115_DRDY, INPUT_PULLUP);
}

void ADS1115::setOSMode(adsOSMode_t osmode) {
    adsOSMode = osmode;
}

adsOSMode_t ADS1115::getOSMode() {
    return adsOSMode;
}

void ADS1115::setGain(adsGain_t gain) {
    adsGain = gain;
}

adsGain_t ADS1115::getGain() {
    return adsGain;
}

void ADS1115::setMode(adsMode_t mode) {
    adsMode = mode;
}

adsMode_t ADS1115::getMode() {
    return adsMode;
}

void ADS1115::setRate(adsRate_t rate) {
    adsRate = rate;
}

adsRate_t ADS1115::getRate() {
    return adsRate;
}

void ADS1115::setCompMode(adsCompMode_t compmode) {
    adsCompMode = compmode;
}

adsCompMode_t ADS1115::getCompMode() {
    return adsCompMode;
}

void ADS1115::setCompPol(adsCompPol_t comppol) {
    adsCompPol = comppol;
}

adsCompPol_t ADS1115::getCompPol() {
    return adsCompPol;
}

void ADS1115::setCompLat(adsCompLat_t complat) {
    adsCompLat = complat;
}

adsCompLat_t ADS1115::getCompLat() {
    return adsCompLat;
}

void ADS1115::setCompQue(adsCompQue_t compque) {
    adsCompQue = compque;
}

adsCompQue_t ADS1115::getCompQue() {
    return adsCompQue;
}

void ADS1115::setLowThreshold(int16_t threshold) {
    lowThreshold = threshold;
    lowThreshold = __bswap_16(lowThreshold);
    wiringPiI2CWriteReg16(fd, ADS1115_REG_POINTER_LOWTHRESH, lowThreshold);
}

int16_t ADS1115::getLowThreshold() {
    return lowThreshold;
}

void ADS1115::setHighThreshold(int16_t threshold) {
    highThreshold = threshold;
    highThreshold = __bswap_16(highThreshold);
    wiringPiI2CWriteReg16(fd, ADS1115_REG_POINTER_HITHRESH, highThreshold);
}

int16_t ADS1115::getHighThreshold() {
    return highThreshold;
}

// Reads the conversion results, measuring the voltage for a single-ended ADC reading from the specified channel
// Negative voltages cannot be applied to this circuit because the ADS1115 can only accept positive voltages
uint16_t ADS1115::measureSingleEnded(uint8_t channel) {
    if (channel > 3) {
        return 0;
    }

    // Start with default values
    uint16_t config = ADS1115_REG_CONFIG_CQUE_NONE    |  // Disable the comparator (default val)
                      ADS1115_REG_CONFIG_CLAT_NONLAT  |  // Non-latching           (default val)
                      ADS1115_REG_CONFIG_CPOL_ACTVLOW |  // Alert/Rdy active low   (default val)
                      ADS1115_REG_CONFIG_CMODE_TRAD;     // Traditional comparator (default val)

    // Set Operational status/single-shot conversion start
    config |= adsOSMode;

    // Set PGA/voltage range
    config |= adsGain;

    // Set Device operating mode
    config |= adsMode;

    // Set Data rate
    config |= adsRate;

    // Set single-ended input channel
    switch (channel) {
        case (0):
            config |= ADS1115_REG_CONFIG_MUX_SINGLE_0;
            break;
        case (1):
            config |= ADS1115_REG_CONFIG_MUX_SINGLE_1;
            break;
        case (2):
            config |= ADS1115_REG_CONFIG_MUX_SINGLE_2;
            break;
        case (3):
            config |= ADS1115_REG_CONFIG_MUX_SINGLE_3;
            break;
    }

    // Write config register to the ADC
    config = __bswap_16(config);
    wiringPiI2CWriteReg16(fd, ADS1115_REG_POINTER_CONFIG, config);
    // Wait for the conversion to complete
    delayMicroseconds(conversionDelay);

    // Read the conversion results
    uint16_t rawAdc = wiringPiI2CReadReg16(fd, ADS1115_REG_POINTER_CONVERSION);
    rawAdc = __bswap_16(rawAdc);
    return rawAdc;
}

// Reads the conversion results, measuring the voltage difference between the P (AIN#) and N (AIN#) input
// Generates a signed value since the difference can be either positive or negative
int16_t ADS1115::measureDifferential(uint8_t channel) {
    // Start with default values
    uint16_t config = ADS1115_REG_CONFIG_CQUE_NONE    |  // Disable the comparator (default val)
                      ADS1115_REG_CONFIG_CLAT_NONLAT  |  // Non-latching           (default val)
                      ADS1115_REG_CONFIG_CPOL_ACTVLOW |  // Alert/Rdy active low   (default val)
                      ADS1115_REG_CONFIG_CMODE_TRAD;     // Traditional comparator (default val)

    // Set Operational status/single-shot conversion start
    config |= adsOSMode;

    // Set PGA/voltage range
    config |= adsGain;

    // Set Device operating mode
    config |= adsMode;

    // Set Data rate
    config |= adsRate;

    // Set Differential input channel
    switch (channel) {
        case (01):
            config |= ADS1115_REG_CONFIG_MUX_DIFF_0_1;  // AIN0 = P, AIN1 = N
            break;
        case (03):
            config |= ADS1115_REG_CONFIG_MUX_DIFF_0_3;  // AIN0 = P, AIN3 = N
            break;
        case (13):
            config |= ADS1115_REG_CONFIG_MUX_DIFF_1_3;  // AIN1 = P, AIN3 = N
            break;
        case (23):
            config |= ADS1115_REG_CONFIG_MUX_DIFF_2_3;  // AIN2 = P, AIN3 = N
            break;
    }

    config = __bswap_16(config);
    // Write config register to the ADC
    wiringPiI2CWriteReg16(fd, ADS1115_REG_POINTER_CONFIG, config);
    // Wait for the conversion to complete
    delayMicroseconds(conversionDelay);

    // Read the conversion results
    int16_t rawAdc = wiringPiI2CReadReg16(fd, ADS1115_REG_POINTER_CONVERSION);
    rawAdc = __bswap_16(rawAdc);
    return rawAdc;
}

void ADS1115::configureRegisters() {
    printf("[ADS1115] Configuring registers...\n");

    setLowThreshold();
    delayMicroseconds(conversionDelay);
    setHighThreshold();
    delayMicroseconds(conversionDelay);

    // The COMP_POL bit continues to function and the COMP_QUE bits can disable the pin;
    // however, the COMP_MODE and COMP_LAT bits no longer control any function.
    // COMP_QUE bits should be anything other than '11' for the conversion ready (RDY) to show up at the ALERT/RDY pin output.
    // https://e2e.ti.com/support/data-converters/f/73/t/544531?ADS1115-RDY-pin-single-ended-conversion
    int16_t config = adsOSMode | ADS1115_REG_CONFIG_MUX_DIFF_0_1 | adsGain | ADS1115_REG_CONFIG_MODE_CONTIN | adsRate |
                     adsCompMode | ADS1115_REG_CONFIG_CPOL_ACTVHI | adsCompLat | ADS1115_REG_CONFIG_CQUE_1CONV;
    // Odroid XU4 byte order: little-endian, need to swap the bytes before read or write
    // Linux command: $ lscpu | grep "Byte Order"
    config = __bswap_16(config);
    wiringPiI2CWriteReg16(fd, ADS1115_REG_POINTER_CONFIG, config);
    delayMicroseconds(conversionDelay);

    printf("[ADS1115] Registers configuration done.\n");
}

void ADS1115::setupDrdyISR() {
    printf("[ADS1115] Setting up ADS1115 data ready interrupt...\n");
    if (wiringPiISR(ADS1115_DRDY, INT_EDGE_FALLING, &(ADS1115::drdy)) < 0) {
        printf("[ADS1115] Unable to setup ISR! Freezing.\n");
        while (1)
            ;
    }
    printf("[ADS1115] ISR setup done.\n");
    printf("[ADS1115] Data collection started...\n");
}

void ADS1115::stopContinuousRead() {
    printf("[ADS1115] Closing ADC output file...\n");
    fclose(fp);
    printf("[ADS1115] Data collection done.\n");
}

void ADS1115::drdy() {
    // Static wrapper function for interrupt
    adPtr->acquire();
}

void ADS1115::acquire() {
    // Do not call wiringPiI2CReadReg16 from ISR because it uses I2C library that needs interrupts to make it work,
    // interrupts would need to be re-enabled in the ISR, which is not a very good practice.
    continuousConversionReady = true;
}

void ADS1115::loop() {
    if (continuousConversionReady) {
        int16_t value = wiringPiI2CReadReg16(fd, ADS1115_REG_POINTER_CONVERSION);
        value = __bswap_16(value);
        fprintf(fp, "XU4 Microsecond: %u, ADC Sample: %d\n", micros(), value);
        continuousConversionReady = false;
    }
}
