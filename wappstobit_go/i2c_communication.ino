#include <Arduino.h>
#include <ArduinoJson.h>
#include "driver/i2c.h"

#include "i2c_communication.h"
#include "wappstobit_go.h"

#define SDA_PIN 32
#define SCL_PIN 33
#define I2C_SLAVE_ADDR 0x11

#define JSON_STATIC_BUFFER_SIZE 2000
#define JSON_SEND_BUFFER 300

#define I2C_BUFFER_LENGTH 5000

i2c_port_t portNum = I2C_NUM_0;

char content_buffer[2000] = {0,};
uint8_t i2c_read_buffer[I2C_BUFFER_LENGTH] = {0};
uint8_t i2c_write_buffer[I2C_BUFFER_LENGTH] = {0,};

//#define ENABLE_DEBUG 1

void configure_i2c_slave(void)
{
    i2c_config_t config;
    config.sda_io_num = gpio_num_t(SDA_PIN);
    config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    config.scl_io_num = gpio_num_t(SCL_PIN);
    config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    config.mode = I2C_MODE_SLAVE;
    config.slave.addr_10bit_en = 0;
    config.slave.slave_addr = I2C_SLAVE_ADDR;

    esp_err_t res = i2c_param_config(portNum, &config);
    if (res != ESP_OK) {
        Serial.println("Invalid I2C parameters or no microbit");
    }

    res = i2c_driver_install(portNum, config.mode, 2 * I2C_BUFFER_LENGTH, 2 * I2C_BUFFER_LENGTH, 0);
}

void i2c_slave_send_data(void)
{
    i2c_reset_tx_fifo(portNum);
    i2c_slave_write_buffer(portNum, i2c_write_buffer, strlen((const char*)i2c_write_buffer)+1, 0);
}

bool i2c_slave_data_available(void)
{
    int len = 0;
    static uint16_t content_length = 0;
    int16_t read_len = 0;

    read_len = i2c_slave_read_buffer(portNum, &i2c_read_buffer[content_length], I2C_BUFFER_LENGTH, 5);
    content_length += read_len;

    if (read_len <= 0) {
        return false;
    } else {
        for(int i=0; i<content_length; i++) {
            if(i2c_read_buffer[i] == 0x00) {
                len = i+1;
                content_length -= len;
                memcpy((void *) content_buffer, (void *) i2c_read_buffer, len);
                memmove((void *) i2c_read_buffer, (void *) i2c_read_buffer+len, content_length);
#ifdef ENABLE_DEBUG
                Serial.print("I2C: ");
                Serial.println((char*)content_buffer);
#endif
                return true;
            }
        }
        return false;
    }
}

void parse_json(void)
{
    StaticJsonDocument<JSON_STATIC_BUFFER_SIZE> root;
    DeserializationError error = deserializeJson(root, content_buffer);

    if(error) {
        Serial.print("Invalid json: ");
        Serial.println(error.f_str());
        return;
    }

    if(root.containsKey("device")) {
        int id = root["device"];
        if(id != 1) {
            Serial.println("Unknown device id");
            return;
        }
        if(!root.containsKey("value")) {
            const char* name = root["name"];
            wappstobit_name(name);
            return;
        }

        int value_id = root["value"];
        if(value_id <= 0 || value_id > 20) {
            Serial.print("Invalid value_id: ");
            Serial.println(value_id);
            return;
        }

        if(root.containsKey("data")) {
            const char* data = root["data"];
            if (value_id <= 15) {
                wappstobit_data_number(value_id, atof(data));
            } else {
                wappstobit_data_string(value_id, data);
            }
        } else if(root.containsKey("name")) {
            const char* name = root["name"];
            const char* type = root["type"];
            if (value_id <= 15) {
                const char* unit = root["unit"];
                const char* min = root["min"];
                const char* max = root["max"];
                const char* step = root["step"];
                wappstobit_value_number_setup(value_id, name, type, unit, atof(min), atof(max), atof(step));
            } else {
                wappstobit_value_string_setup(value_id, name, type);
            }
        } else {
            Serial.println("Unhandled device json:");
            serializeJsonPretty(root, Serial);
            Serial.println("");
        }

    } else if(root.containsKey("command")) {
        const char* cmd = root["command"];

        if(strcmp(cmd, "clean") == 0) {
            wappstobit_clean();
        } else if(strcmp(cmd, "save") == 0) {
            wappstobit_save();
        } else if(strcmp(cmd, "info") == 0) {
            wappstobit_request_info();
        } else if(strcmp(cmd, "config_apn") == 0) {
            const char* apn = root["apn"];
            wappstobit_set_apn(apn);
        } else if(strcmp(cmd, "config_wifi") == 0) {
            const char* ssid = root["ssid"];
            const char* pass = root["pass"];
            wappstobit_set_wifi(ssid, pass);
        } else {
            Serial.println("Unknown command: ");
            serializeJsonPretty(root, Serial);
            Serial.println("");
        }
    } else {
        Serial.println("Json without valid tags");
    }
    memset(content_buffer, 0x00, 2000);
}

void send_number(uint8_t value_id, float data)
{
    char tmp[1000] = {0,};
    sprintf(tmp, "{\"device\":1,\"value\":\"%d\",\"data\":\"%f\"}", value_id, data);
    memset(i2c_write_buffer, 0x00, I2C_BUFFER_LENGTH);
    memcpy(i2c_write_buffer, tmp, strlen(tmp));
    i2c_slave_send_data();
}

void send_string(uint8_t value_id, const char* data)
{
    char tmp[1000] = {0,};
    sprintf(tmp, "{\"device\":1,\"value\":\"%d\",\"data\":\"%s\"}", value_id, data);
    memset(i2c_write_buffer, 0x00, I2C_BUFFER_LENGTH);
    memcpy(i2c_write_buffer, tmp, strlen(tmp));
    i2c_slave_send_data();
}

void send_info(float longitude, float latitude, uint8_t signal_strength, const char* status, const char* network, long utc_time, uint8_t ready, uint8_t queue_full)
{
    char tmp[1000] = {0,};
    sprintf(tmp, "{\"lon\":\"%f\",\"lat\":\"%f\",\"signal\":\"%d\",\"status\":\"%s\",\"network\":\"%s\",\"uptime\":\"%ld\",\"utc_time\":\"%ld\",\"ready\":\"%d\",\"queue_full\":\"%d\",\"version\":\"%s\"}",
            longitude, latitude, 0, status, network, millis(), utc_time, ready, queue_full, "0.0.1");
    memset(i2c_write_buffer, 0x00, I2C_BUFFER_LENGTH);
    memcpy(i2c_write_buffer, tmp, strlen(tmp));
    i2c_slave_send_data();
}
