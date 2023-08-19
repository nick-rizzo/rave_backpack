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
#include "globals.h"
#include "led_patterns.h"
#include "led_driver.h"
#include "ssd_1306.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
//I2C_HandleTypeDef hi2c1;

//SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
typedef enum {
	LED_OFF=0, PATTERN=1, COLOR=3, SPEED=5, BRIGHTNESS=7
}ctrl_state;

typedef enum {
	IDLE, STATIC, BREATHE, RAINBOW, METEOR
}pattern_state;

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

GPIO_PinState up_cur = GPIO_PIN_RESET;
GPIO_PinState down_cur = GPIO_PIN_RESET;
GPIO_PinState left_cur = GPIO_PIN_RESET;
GPIO_PinState right_cur = GPIO_PIN_RESET;

GPIO_PinState up_nxt = GPIO_PIN_RESET;
GPIO_PinState down_nxt = GPIO_PIN_RESET;
GPIO_PinState left_nxt = GPIO_PIN_RESET;
GPIO_PinState right_nxt = GPIO_PIN_RESET;

dir_ctrl cur_dir;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


dir_ctrl debounced_polling_qnd(){
	  up_cur = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);
	  down_cur = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1);
	  left_cur = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
	  right_cur = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4);
	  if ( (up_cur != up_nxt) || (down_cur != down_nxt) || (left_cur != left_nxt) || (right_cur != right_nxt) ){
		  HAL_Delay(100);
		  up_cur = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);
		  down_cur = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1);
		  left_cur = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
		  right_cur = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4);

		  up_nxt = up_cur;
		  down_nxt = down_cur;
		  left_nxt = left_cur;
		  right_nxt = right_cur;

		  if (up_cur){
//		  	  HAL_UART_Transmit(&huart2, up_msg, strlen(up_msg), 10000);
			  return UP;

		  }
		  else if (down_cur){
//		  	  HAL_UART_Transmit(&huart2, dn_msg, strlen(dn_msg), 10000);
			  return DN;
		  }
		  else if (left_cur){
//		  	  HAL_UART_Transmit(&huart2, left_msg, strlen(left_msg), 10000);
			  return LEFT;
		  }
		  else if (right_cur){
//		  	  HAL_UART_Transmit(&huart2, right_msg, strlen(right_msg), 10000);
			  return RIGHT;
		  }else{
			  return NONE;
		  }
	  }
	  return NONE;
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
  MX_SPI1_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  dma_buffer_init();
  ssd1306_init();
  HAL_Delay(50);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	  rainbow();
	  cur_dir = debounced_polling_qnd();

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
	  			} else if (cur_dir == UP){
	  			  	new_mode_state = BRIGHTNESS;
	  			  	move_selector(PATTERN, BRIGHTNESS);
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
	  		  	  	  } else if (cur_dir == LEFT){
	  		  	  		  new_col_state = METEOR;
	  		  	  		  clear_page(1);
	  		  	  	  	  write_page(1, "   Meteor   ");
	  		  	  	  	  insert_selector(PATTERN);
	  		  	  	  	  ssd1306_update_display();
	  		  	  	  } else {
	  		  	  		  new_col_state = STATIC;
	  		  	  	  }

	  		  		  static_color(test_red);
	  		  		  break;
	  		  	  }
	  		  	  case (BREATHE):{

	  				  if (cur_dir == RIGHT){
	  		  	  	  	  new_col_state = RAINBOW;
	  		  	  	  	  clear_page(1);
	  		  	  	  	  write_page(1, "  Rainbow   ");
	  		  	  	  	  insert_selector(PATTERN);
	  		  	  	  	  ssd1306_update_display();
	  		  	  	  } else if (cur_dir == LEFT){
	  		  	  	  	  new_col_state = STATIC;
	  		  	  	  	  clear_page(1);
	  		  	  	  	  write_page(1, "   Static   ");
	  		  	  	  	  insert_selector(PATTERN);
	  		  	  	  	  ssd1306_update_display();
	  		  	  	  } else {
	  		  	  	  	  new_col_state = BREATHE;
	  		  	  	  }

	  			 	 breathe(test_red);
	  			 	 break;
	  		  	  }
	  		  	  case (RAINBOW):{

	  		  	  	  if (cur_dir == RIGHT){
	  		  	  	  	  new_col_state = METEOR;
	  		  	  	  	  clear_page(1);
	  		  	  	  	  write_page(1, "   Meteor   ");
	  		  	  	  	  insert_selector(PATTERN);
	  		  	  	  	  ssd1306_update_display();
	  		  	  	  } else if (cur_dir == LEFT){
	  		  	  		  new_col_state = BREATHE;
	  		  	  		  clear_page(1);
	  		  	  		  write_page(1, "  Breathe   ");
	  		  	  		  insert_selector(PATTERN);
	  		  	  		  ssd1306_update_display();
	  		  	  	  } else {
	  		  	  	  	  new_col_state = RAINBOW;
	  		  	  	  }

	  		  		  rainbow();
	  		  		  break;
	  		  	  }
	  		  	  case (METEOR):{

	  		  	  	  if (cur_dir == RIGHT){
	  		  	  	  	  new_col_state = STATIC;
	  		  	  	  	  clear_page(1);
	  		  	  	  	  write_page(1, "   Static   ");
	  		  	  	  	  insert_selector(PATTERN);
	  		  	  	  	  ssd1306_update_display();
	  		  	  	  } else if (cur_dir == LEFT){
	  		  	  	  	  new_col_state = RAINBOW;
	  		  	  	  	  clear_page(1);
	  		  	  	  	  write_page(1, "  Rainbow   ");
	  		  	  	  	  insert_selector(PATTERN);
	  		  	  	  	  ssd1306_update_display();
	  		  	  	  } else {
	  		  	  	  	  new_col_state = METEOR;
	  		  	  	  }

	  		  		  meteor(test_red);
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

		  			} else if (cur_dir == UP){
		  			  	new_mode_state = PATTERN;
		  			  	move_selector(COLOR, PATTERN);
		  			} else {
		  			  	new_mode_state = COLOR;
		  			}

	  		break;
	  	  }
	  	  case SPEED:{

		  			if (cur_dir == DN){
		  				new_mode_state = BRIGHTNESS;
		  				move_selector(SPEED, BRIGHTNESS);
		  			} else if (cur_dir == UP){
		  			  	new_mode_state = COLOR;
		  			  	move_selector(SPEED, COLOR);
		  			} else {
		  			  	new_mode_state = SPEED;
		  			}

	  		break;
	  	  }
	  	  case BRIGHTNESS:{

		  			if (cur_dir == DN){
		  				new_mode_state = PATTERN;
		  				move_selector(BRIGHTNESS, PATTERN);
		  			} else if (cur_dir == UP){
		  			  	new_mode_state = SPEED;
		  			  	move_selector(BRIGHTNESS, SPEED);
		  			} else {
		  			  	new_mode_state = BRIGHTNESS;
		  			}

	  		break;
	  	  }
	  }

	  cur_mode_state = new_mode_state;
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV16;
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
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(WS2812b_data_out_GPIO_Port, WS2812b_data_out_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : up_arrow_Pin dn_arrow_Pin */
  GPIO_InitStruct.Pin = up_arrow_Pin|dn_arrow_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : right_arrow_Pin */
  GPIO_InitStruct.Pin = right_arrow_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(right_arrow_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : left_arrow_Pin */
  GPIO_InitStruct.Pin = left_arrow_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(left_arrow_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : WS2812b_data_out_Pin */
  GPIO_InitStruct.Pin = WS2812b_data_out_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(WS2812b_data_out_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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