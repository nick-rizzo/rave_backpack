/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "led_patterns.h"
#include "led_driver.h"
#include "ssd1306.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEBOUNCE_DELAY 50
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi2_tx;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
typedef enum {
	LED_OFF=0, PATTERN=1, COLOR=3, SPEED=5, BRIGHTNESS=7
}ctrl_state;

typedef enum {
	NONE=0, UP=1, DN=2, LEFT=3, RIGHT=4
}dir_ctrl;

ctrl_state cur_mode_state = LED_OFF;
ctrl_state new_mode_state = LED_OFF;

pattern_state cur_col_state = IDLE;
pattern_state new_col_state = IDLE;

char * up_msg = "up pressed\r\n";
char * dn_msg = "dn pressed\r\n";
char * left_msg = "left pressed\r\n";
char * right_msg = "right pressed\r\n";

volatile dir_ctrl cur_dir = NONE;
volatile bool led_enable = 1;

uint32_t cur_color = 0x00FF00U; // init to red
uint8_t cur_brightness = 100;
uint8_t cur_speed = 50;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_Delay(uint32_t Delay)
{
  uint32_t tickstart = HAL_GetTick();
  uint32_t wait = Delay;

  /* Add a freq to guarantee minimum wait */
  if (wait < HAL_MAX_DELAY)
  {
    wait += (uint32_t)(uwTickFreq);
  }

  while((HAL_GetTick() - tickstart) < wait)
  {
	  if (!led_enable){
		  break;
	  }
  }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  dma_buffer_init();
  ssd1306_init();
  HAL_Delay(50);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  switch (cur_mode_state){
	  	  case LED_OFF:{
	  		  if (1==1){
	  			  display_init();
	  			  insert_selector(PATTERN);
	  			  ssd1306_update_display();
	  			  new_mode_state = PATTERN;
	  		  }
	  		  else{
	  			  new_mode_state = LED_OFF;
	  		  }
	  		  break;
	  	  }
	  	  case PATTERN:{

	  			if (cur_dir == DN){
	  				new_mode_state = COLOR;
	  				move_selector(PATTERN, COLOR);
	  				cur_dir = NONE;
	  			} else if (cur_dir == UP){
	  			  	new_mode_state = BRIGHTNESS;
	  			  	move_selector(PATTERN, BRIGHTNESS);
	  			  	cur_dir = NONE;
	  			} else {
	  			  	new_mode_state = PATTERN;
	  			}
	  		  switch(cur_col_state){
	  		  	  case IDLE:{
	  		  		  new_col_state = STATIC;
	  		  		  break;
	  		  	  }
	  		  	  case STATIC:{

	  		  	  	  if (cur_dir == RIGHT){
	  		  	  		  new_col_state = BREATHE;
	  		  	  		  clear_page(1);
	  		  	  	  	  write_page(1, "  Breathe   ");
	  		  	  	  	  insert_selector(PATTERN);
	  		  	  	  	  ssd1306_update_display();
	  		  	  	  	  cur_dir = NONE;
	  		  	  	  } else if (cur_dir == LEFT){
	  		  	  		  new_col_state = METEOR;
	  		  	  		  clear_page(1);
	  		  	  	  	  write_page(1, "   Meteor   ");
	  		  	  	  	  insert_selector(PATTERN);
	  		  	  	  	  ssd1306_update_display();
	  		  	  	  	  cur_dir = NONE;
	  		  	  	  } else {
	  		  	  		  new_col_state = STATIC;
	  		  	  	  }
	  		  		  break;
	  		  	  }
	  		  	  case (BREATHE):{

	  				  if (cur_dir == RIGHT){
	  		  	  	  	  new_col_state = RAINBOW;
	  		  	  	  	  clear_page(1);
	  		  	  	  	  write_page(1, "  Rainbow   ");
	  		  	  	  	  insert_selector(PATTERN);
	  		  	  	  	  ssd1306_update_display();
	  		  	  	  	  cur_dir = NONE;
	  		  	  	  } else if (cur_dir == LEFT){
	  		  	  	  	  new_col_state = STATIC;
	  		  	  	  	  clear_page(1);
	  		  	  	  	  write_page(1, "   Static   ");
	  		  	  	  	  insert_selector(PATTERN);
	  		  	  	  	  ssd1306_update_display();
	  		  	  	  	  cur_dir = NONE;
	  		  	  	  } else {
	  		  	  	  	  new_col_state = BREATHE;
	  		  	  	  }
	  			 	 break;
	  		  	  }
	  		  	  case (RAINBOW):{

	  		  	  	  if (cur_dir == RIGHT){
	  		  	  	  	  new_col_state = METEOR;
	  		  	  	  	  clear_page(1);
	  		  	  	  	  write_page(1, "   Meteor   ");
	  		  	  	  	  insert_selector(PATTERN);
	  		  	  	  	  ssd1306_update_display();
	  		  	  	  	  cur_dir = NONE;
	  		  	  	  } else if (cur_dir == LEFT){
	  		  	  		  new_col_state = BREATHE;
	  		  	  		  clear_page(1);
	  		  	  		  write_page(1, "  Breathe   ");
	  		  	  		  insert_selector(PATTERN);
	  		  	  		  ssd1306_update_display();
	  		  	  		  cur_dir = NONE;
	  		  	  	  } else {
	  		  	  	  	  new_col_state = RAINBOW;
	  		  	  	  }
	  		  		  break;
	  		  	  }
	  		  	  case (METEOR):{

	  		  	  	  if (cur_dir == RIGHT){
	  		  	  	  	  new_col_state = STATIC;
	  		  	  	  	  clear_page(1);
	  		  	  	  	  write_page(1, "   Static   ");
	  		  	  	  	  insert_selector(PATTERN);
	  		  	  	  	  ssd1306_update_display();
	  		  	  	  	  cur_dir = NONE;
	  		  	  	  } else if (cur_dir == LEFT){
	  		  	  	  	  new_col_state = RAINBOW;
	  		  	  	  	  clear_page(1);
	  		  	  	  	  write_page(1, "  Rainbow   ");
	  		  	  	  	  insert_selector(PATTERN);
	  		  	  	  	  ssd1306_update_display();
	  		  	  	  	  cur_dir = NONE;
	  		  	  	  } else {
	  		  	  	  	  new_col_state = METEOR;
	  		  	  	  }
	  		  		  break;
	  		  	  }
	  	  	  }// end switch

	  		  cur_col_state = new_col_state;
	  	  	  break;
	  	  }
	  	  case COLOR:{
	  		  if (cur_dir == DN){
		  		new_mode_state = SPEED;
		  		move_selector(COLOR, SPEED);
		  		cur_dir = NONE;
	  		  }
	  		  else if (cur_dir == UP){
		  		new_mode_state = PATTERN;
		  		move_selector(COLOR, PATTERN);
		  		cur_dir = NONE;
	  		  } else {
		  		  	new_mode_state = COLOR;
	  		  }

	  		  if (cur_dir == RIGHT){
	  			  if (col_arr_idx == 8/*(sizeof(col_arr)-1)*/){
	  				  col_arr_idx = 0;
	  			  } else {
	  				  col_arr_idx++;
	  			  }
	  			cur_color = col_arr[col_arr_idx];
	  			clear_page(3);
	  			write_page(3, col_name[col_arr_idx]);
	  			insert_selector(COLOR);
	  			ssd1306_update_display();
	  			cur_dir = NONE;

	  		  } else if (cur_dir == LEFT){
	  			  if (col_arr_idx == 0){
	  				  col_arr_idx = 8/*(sizeof(col_arr)-1)*/;
	  			  } else {
	  				  col_arr_idx--;
	  			  }
	  			cur_color = col_arr[col_arr_idx];
	  			clear_page(3);
	  			write_page(3, col_name[col_arr_idx]);
	  			insert_selector(COLOR);
	  			ssd1306_update_display();
	  			cur_dir = NONE;
	  		  }
	  		  break;
	  	  }
	  	  case SPEED:{

		  			if (cur_dir == DN){
		  				new_mode_state = BRIGHTNESS;
		  				move_selector(SPEED, BRIGHTNESS);
		  				cur_dir = NONE;
		  			} else if (cur_dir == UP){
		  			  	new_mode_state = COLOR;
		  			  	move_selector(SPEED, COLOR);
		  			  	cur_dir = NONE;
		  			} else {
		  			  	new_mode_state = SPEED;
		  			}

		  			if (cur_dir == RIGHT){
		  				cur_speed = (cur_speed == 100) ? 100 : (cur_speed + 10);
		  				cur_dir = NONE;
		  				clear_page(SPEED);
		  				draw_box((11*(cur_speed/10)),5,9,(5*8)+1);
		  				insert_selector(SPEED);
		  				ssd1306_update_display();
		  			} else if (cur_dir == LEFT){
		  				cur_speed = (cur_speed == 10) ? 10 : (cur_speed - 10);
		  				cur_dir = NONE;
		  				clear_page(SPEED);
		  				draw_box((11*(cur_speed/10)),5,9,(5*8)+1);
		  				insert_selector(SPEED);
		  				ssd1306_update_display();
		  			}

	  		break;
	  	  }
	  	  case BRIGHTNESS:{

		  			if (cur_dir == DN){
		  				new_mode_state = PATTERN;
		  				move_selector(BRIGHTNESS, PATTERN);
		  				cur_dir = NONE;
		  			} else if (cur_dir == UP){
		  			  	new_mode_state = SPEED;
		  			  	move_selector(BRIGHTNESS, SPEED);
		  			  	cur_dir = NONE;
		  			} else {
		  			  	new_mode_state = BRIGHTNESS;
		  			}

		  			if (cur_dir == RIGHT){
		  				cur_brightness = (cur_brightness == 100) ? 100 : (cur_brightness + 10);
		  				cur_dir = NONE;
		  				clear_page(BRIGHTNESS);
		  				draw_box((11*(cur_brightness/10)),5,9,(7*8)+1);
		  				insert_selector(BRIGHTNESS);
		  				ssd1306_update_display();
		  			} else if (cur_dir == LEFT){
		  				cur_brightness = (cur_brightness == 10) ? 10 : (cur_brightness - 10);
		  				cur_dir = NONE;
		  				clear_page(BRIGHTNESS);
		  				draw_box((11*(cur_brightness/10)),5,9,(7*8)+1);
		  				insert_selector(BRIGHTNESS);
		  				ssd1306_update_display();
		  			}

	  		break;
	  	  }
	  }
	  cur_mode_state = new_mode_state;
	  if (led_enable){
	  display_pattern(cur_col_state, cur_color, cur_brightness, cur_speed);
	  }
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLRCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 1000;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 300;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : up_arrow_Pin left_arrow_Pin */
  GPIO_InitStruct.Pin = up_arrow_Pin|left_arrow_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : dn_arrow_Pin right_arrow_Pin */
  GPIO_InitStruct.Pin = dn_arrow_Pin|right_arrow_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	HAL_TIM_Base_Stop_IT(&htim3);
	if (HAL_GPIO_ReadPin (up_arrow_GPIO_Port, up_arrow_Pin)){
		cur_dir = UP;
	}
	else if (HAL_GPIO_ReadPin (dn_arrow_GPIO_Port, dn_arrow_Pin)){
		cur_dir = DN;
	}
	else if (HAL_GPIO_ReadPin (left_arrow_GPIO_Port, left_arrow_Pin)){
		cur_dir = LEFT;
	}
	else if (HAL_GPIO_ReadPin (right_arrow_GPIO_Port, right_arrow_Pin)){
		cur_dir = RIGHT;
	}
	else{
		cur_dir = NONE;
	}
	led_enable = 1;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	led_enable = 0;
	HAL_TIM_Base_Start_IT(&htim3);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
