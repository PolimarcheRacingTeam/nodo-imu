/*
 * imu.h
 *
 *  Created on: 26 Marzo 2018
 *      Author: Francesco Conforte
 */

#ifndef IMU_H_
#define IMU_H_


#include "inv_mpu.h" //libreria Invensense adattata per renesas rx
#include "i2c.h"

typedef struct {
	short rawacc[3];
	short rawgyr[3];
	struct{
		float x,y,z;
	} acc;
	struct{
		float x,y,z;
	} gyr;

	uint16_t acc_sens;
	float gyr_sens;

} imudata;

extern imudata imu;

int imu_init();

//data must be of type float[6]
int imu_read();

//data must be of type int16_t[6]
int imu_read_raw();

int imu_cal(); //imu calibration

#endif /* IMU_H_ */
