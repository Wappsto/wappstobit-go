#include "wappstobit_go.h"
#include "i2c_communication.h"

void setup()
{
    Serial.begin(115200);

    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    digitalWrite(LED_RED, LED_OFF);
    digitalWrite(LED_GREEN, LED_ON);
    digitalWrite(LED_BLUE, LED_OFF);

    configure_i2c_slave();
}

void loop()
{
    if(i2c_slave_data_available()) {
        digitalWrite(LED_RED, LED_ON);
        parse_json();
        digitalWrite(LED_RED, LED_OFF);
    }
    delay(1);
}

void wappstobit_name(const char* name)
{
    Serial.print("Set name: ");
    Serial.println(name);
}

void wappstobit_request_info(void)
{
    send_info(0.0f, 0.0f, 10, "Status", "Network", 0, 0, 0);
}

void wappstobit_data_number(uint8_t value_id, float data)
{
    Serial.print("Data [");
    Serial.print(value_id);
    Serial.print("] Number: ");
    Serial.println(data);
}

void wappstobit_data_string(uint8_t value_id, const char* data)
{
    Serial.print("Data [");
    Serial.print(value_id);
    Serial.print("] String: ");
    Serial.println(data);
}

void wappstobit_value_number_setup(uint8_t value_id, const char* name, const char* type, const char* unit, float min, float max, float step)
{
    Serial.print("Setup [");
    Serial.print(value_id);
    Serial.print("] Number: ");
    Serial.print(name);
    Serial.print(" Type: ");
    Serial.print(type);
    Serial.print(" Min: ");
    Serial.print(min);
    Serial.print(" Max: ");
    Serial.print(max);
    Serial.print(" Step: ");
    Serial.println(step);
}

void wappstobit_value_string_setup(uint8_t value_id, const char* name, const char* type)
{
    Serial.print("Setup [");
    Serial.print(value_id);
    Serial.print("] String: ");
    Serial.print(name);
    Serial.print(" Type: ");
    Serial.println(type);
}

void wappstobit_set_wifi(const char* ssid, const char* password)
{
    Serial.print("Set SSID: ");
    Serial.print(ssid);
    Serial.print(" Password: ");
    Serial.println(password);
}

void wappstobit_clean(void)
{
    Serial.println("Perform clean");
}

void wappstobit_save(void)
{
    Serial.println("Perform save");
}

void wappstobit_set_apn(const char* apn)
{
    Serial.println("Apn not supported");
}
