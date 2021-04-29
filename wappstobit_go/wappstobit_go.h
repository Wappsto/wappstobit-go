#define LED_RED 21
#define LED_GREEN 22
#define LED_BLUE 23

#define LED_ON LOW
#define LED_OFF HIGH

void wappstobit_name(const char* name);
void wappstobit_request_info(void);
void wappstobit_data_number(uint8_t value_id, float data);
void wappstobit_data_string(uint8_t value_id, const char* data);
void wappstobit_value_number_setup(uint8_t value_id, const char* name, const char* type, const char* unit, float min, float max, float step);
void wappstobit_value_string_setup(uint8_t value_id, const char* name, const char* type);
void wappstobit_set_wifi(const char* ssid, const char* password);
void wappstobit_clean(void);
void wappstobit_save(void);
void wappstobit_set_apn(const char* apn);
