#ifndef BMP390_H
#define BMP390_H

#include <Wire.h>

#define BMP390_I2C_ADDR_SEC 0x77
#define BMP3_REG_CHIP_ID 0x00
#define BMP3_REG_PWR_CTRL 0x1B
#define BMP3_REG_OSR 0X1C
#define BMP3_REG_ODR 0x1D
#define BMP3_REG_CONFIG 0x1F
#define BMP3_REG_DATA 0x04
#define BMP3_REG_CALIB_DATA 0x31
#define BMP3_REG_CMD 0x7E

#define BMP390_CHIP_ID 0x60

#define BMP3_MODE_SLEEP 0x00
#define BMP3_MODE_FORCED 0x01
#define BMP3_MODE_NORMAL 0x03

#define BMP3_NO_OVERSAMPLING 0x00
#define BMP3_OVERSAMPLING_2X 0x01
#define BMP3_OVERSAMPLING_4X 0x02
#define BMP3_OVERSAMPLING_8X 0x03
#define BMP3_OVERSAMPLING_16X 0x04
#define BMP3_OVERSAMPLING_32X 0x05

#define BMP3_IIR_FILTER_DISABLE 0x00

#define BMP3_ODR_25_HZ 0x03

#define BMP3_SOFT_RESET_CMD 0xB6

struct bmp3_data {
  double temperature;
  double pressure;
  bool success;
};

class BMP390 {
public:
  BMP390(int sda_pin, int scl_pin);
  bmp3_data get_bmp_values();

private:
  struct bmp3_calib_data {
    double par_t1;
    double par_t2;
    double par_t3;
    double par_p1;
    double par_p2;
    double par_p3;
    double par_p4;
    double par_p5;
    double par_p6;
    double par_p7;
    double par_p8;
    double par_p9;
    double par_p10;
    double par_p11;
    double t_lin;
  };

  bmp3_calib_data _calib_data;
  bool _initialized;
  int _sda_pin;
  int _scl_pin;

  void writeReg(uint8_t reg_addr, uint8_t *reg_data, uint32_t len);
  void readReg(uint8_t reg_addr, uint8_t *reg_data, uint32_t len);
  void bmp3_soft_reset();
  void parse_calib_data(const uint8_t *reg_data);
  bool get_calib_data();
  double compensate_temperature(uint32_t uncomp_temp);
  double compensate_pressure(uint32_t uncomp_press);
  bool init_bmp390();
};

#endif // BMP390_H
