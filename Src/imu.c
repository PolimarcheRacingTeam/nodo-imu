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
float ax_cal=0,ay_cal=0,az_cal=0,gx_cal=0,gy_cal=0,gz_cal=0;



int imu_read(){
	imu_read_raw();

	imu.acc.x=((float)imu.rawacc[0]/imu.acc_sens)-ax_cal;
	imu.gyr.x=((float)imu.rawgyr[0]/imu.gyr_sens*0.01745329252)-gx_cal; //rad
	imu.acc.y=((float)imu.rawacc[1]/imu.acc_sens)-ay_cal;
	imu.gyr.y=((float)imu.rawgyr[1]/imu.gyr_sens*0.01745329252)-gy_cal; //rad
	imu.acc.z=((float)imu.rawacc[2]/imu.acc_sens)-az_cal;
	imu.gyr.z=((float)imu.rawgyr[2]/imu.gyr_sens*0.01745329252)-gz_cal; //rad

	return 0;
}

//funzione richiamata nella calibrazione che mi genera i valori da sottrarre (off-set)
int imu_read_nc(){
	imu_read_raw();

	imu.acc.x=((float)imu.rawacc[0]/imu.acc_sens);
	imu.gyr.x=((float)imu.rawgyr[0]/imu.gyr_sens*0.01745329252); //rad
	imu.acc.y=((float)imu.rawacc[1]/imu.acc_sens);
	imu.gyr.y=((float)imu.rawgyr[1]/imu.gyr_sens*0.01745329252); //rad
	imu.acc.z=((float)imu.rawacc[2]/imu.acc_sens);
	imu.gyr.z=((float)imu.rawgyr[2]/imu.gyr_sens*0.01745329252); //rad

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
	if (mpu_init()) //se mpu_init() � vero, cio� � 1 allora ritorna 2, altrimenti va avanti
		return 0x2;
	if (mpu_set_sensors(INV_XYZ_ACCEL | INV_XYZ_GYRO)) //only gyro and acc are turn on
		return 0x3;
	if (mpu_set_sample_rate(1000)) //campiona i dati a 1000Hz, cio� ogni millisecondo
		return 0x4;
	mpu_get_accel_sens(&imu.acc_sens);
	mpu_get_gyro_sens(&imu.gyr_sens);
	return 0x0;
}

/*Livello l'IMU. Poich� alla prima accensione dell'MPU9250 vengono rilevati dei valori del giroscopio e dell'accelerometro
	con dei picchi molto elevati, e dal datasheet si legge che lo Sturtup Time per l'accelerometro e il
	giroscopio varia tra i 20ms e i 30ms, e considerando che i dati vengono campionati ogni
	ms, allora tralasciamo i primi 60 dati (esagerando un po' per essere pi� sicuri).*/

int imu_cal(void) {
	/*In questo primo step vengono tralasciati i primi 60 valori, in modo da prendere dei valori
	pi� veritieri*/

	//CALIBRATION: STEP 1
	printf("MAKE SURE THE IMU IS LEVEL. CALIBRATION STEP 1");
	printf("\n\n");

	for (int i=0; i<60; i++)   {
		imu_read_nc();
	}

	/*Nel secondo step campiono 50 valori, li sommo e ne faccio la media per portare i valori campionati vicino allo zero.*/
	//CALIBRATION: STEP 2

	printf("CALIBRATION STEP 2");
	printf("\n\n");
	float accx_sum=0,accy_sum=0,accz_sum=0,pitch_sum=0,roll_sum=0,yaw_sum=0;
	int err=-1;
	for(int i=0;i<50;i++)   {
		while (err!=0)   {
			err=imu_read_nc();
			if (err==0) continue;
			else   {
				printf("Dati non letti...\n\r");
				err = imu_read_nc();
			}
		}
		accx_sum+=imu.acc.x;
		accy_sum+=imu.acc.y;
		accz_sum+=imu.acc.z;

		pitch_sum+=imu.gyr.x;
		roll_sum+=imu.gyr.y;
		yaw_sum+=imu.gyr.z;
	}

	ax_cal=accx_sum/50;
	ay_cal=accy_sum/50;
	az_cal=accz_sum/50;

	gx_cal=pitch_sum/50;
	gy_cal=roll_sum/50;
	gz_cal=yaw_sum/50;


	/*printf("L'accelerazione lungo x e': %f\n\r",ax_cal);
	printf("L'accelerazione lungo y e': %f\n\r",ay_cal);
	printf("L'accelerazione lungo z e': %f\n\r",az_cal);

	printf("Il rollio     (roll)  e': %f \n\r",gx_cal);
	printf("Il beccheggio (pitch) e': %f \n\r",gy_cal);
	printf("L' imbardata  (yaw)   e': %f \n\r",gz_cal);*/

	return 0;
}
