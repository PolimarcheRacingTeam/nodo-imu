/*
 * imu.c
 *
 *  Created on: 26 mar 2018
 *      Author: Francesco Conforte
 */



#include "i2c.h"
#include "inv_mpu.h"
#include "imu.h"


imudata imu;

int imu_read(){
	imu_read_raw();

	imu.acc.x=(float)imu.rawacc[0]/imu.acc_sens;
	imu.gyr.x=(float)imu.rawgyr[0]/imu.gyr_sens*0.01745329252; //rad
	imu.acc.y=(float)imu.rawacc[1]/imu.acc_sens;
	imu.gyr.y=(float)imu.rawgyr[1]/imu.gyr_sens*0.01745329252; //rad
	imu.acc.z=(float)imu.rawacc[2]/imu.acc_sens;
	imu.gyr.z=(float)imu.rawgyr[2]/imu.gyr_sens*0.01745329252; //rad

	return 0;
}

int imu_read_raw(){

	mpu_get_accel_reg(imu.rawacc, NULL);
	mpu_get_gyro_reg(imu.rawgyr, NULL);

	return 0;
}

/**
 *  @brief      Initialize imu.
 *  Initial configuration:\n
 *  Gyro FSR: +/- 2000DPS\n
 *  Accel FSR +/- 2G\n
 *  FIFO rate: 1000Hz\n
 *  FIFO: Disabled.\n
 *  Turn specific sensors on/off: only accel and gyro are turn on\n
 *  @return     0 if successful.
 */
int imu_init(){
	if (mpu_init()) //se mpu_init() è vero, cioè è 1 allora ritorna 2, altrimenti va avanti
		return 0x2;
	if (mpu_set_sensors(INV_XYZ_ACCEL | INV_XYZ_GYRO)) //only gyro and acc are turn on
		return 0x3;
	if (mpu_set_sample_rate(1000)) //campiona i dati a 1000Hz, cioè ogni millisecondo
		return 0x4;
	mpu_get_accel_sens(&imu.acc_sens);
	mpu_get_gyro_sens(&imu.gyr_sens);
	return 0x0;
}

/*Livello l'IMU. Poichè alla prima accensione dell'MPU9250 vengono rilevati dei valori del giroscopio e dell'accelerometro
	con dei picchi molto elevati, e dal datasheet si legge che lo Sturtup Time per l'accelerometro e il
	giroscopio varia tra i 20ms e i 30ms, e considerando che i dati vengono campionati ogni
	ms, allora tralasciamo i primi 60 dati (esagerando un po' per essere più sicuri).*/

int imu_cal(void) {
	/*In questo primo step vengono tralasciati i primi 60 valori, in modo da prendere dei valori
	più veritieri*/

	//CALIBRATION: STEP 1
	printf("MAKE SURE THE IMU IS LEVEL. CALIBRATION STEP 1");
	printf("\n\n");

	for (int i=0; i<=60; i++)   {
		int ret=imu_read();
	}

	/*Nel secondo step campiono 50 valori, li sommo e ne faccio la media per portare i valori campionati vicino allo zero.*/
	//CALIBRATION: STEP 2

	printf("CALIBRATION STEP 2");
	printf("\n\n");
	float accx_sum=0,accy_sum=0,accz_sum=0,pitch_sum=0,roll_sum=0,yaw_sum=0;
	float ax,ay,az,gx,gy,gz;
	int err=-1;
	for(int i=0;i<=50;i++)   {
		while (err!=0)   {
			err=imu_read();
		    if (err==0) continue;
		    else   {
		    	printf("Dati non letti...\n\r");
		  		err = imu_read();
		    }
		}
		accx_sum+=imu.acc.x*1000;
		accy_sum+=imu.acc.y*1000;
		accz_sum+=imu.acc.z*1000;

		pitch_sum+=imu.gyr.x*1000;
		roll_sum+=imu.gyr.y*1000;
		yaw_sum+=imu.gyr.z*1000;
    }

	ax=accx_sum/50;
	ay=accy_sum/50;
	az=accz_sum/50;
	gx=pitch_sum/50;
	gy=roll_sum/50;
	gz=yaw_sum/50;

	printf("L'accelerazione lungo x e': %f\n\r",ax);
	printf("L'accelerazione lungo y e': %f\n\r",ay);
	printf("L'accelerazione lungo z e': %f\n\r",az);

	printf("Il rollio     (roll)  e': %f \n\r",gx);
	printf("Il beccheggio (pitch) e': %f \n\r",gy);
	printf("L' imbardata  (yaw)   e': %f \n\r",gz);

	return 0;
}
