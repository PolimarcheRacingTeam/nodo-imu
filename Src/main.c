
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f3xx_hal.h"
#include "can.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "imu.h"
#include <string.h> //per usare memcpy
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#define ACCEL_ID 0x25  //25 e 26 sono 2 numeri a caso
#define GYRO_ID 0x26

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
extern void initialise_monitor_handles(void);
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	initialise_monitor_handles();
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_CAN_Init();
  /* USER CODE BEGIN 2 */
	int err=-1;
	float ax,ay,az,gx,gy,gz;

	printf("START...\n\r");

	while (err!=0)   {
		err=imu_init();
		if (err==0)
			printf("Inizializzazione IMU ok...\n\r");
		else
			err = imu_init();
	}

	//IMU CALIBRATION
	imu_cal();

	uint32_t last=HAL_GetTick();
	uint32_t tick;
	int16_t data_acc[3];
	int16_t data_gyr[3];
	float accx_sum=0,accy_sum=0,accz_sum=0,pitch_sum=0,roll_sum=0,yaw_sum=0;

	//definisco lunghezza dei miei pacchetti
	int dlc = 6;

	// variabile per eventuali codici errore
	HAL_StatusTypeDef canret;

	// struct messaggio da trasmettere
	CanTxMsgTypeDef canmsg;

	//passiamo alla HAL un puntatore al nostro messaggio.
	hcan.pTxMsg = &canmsg;

	// impostiamo il messaggio
	// IDE definisce la lunghezza dell'identificatore (std-11bit vs ext-29bit)
	canmsg.IDE = CAN_ID_STD;
	canmsg.RTR = CAN_RTR_DATA;

	// DLC definisce la lunghezza del pacchetto in Byte: min=0, max=8
	canmsg.DLC = dlc;

	while(1)   {
		for(int i=0; i<=9; i++)   {
			do   {
						tick=HAL_GetTick();

					}
					while(tick-last<1);
					last=tick;

			err=-1;
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

		ax=accx_sum/10;
		ay=accy_sum/10;
		az=accz_sum/10;
		gx=pitch_sum/10;
		gy=roll_sum/10;
		gz=yaw_sum/10;

		data_acc[0]=ax;
		data_acc[1]=ay;
		data_acc[2]=az;

		data_gyr[0]=gx;
		data_gyr[1]=gy;
		data_gyr[2]=gz;


		/*printf("L'accelerazione lungo x e': %f\n\r",ax);
  		printf("L'accelerazione lungo y e': %f\n\r",ay);
  		printf("L'accelerazione lungo z e': %f\n\r",az);

  		printf("Il rollio     (roll)  e': %f \n\r",gx);
  		printf("Il beccheggio (pitch) e': %f \n\r",gy);
  		printf("L' imbardata  (yaw)   e': %f \n\r",gz);*/

		//TRASMISIONE CAN


		// scegliamo id per il pacchetto
		canmsg.StdId = ACCEL_ID;

		//metto i dati nel pacchetto
		memcpy(canmsg.Data, (void*)data_acc,dlc);

		// chiediamo la tramsissione del messaggio di cui abbiamo fornito il
		// puntatore in precendenza
		canret = HAL_CAN_Transmit_IT(&hcan);


		// scegliamo id per il pacchetto
		canmsg.StdId = GYRO_ID;

		//metto i dati nel pacchetto
		memcpy(canmsg.Data, (void*)data_gyr,dlc);

		// chiediamo la tramsissione del messaggio di cui abbiamo fornito il
		// puntatore in precendenza
		canret = HAL_CAN_Transmit_IT(&hcan);

		// se la funzione fallisce probabilmente abbiamo chiesto troppe
		// trasmissioni in troppo poco tempo, "cassetta delle lettere piena" o
		// "coda piena", dato che al massimo si possono accodare 3 messaggi.
		// non c'è stato tempo per sbrogliare tutte le richieste, forse il bus
		// è troppo impegnato con messaggi di priorità superiore (id inferiore)
		if (canret) printf("canret = %d\n",canret);



	}
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

	}
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while(1)
	{
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
