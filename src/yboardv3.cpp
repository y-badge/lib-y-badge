#include "yboardv3.h"

YBoardV3::YBoardV3() : YBoard(5, 20, 9) {}

YBoardV3::~YBoardV3() {}

void YBoardV3::setup() {
    YBoard::setup();

    setup_switches();
    setup_buttons();
    setup_speaker();
}

YBoard::BoardType YBoardV3::get_type() { return BoardType::v3; }

////////////////////////////// Switches ///////////////////////////////
void YBoardV3::setup_switches() {
    pinMode(this->switch1_pin, INPUT);
    pinMode(this->switch2_pin, INPUT);
}

bool YBoardV3::get_switch(uint8_t switch_idx) {
    switch (switch_idx) {
    case 1:
        return !digitalRead(this->switch1_pin);
    case 2:
        return !digitalRead(this->switch2_pin);
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


////////////////////////////// Speaker/Tones //////////////////////////////////
void YBoardV3::setup_speaker() {
    // Set microSD Card CS as OUTPUT and set HIGH
    pinMode(sd_cs_pin, OUTPUT);
    digitalWrite(sd_cs_pin, HIGH);

    // Initialize SPI bus for microSD Card
    SPI.begin(spi_sck_pin, spi_miso_pin, spi_mosi_pin);

    // Start microSD Card
    if (!SD.begin(sd_cs_pin)) {
        Serial.println("Error accessing microSD card!");
    }

    // Setup I2S
    audio.setPinout(i2s_bclk_pin, i2s_lrc_pin, i2s_dout_pin);

    // Make the volume out of 100
    audio.setVolumeSteps(100);

    // Set Volume
    audio.setVolume(25);
}

void YBoardV3::loop_speaker() { audio.loop(); }

void YBoardV3::play_song_from_sd(const char *filename) { audio.connecttoFS(SD, filename); }

void YBoardV3::set_speaker_volume(uint8_t volume) { audio.setVolume(volume); }
