/*
 * ADS1115 test code
 *
 * Author: Hao Zhou <zhh@umich.edu>
 * Date:   2020-12-8
 */

#include <wiringPi.h>
#include "ads1115.h"

ADS1115 ad;
ADS1115 *adPtr;

void setup(void) {
    // The ADC gain (PGA), Device operating mode, Data rate can be changed via the following functions
    ad.setGain(GAIN_TWO);          // 2x gain   +/- 2.048V 1 bit = 0.0625mV (default)
    //ad.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V 1 bit = 0.1875mV
    // ad.setGain(GAIN_ONE);       // 1x gain   +/- 4.096V 1 bit = 0.125mV
    // ad.setGain(GAIN_FOUR);      // 4x gain   +/- 1.024V 1 bit = 0.03125mV
    // ad.setGain(GAIN_EIGHT);     // 8x gain   +/- 0.512V 1 bit = 0.015625mV
    // ad.setGain(GAIN_SIXTEEN);   // 16x gain  +/- 0.256V 1 bit = 0.0078125mV

    ad.setMode(MODE_CONTIN);       // Continuous conversion mode
    // ad.setMode(MODE_SINGLE);    // Power-down single-shot mode (default)

    ad.setRate(RATE_128);          // 128SPS (default)
    // ad.setRate(RATE_8);         // 8SPS
    // ad.setRate(RATE_16);        // 16SPS
    // ad.setRate(RATE_32);        // 32SPS
    // ad.setRate(RATE_64);        // 64SPS
    // ad.setRate(RATE_250);       // 250SPS
    // ad.setRate(RATE_475);       // 475SPS
    // ad.setRate(RATE_860);       // 860SPS

    ad.setOSMode(OSMODE_SINGLE);   // Set to start a single-conversion
}

void loop(void) {
    int16_t results;

    // Be sure to update this value based on the IC and the gain settings!
    float multiplier = 0.0625;

    results = ad.measureDifferential();

    printf("Differential: %d (%f mV)\n", results, results * multiplier);
    delay(5);  // Delay milliseconds
}

/*
int main() {
    setup();
    while (1)
        loop();
    return 0;
}
*/

int main() {
    // Setup
    printf("Setting up WiringPi...\n");
    if (wiringPiSetup() < 0) {
        printf("Unable to setup wiringPi! Freezing.\n");
        while (1)
            ;
    }

    adPtr = new ADS1115();

    printf("Setting up ADS1115...\n");
    adPtr->configureRegisters();
    adPtr->setupDrdyISR();

    // Loop
    uint32_t lastTime = millis();
    while (millis() - lastTime < 10000)  // Collect 10 seconds of data
        adPtr->loop();

    adPtr->stopContinuousRead();

    return 0;
}
