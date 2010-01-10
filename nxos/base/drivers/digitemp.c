/** @file digitemp.c
 *  @brief Digitial Temperature Sensor driver.
 *
 * Driver for Lego Education's Digital Temperature Sensor (9749).
 */

/* Copyright (C) 2009 the NxOS developers
 *
 * See AUTHORS for a full list of the developers.
 *
 * Redistribution of this file is permitted under
 * the terms of the GNU Public License (GPL) version 2.
 */

#include "base/display.h"
#include "base/util.h"
#include "base/drivers/i2c.h"
#include "base/drivers/i2c_memory.h"
#include "base/drivers/digitemp.h"

/** Initializes the digital temperatur sensor.
 *
 * @param sensor The sensor port number.
 *
 */
void digitemp_init(U32 sensor) {
  nx_i2c_memory_init(sensor, DIGITEMP_I2C_ADDRESS, FALSE);
}

/** Close the link to the temperature sensor and disable the sensor port.
 *
 * @param sensor The sensor port number.
 *
 */
void digitemp_close(U32 sensor) {
  nx_i2c_memory_close(sensor);
}

/** Set the mode of the digital temperature sensor on port @a sensor.
 *
 * @param sensor The sensor port number.
 * @param mode The mode of the sensor.
 *
 * @return True if the mode was set, false otherwise.
 */
bool digitemp_set_mode(U32 sensor, digitemp_mode m) {
  U8 mode = m;
  return nx_i2c_memory_write(sensor, DIGITEMP_CONFIG, &mode, 1) == I2C_ERR_OK;
}

/** Digital Temperature Sensor detection.
 *
 * Tries to detect the presence of an digital temperature sensor on the given port by
 * reading the temperature and mode.
 *
 * @param sensor The sensor port number.
 *
 * @return True if an digital temperature sensor was found.
 */
bool digitemp_detect(U32 sensor) {
  U8 temp[2];
  if ( nx_i2c_memory_read(sensor, DIGITEMP_CONFIG, temp, 1) != I2C_ERR_OK)
      return FALSE;
  if ( nx_i2c_memory_read(sensor, DIGITEMP_TLOW, temp, 2) != I2C_ERR_OK)
      return FALSE;
  if ( nx_i2c_memory_read(sensor, DIGITEMP_THIGH, temp, 2) != I2C_ERR_OK)
      return FALSE;
  return nx_i2c_memory_read(sensor, DIGITEMP_TEMPERATURE, temp, 2) == I2C_ERR_OK;
}

/** Read the temperature raw.
 *
 * @param sensor The sensor port number.
 *
 * @return The temperature in raw format. You have to shift the
 * returned value according to the mode. E.g. for 12bits resolution
 * you have to do
 *   result >>= (16-12);
 */
S16 digitemp_get_temperature_raw(U32 sensor)
{
  S16 temperature;
  U8 buf[2];
  if( nx_i2c_memory_read(sensor, DIGITEMP_TEMPERATURE, buf, 2) != I2C_ERR_OK )
    return 0x6666;
 ((U8*)&temperature)[0]=buf[1];
 ((U8*)&temperature)[1]=buf[0];
  return temperature;
}

/** Display connected digital temperature sensor's information.
 *
 * @param sensor The sensor port number.
 */
void digitemp_info(U32 sensor) {
  U8 mode = 0;
  S16 temperature = 0;

  nx_i2c_memory_read(sensor, DIGITEMP_CONFIG, &mode, 1);
  nx_display_string("Mode: 0x");
  nx_display_hex(mode);
  nx_display_end_line();
  temperature = digitemp_get_temperature(sensor);
  nx_display_string("Temp: ");
  if(temperature < 0 ) {
    nx_display_string("-");
    temperature = -temperature;
  }
  nx_display_uint(temperature/2);
  if( temperature & 1 )
    nx_display_string(".5");
  nx_display_string(" C");
  nx_display_end_line();
}

S16 digitemp_get_temperature(U32 sensor) {
  return digitemp_get_temperature_raw(sensor) >> (16-9);
}

