/* Copyright (C) 2007 the NxOS developers
 *
 * See AUTHORS for a full list of the developers.
 *
 * Redistribution of this file is permitted under
 * the terms of the GNU Public License (GPL) version 2.
 */

#include "base/at91sam7s256.h"
#include "base/drivers/ht_gyro.h"

#include "base/drivers/sensors.h"
#include "base/display.h"
#include "base/drivers/systick.h"

void ht_gyro_init(U32 sensor) {
  nx_sensors_analog_enable(sensor);
}

void ht_gyro_close(U32 sensor) {
  nx_sensors_analog_disable(sensor);
}

U32 ht_gyro_get_value(U32 sensor) {
  return nx_sensors_analog_get(sensor);
}

U32 ht_gyro_calculate_average_zero(U32 sensor) {
  U32 zero = 0;
  for(U32 i = 0; i < 8; ++i) {
    zero += ht_gyro_get_value(sensor);
    nx_systick_wait_ms(4);
  }
  return zero>>3; /* = /8 */
}

void ht_gyro_info(U32 sensor) {
  nx_display_uint(ht_gyro_get_value(sensor));
  nx_display_end_line();
}
