#include "yboard.h"

YBoardV3 Yboard;

YBoardV3::YBoardV3() : strip(led_count, led_pin, NEO_GRB + NEO_KHZ800) {}

YBoardV3::~YBoardV3() {}

void YBoardV3::setup() {
    setup_leds();
    setup_switches();
    setup_buttons();
    setup_speaker();
    setup_accelerometer();
    setup_temperature();
}

////////////////////////////// LEDs ///////////////////////////////
void YBoardV3::setup_leds() {
    strip.begin();
    strip.clear();
    set_led_brightness(50);
}

void YBoardV3::set_led_color(uint16_t index, uint8_t red, uint8_t green, uint8_t blue) {
    strip.setPixelColor(index - 1, red, green, blue);
    strip.show();
}

void YBoardV3::set_led_brightness(uint8_t brightness) {
    strip.setBrightness(brightness);
    strip.show();
}

void YBoardV3::set_all_leds_color(uint8_t red, uint8_t green, uint8_t blue) {
    for (int i = 0; i < this->led_count; i++) {
        strip.setPixelColor(i, red, green, blue, false);
    }
    strip.show();
}

////////////////////////////// Switches ///////////////////////////////
void YBoardV3::setup_switches() {
    pinMode(this->switch1_pin, INPUT);
    pinMode(this->switch2_pin, INPUT);
}

bool YBoardV3::get_switch(uint8_t switch_idx) {
    switch (switch_idx) {
    case 1:
        return digitalRead(this->switch1_pin);
    case 2:
        return digitalRead(this->switch2_pin);
    default:
        return false;
    }
}

////////////////////////////// Buttons ///////////////////////////////
void YBoardV3::setup_buttons() {
    pinMode(this->button1_pin, INPUT);
    pinMode(this->button2_pin, INPUT);
}

bool YBoardV3::get_button(uint8_t button_idx) {
    switch (button_idx) {
    case 1:
        return !digitalRead(this->button1_pin);
    case 2:
        return !digitalRead(this->button2_pin);
    default:
        return false;
    }
}

////////////////////////////// Knob ///////////////////////////////
int YBoardV3::get_knob() {
    int value = map(analogRead(this->knob_pin), 2888, 8, 0, 100);
    value = max(0, value);
    value = min(100, value);
    return value;
}

////////////////////////////// Speaker/Tones //////////////////////////////////
bool YBoardV3::setup_speaker() {
    // Set microSD Card CS as OUTPUT and set HIGH
    pinMode(sd_cs_pin, OUTPUT);
    digitalWrite(sd_cs_pin, HIGH);

    // Initialize SPI bus for microSD Card
    SPI.begin(spi_sck_pin, spi_miso_pin, spi_mosi_pin);

    YAudio::setup();

    // Set Volume
    YAudio::set_wave_volume(5);

    // Start microSD Card
    if (!SD.begin(sd_cs_pin)) {
        Serial.println("Error accessing microSD card!");
        sd_card_present = false;
        return false;
    }

    sd_card_present = true;
    return true;
}

void YBoardV3::loop_speaker() { YAudio::loop(); }

bool YBoardV3::play_sound_file(const std::string &filename) {
    if (!play_sound_file_background(filename)) {
        return false;
    }

    while (is_audio_playing()) {
        loop_speaker();
    }

    return true;
}

bool YBoardV3::play_sound_file_background(const std::string &filename) {
    // Prepend filename with a / if it doesn't have one
    std::string _filename = filename;
    if (_filename[0] != '/') {
        _filename.insert(0, "/");
    }

    if (!sd_card_present) {
        Serial.println("ERROR: SD Card not present.");
        return false;
    }

    if (!SD.exists(_filename.c_str())) {
        Serial.println("File does not exist.");
        return false;
    }

    return YAudio::play_sound_file(_filename);
}

void YBoardV3::set_sound_file_volume(uint8_t volume) { YAudio::set_wave_volume(volume); }

bool YBoardV3::play_notes(const std::string &notes) {
    if (!play_notes_background(notes)) {
        return false;
    }

    while (is_audio_playing()) {
        loop_speaker();
    }

    return true;
}

bool YBoardV3::play_notes_background(const std::string &notes) { return YAudio::add_notes(notes); }

void YBoardV3::stop_audio() { YAudio::stop(); }

bool YBoardV3::is_audio_playing() { return YAudio::is_playing(); }

////////////////////////////// Accelerometer /////////////////////////////////////
bool YBoardV3::setup_accelerometer() {
    if (!wire_begin) {
        Wire.begin(sda_pin, scl_pin);
        wire_begin = true;
    }

    if (!accel.begin(accel_addr, Wire)) {
        Serial.println("WARNING: Accelerometer not detected.");
        return false;
    }

    return true;
}

bool YBoardV3::accelerometer_available() { return accel.available(); }

accelerometer_data YBoardV3::get_accelerometer() {
    accelerometer_data data;
    data.x = accel.getX();
    data.y = accel.getY();
    data.z = accel.getZ();
    return data;
}

// ////////////////////////////// Temperature /////////////////////////////////////
bool YBoardV3::setup_temperature() {
    if (!wire_begin) {
        Wire.begin(sda_pin, scl_pin);
        wire_begin = true;
    }

    if (!aht.begin(&Wire)) {
        Serial.println("WARNING: Could not find temperature sensor.");
        return false;
    }

    return true;
}

temperature_data YBoardV3::get_temperature() {
    temperature_data data;
    sensors_event_t h, t;
    aht.getEvent(&h, &t);

    data.temperature = t.temperature;
    data.humidity = h.relative_humidity;

    return data;
}
