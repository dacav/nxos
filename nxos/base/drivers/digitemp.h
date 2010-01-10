/** @file digitemp.h
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

#ifndef __NXOS_DIGITEMP_H__
#define __NXOS_DIGITEMP_H__

#include "base/types.h"

/** @addtogroup driver */
/*@{*/

/** @defgroup digitemp Lego Education Digital Temperature Sensor driver
 *
 * The digitemp driver provides a simple API to use a digital temperature
 * sensor from Lego Education (9749).
 */
/*@{*/

/** I2C address.
 */
#define DIGITEMP_I2C_ADDRESS  0x4C

/** Internal memory addresses.
 *
 * This enum contains the sensor's internal memory addresses.
 * This sensor seems to be a TMP275 from TI.
 */
typedef enum {
  DIGITEMP_TEMPERATURE = 0, /* 2 Bytes */
  DIGITEMP_CONFIG = 0x01,
  DIGITEMP_TLOW = 0x02, /* 2 Bytes, not used */
  DIGITEMP_THIGH = 0x03 /* 2 Bytes, not used */
} digitemp_memory_slot;

/** The various modes this temperature sensor supports.
 *
 * Use 'or' to set them, e.g. to start one conversion in Shutdown Mode
 * with 12bit resolution use
 *   digitemp_set_mode(DIGITEMP_MODE_SHUTDOWN | DIGITEMP_MODE_ONESHOT
 *     | DIGITEMP_MODE_RES12BITS);
 * After this the sensor will do conversion and goes to sleep (but still
 * offers the result of the conversion). The times need are shown below.
 * E.g. with the 12 bits resolution used above you have to wait 220ms before
 * the result is available for read.
 * The default at power on is 0 -> continues conversion with 9 bits resolution.
 *
 */
typedef enum {
  DIGITEMP_MODE_SHUTDOWN = 0x01, /* Go into the Shutdown Mode */
  DIGITEMP_MODE_ONESHOT = 0x80, /* Do one conversion if in Shutdown Mode */
  DIGITEMP_MODE_RES9BITS = 0x00, /* 9 bits resolution (default) -> 0.5 C (27.5ms)*/
  DIGITEMP_MODE_RES10BITS = 0x20, /* 10 bits resolution -> 0.25 C (55ms)*/
  DIGITEMP_MODE_RES11BITS = 0x40, /* 11 bits resolution -> 0.125 C (110ms)*/
  DIGITEMP_MODE_RES12BITS = 0x60 /* 12 bits resolution -> 0.0625 C (220ms)*/
} digitemp_mode;

/** Initialize the digital temperature sensor on port @a sensor.
 *
 * @param sensor The sensor port number.
 */
void digitemp_init(U32 sensor);

/** Close the link with the digital temperature sensor and disable him.
 *
 * @param sensor The sensor port number.
 */
void digitemp_close(U32 sensor);

/** Check the presence of an digital temperature sensor on port @a sensor.
 *
 * @param sensor The sensor port number.
 *
 * @note The device on port @a sensor first needs to be initialized with
 * digitemp_init().
 *
 * @return True if an digital temperature sensor was found, false otherwise.
 */
bool digitemp_detect(U32 sensor);

/** Set the mode of the digital temperature sensor on port @a sensor.
 *
 * @param sensor The sensor port number.
 * @param mode The mode of the sensor.
 *
 * @return True if the mode was set, false otherwise.
 */
bool digitemp_set_mode(U32 sensor, digitemp_mode);

/** Read the temperature raw.
 *
 * @param sensor The sensor port number.
 *
 * @return The temperature in raw format. You have to shift the
 * returned value according to the mode. E.g. for 12bits resolution
 * you have to do
 *   result >>= (16-12);
 */
S16 digitemp_get_temperature_raw(U32 sensor);

/** Read the temperature*2.
 *
 * @param sensor The sensor port number.
 *
 * @return The temperature*2.
 *
 * @note Use digitemp_get_temperature_raw for higher resolutions because
 * all the lower bits (the better resolution) is just shifted out.
 */
S16 digitemp_get_temperature(U32 sensor);

/** Display the digital temperature sensor's information.
 *
 * Displays on the NXT screen the digital temperature sensor's information
 * (mode and temperature).
 *
 * @param sensor The sensor port number.
 */
void digitemp_info(U32 sensor);

/*@}*/
/*@}*/

#endif /* __NXOS_DIGITEMP_H__ */
