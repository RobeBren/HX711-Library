/*
 * ***************************************************************************
 * Component:    HX711 - 24-Bit Analog-to-Digital Converter
 * Manufacturer: Avia Semiconductor
 *
 * Product Page:
 *     https://www.sparkfun.com/sparkfun-load-cell-amplifier-hx711.html
 *
 * Datasheet:
 *     https://cdn.sparkfun.com/datasheets/Sensors/ForceFlex/hx711_english.pdf
 *
 * ***************************************************************************
 */
#ifndef BR_HX711_H
#define BR_HX711_H

#include <Arduino.h>

class BR_HX711 {

    public:

        // Allowed Channel Gains for calculating weight
        enum class Channel_Gain {
            CHANNEL_A_GAIN_128 = 25,
            CHANNEL_B_GAIN_32  = 26,
            CHANNEL_A_GAIN_64  = 27
        };

        enum class Reading_Status {
            NORMAL,
            SATURATED_POSITIVE,
            SATURATED_NEGATIVE
        };

        // Constructor
        BR_HX711(uint8_t dout_gpio, uint8_t sck_gpio);

        // Public Functions
        void begin();
        void setChannelGain(Channel_Gain channel_gain);
        float calculateInstantaneousWeight();
        float calculateAverageWeight(uint8_t samples);
        Reading_Status getReadingStatus();
        void powerDown();
        void powerUp();
        void setOffset(int32_t offset);
        void setCalibrationFactor(float calibration_factor);

    private:
        // GPIO Pins
        uint8_t _dout;
        uint8_t _sck;

        // Calibration values
        int32_t _offset = 0;
        float _calibration_factor = 1.0f;

        // Configuration
        Channel_Gain _channel_gain = Channel_Gain::CHANNEL_A_GAIN_128;

        // Private Variable
        Reading_Status _reading_status = Reading_Status::NORMAL;

        // Private Functions
        int32_t readRawData();

};

#endif
