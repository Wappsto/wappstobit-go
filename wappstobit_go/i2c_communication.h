void parse_json(void);
bool i2c_slave_data_available(void);
void send_info(float longitude, float latitude, uint8_t signal_strength, const char* status, const char* network, long utc_time, uint8_t ready, uint8_t queue_full);
void send_number(uint8_t value_id, float data);
void send_string(uint8_t value_id, const char* data);
