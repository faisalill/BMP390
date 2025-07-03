#include "BMP390.h"

BMP390::BMP390(int sda_pin, int scl_pin) 
    : _initialized(false), _sda_pin(sda_pin), _scl_pin(scl_pin) {
}

void BMP390::writeReg(uint8_t reg_addr, uint8_t* reg_data, uint32_t len) {
    Wire.beginTransmission(BMP390_I2C_ADDR_SEC);
    Wire.write(reg_addr);
    for (uint32_t i = 0; i < len; i++) {
        Wire.write(reg_data[i]);
    }
    uint8_t status = Wire.endTransmission();
    if (status != 0) {
        Serial.print("I2C Write Error: ");
        Serial.println(status);
    }
}

void BMP390::readReg(uint8_t reg_addr, uint8_t* reg_data, uint32_t len) {
    Wire.beginTransmission(BMP390_I2C_ADDR_SEC);
    Wire.write(reg_addr);
    Wire.endTransmission(false);
    
    uint8_t bytesReceived = Wire.requestFrom(BMP390_I2C_ADDR_SEC, (int)len);
    
    if (bytesReceived == len) {
        for (uint32_t i = 0; i < len; i++) {
            reg_data[i] = Wire.read();
        }
    } else {
        Serial.print("I2C Read Error: ");
        Serial.println(bytesReceived);
        for (uint32_t i = 0; i < len; i++) {
            reg_data[i] = 0;
        }
    }
}

void BMP390::bmp3_soft_reset() {
    uint8_t cmd = BMP3_SOFT_RESET_CMD;
    writeReg(BMP3_REG_CMD, &cmd, 1);
    delay(20);
}

void BMP390::parse_calib_data(const uint8_t* reg_data) {
    auto u16 = [](uint8_t msb, uint8_t lsb) { return (uint16_t)(((uint16_t)msb << 8) | lsb); };
    
    uint16_t par_t1_u = u16(reg_data[1], reg_data[0]);
    uint16_t par_t2_u = u16(reg_data[3], reg_data[2]);
    int8_t   par_t3_s = (int8_t)reg_data[4];

    int16_t  par_p1_s = (int16_t)u16(reg_data[6], reg_data[5]);
    int16_t  par_p2_s = (int16_t)u16(reg_data[8], reg_data[7]);
    int8_t   par_p3_s = (int8_t)reg_data[9];
    int8_t   par_p4_s = (int8_t)reg_data[10];
    uint16_t par_p5_u = u16(reg_data[12], reg_data[11]);
    uint16_t par_p6_u = u16(reg_data[14], reg_data[13]);
    int8_t   par_p7_s = (int8_t)reg_data[15];
    int8_t   par_p8_s = (int8_t)reg_data[16];
    int16_t  par_p9_s = (int16_t)u16(reg_data[18], reg_data[17]);
    int8_t   par_p10_s = (int8_t)reg_data[19];
    int8_t   par_p11_s = (int8_t)reg_data[20];

    _calib_data.par_t1 = (double)par_t1_u / 0.00390625f;
    _calib_data.par_t2 = (double)par_t2_u / 1073741824.0f;
    _calib_data.par_t3 = (double)par_t3_s / 281474976710656.0f;

    _calib_data.par_p1 = ((double)par_p1_s - 16384.0) / 1048576.0f;
    _calib_data.par_p2 = ((double)par_p2_s - 16384.0) / 536870912.0f;
    _calib_data.par_p3 = (double)par_p3_s / 4294967296.0f;
    _calib_data.par_p4 = (double)par_p4_s / 137438953472.0f;
    _calib_data.par_p5 = (double)par_p5_u / 0.125f;
    _calib_data.par_p6 = (double)par_p6_u / 64.0f;
    _calib_data.par_p7 = (double)par_p7_s / 256.0f;
    _calib_data.par_p8 = (double)par_p8_s / 32768.0f;
    _calib_data.par_p9 = (double)par_p9_s / 281474976710656.0f;
    _calib_data.par_p10 = (double)par_p10_s / 281474976710656.0f;
    _calib_data.par_p11 = (double)par_p11_s / 36893488147419103232.0f;
}

bool BMP390::get_calib_data() {
    uint8_t calib_buffer[21];
    readReg(BMP3_REG_CALIB_DATA, calib_buffer, 21);
    
    bool all_zeros = true;
    for (int i = 0; i < 21; ++i) {
        if (calib_buffer[i] != 0) {
            all_zeros = false;
            break;
        }
    }
    if (all_zeros) {
        Serial.println("Calib data zero.");
        return false;
    }
    
    parse_calib_data(calib_buffer);
    return true;
}

double BMP390::compensate_temperature(uint32_t uncomp_temp) {
    double partial_data1 = (double)(uncomp_temp - _calib_data.par_t1);
    double partial_data2 = (double)(partial_data1 * _calib_data.par_t2);
    _calib_data.t_lin = partial_data2 + (partial_data1 * partial_data1) * _calib_data.par_t3;
    return _calib_data.t_lin;
}

double BMP390::compensate_pressure(uint32_t uncomp_press) {
    double partial_data1 = _calib_data.par_p6 * _calib_data.t_lin;
    double partial_data2 = _calib_data.par_p7 * (_calib_data.t_lin * _calib_data.t_lin);
    double partial_data3 = _calib_data.par_p8 * (_calib_data.t_lin * _calib_data.t_lin * _calib_data.t_lin);
    double partial_out1 = _calib_data.par_p5 + partial_data1 + partial_data2 + partial_data3;

    partial_data1 = _calib_data.par_p2 * _calib_data.t_lin;
    partial_data2 = _calib_data.par_p3 * (_calib_data.t_lin * _calib_data.t_lin);
    partial_data3 = _calib_data.par_p4 * (_calib_data.t_lin * _calib_data.t_lin * _calib_data.t_lin);
    double partial_out2 = (double)uncomp_press * (_calib_data.par_p1 + partial_data1 + partial_data2 + partial_data3);
    
    partial_data1 = (double)uncomp_press * (double)uncomp_press;
    double partial_data_p9_p10 = _calib_data.par_p9 + _calib_data.par_p10 * _calib_data.t_lin;
    double partial_data3_calc = partial_data1 * partial_data_p9_p10;
    double partial_data4 = partial_data3_calc + ((double)uncomp_press * (double)uncomp_press * (double)uncomp_press) * _calib_data.par_p11;

    return partial_out1 + partial_out2 + partial_data4;
}

bool BMP390::init_bmp390() {
    if (!Serial) {
        Serial.begin(115200); 
        while (!Serial);
    }

    Wire.begin(_sda_pin, _scl_pin);
    Wire.setClock(100000);

    delay(250);
    
    Serial.println("BMP390 Init...");

    uint8_t chip_id = 0;
    readReg(BMP3_REG_CHIP_ID, &chip_id, 1);
    
    if (chip_id == BMP390_CHIP_ID) {
        Serial.println("BMP390 found.");
    } else {
        Serial.print("BMP390 not found: 0x");
        Serial.println(chip_id, HEX);
        return false;
    }
    
    bmp3_soft_reset();

    if (!get_calib_data()) {
        Serial.println("Calib data read failed.");
        return false;
    }

    uint8_t pwr_ctrl = (BMP3_MODE_NORMAL << 4) | (1 << 1) | 1;
    writeReg(BMP3_REG_PWR_CTRL, &pwr_ctrl, 1);
    
    uint8_t osr = (BMP3_OVERSAMPLING_4X << 3) | BMP3_OVERSAMPLING_4X;
    writeReg(BMP3_REG_OSR, &osr, 1);
    
    uint8_t odr = BMP3_ODR_25_HZ;
    writeReg(BMP3_REG_ODR, &odr, 1);

    uint8_t config = BMP3_IIR_FILTER_DISABLE << 1;
    writeReg(BMP3_REG_CONFIG, &config, 1);

    Serial.println("BMP390 ready.");
    delay(100);
    
    return true;
}

bmp3_data BMP390::get_bmp_values() {
    bmp3_data sensor_data;
    sensor_data.success = false;

    if (!_initialized) {
        if (!init_bmp390()) {
            Serial.println("BMP390 init failed.");
            return sensor_data;
        }
        _initialized = true;
    }

    uint8_t reg_data[6];
    readReg(BMP3_REG_DATA, reg_data, 6);

    uint32_t uncomp_press = (uint32_t)((reg_data[2] << 16) | (reg_data[1] << 8) | reg_data[0]);
    uint32_t uncomp_temp = (uint32_t)((reg_data[5] << 16) | (reg_data[4] << 8) | reg_data[3]);
    
    if (uncomp_press == 0 && uncomp_temp == 0) {
        Serial.println("Raw data zero.");
        return sensor_data;
    }

    sensor_data.temperature = compensate_temperature(uncomp_temp);
    sensor_data.pressure = compensate_pressure(uncomp_press);
    
    sensor_data.success = true;
    return sensor_data;
}
