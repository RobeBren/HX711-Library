#include <BR_HX711.h>


// Constructor
BR_HX711::BR_HX711(uint8_t dout_gpio, uint8_t sck_gpio) {

    _dout = dout_gpio;
    _sck = sck_gpio;
}

// Initalization of HX711
void BR_HX711::begin() {

    // Configure GPIO Pins
    pinMode(_sck, OUTPUT);
    pinMode(_dout, INPUT);

    // Set clock LOW initially
    digitalWrite(_sck, LOW);
}

int32_t BR_HX711::readRawData() {

    // Wait for conversion to finish
    while (digitalRead(_dout) == HIGH) {
        // Wait
    }

    // Collect 24-bit ADC output
    uint32_t rawData = 0;
    for (uint8_t i = 0; i < 24; i++) {
        digitalWrite(_sck, HIGH);
        delayMicroseconds(1);

        rawData = (rawData << 1) | digitalRead(_dout);

        digitalWrite(_sck, LOW);
        delayMicroseconds(1);
    }

    if (rawData == 0x7FFFFF) {
        _reading_status = Reading_Status::SATURATED_POSITIVE;
    }
    else if (rawData == 0x800000) {
        _reading_status = Reading_Status::SATURATED_NEGATIVE;
    }
    else {
        _reading_status = Reading_Status::NORMAL;
    }

    // Determine additional clock pulses
    uint8_t extraPulses = 0;

    switch (_channel_gain) {
        case Channel_Gain::CHANNEL_A_GAIN_128:
            extraPulses = 1;
            break;

        case Channel_Gain::CHANNEL_B_GAIN_32:
            extraPulses = 2;
            break;

        case Channel_Gain::CHANNEL_A_GAIN_64:
            extraPulses = 3;
            break;
    }

    // Select next channel and gain
    for (uint8_t i = 0; i < extraPulses; i++) {
        digitalWrite(_sck, HIGH);
        delayMicroseconds(1);
        digitalWrite(_sck, LOW);
        delayMicroseconds(1);
    }

    // Convert 24-bit two's complement to signed 32-bit
    int32_t signedData = static_cast<int32_t>(rawData);

    if (rawData & 0x800000UL) {
        signedData -= 0x1000000L;
    }

    return signedData;
}


void BR_HX711::setChannelGain(Channel_Gain channel_gain) {
    _channel_gain = channel_gain;
}


float BR_HX711::calculateInstantaneousWeight() {
    int32_t rawData = readRawData();

    return (rawData - _offset) / _calibration_factor;
}


float BR_HX711::calculateAverageWeight(uint8_t samples) {

    if (samples == 0) {
        return NAN;
    }

    int64_t rawDataSum;

    for (uint8_t i = 0; i < samples; i++) {
        rawDataSum += readRawData();
    }

    float averageRawData = (float)rawDataSum / samples;

    return (averageRawData - _offset) / _calibration_factor;
}

BR_HX711::Reading_Status BR_HX711::getReadingStatus() {
    return _reading_status;
}


void BR_HX711::powerDown() {
    digitalWrite(_sck, HIGH);
    delayMicroseconds(65);
}


void BR_HX711::powerUp() {
    digitalWrite(_sck, LOW);
    delay(400); // 10 SPS Startup Delay 400ms
}


void BR_HX711::setOffset(int32_t offset) {
    _offset = offset;
}


void BR_HX711::setCalibrationFactor(float calibration_factor) {
    if (calibration_factor != 0.0f) {
        _calibration_factor = calibration_factor;
    }
}
