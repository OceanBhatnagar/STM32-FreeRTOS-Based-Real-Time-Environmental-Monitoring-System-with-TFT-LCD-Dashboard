/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include"bmp.h"
#include"bsp_lcd.h"
#include "dht.h"
#include"MQ135.h"
#include"rtc.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include<stdbool.h>
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

typedef struct {
    uint8_t hr, min, sec;
    int32_t pressure;
    int32_t ppm;
    uint8_t temp;
    uint8_t humidity;
} SystemData_t;

SystemData_t gData;
osMutexId_t dataMutex;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c2;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim11;

UART_HandleTypeDef huart2;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */

osThreadId_t Task1_Handle;
  const osThreadAttr_t Task1_Handle_attributes = {
    .name = "Task1",
    .stack_size = 256 * 4,
    .priority = (osPriority_t) osPriorityBelowNormal,
  };

osThreadId_t Task2_Handle;
  const osThreadAttr_t Task2_Handle_attributes = {
    .name = "Task2",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,
  };

  osThreadId_t Task3_Handle;
  const osThreadAttr_t Task3_Handle_attributes = {
    .name = "Task3",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)  osPriorityBelowNormal,
  };


  osThreadId_t Task4_Handle;
  const osThreadAttr_t Task4_Handle_attributes = {
    .name = "Task4",
    .stack_size = 246 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal,
  };


  osThreadId_t Task5_Handle;
  const osThreadAttr_t Task5_Handle_attributes = {
    .name = "Task5",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal,
  };
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C2_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM11_Init(void);
void StartDefaultTask(void *argument);

/* USER CODE BEGIN PFP */
void Task_DHT(void *argument);
void Task_BMP(void *argument);
void Task_MQ135(void *argument);
void Task_RTC(void *argument);
void Task_LCD(void *argument);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_I2C2_Init();
  MX_RTC_Init();
  MX_SPI1_Init();
  MX_TIM11_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim11);

  /* USER CODE END 2 */
  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  // Create mutex AFTER kernel init
  dataMutex = osMutexNew(NULL);
  if (dataMutex == NULL) Error_Handler();

  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  Task1_Handle = osThreadNew(Task_DHT, NULL, &Task1_Handle_attributes);
  Task2_Handle = osThreadNew(Task_BMP, NULL, &Task2_Handle_attributes);
  Task3_Handle = osThreadNew(Task_MQ135, NULL, &Task3_Handle_attributes);
  Task4_Handle = osThreadNew(Task_RTC, NULL, &Task4_Handle_attributes);
  Task5_Handle = osThreadNew(Task_LCD, NULL, &Task5_Handle_attributes);

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
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
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 15;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM11 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM11_Init(void)
{

  /* USER CODE BEGIN TIM11_Init 0 */

  /* USER CODE END TIM11_Init 0 */

  /* USER CODE BEGIN TIM11_Init 1 */

  /* USER CODE END TIM11_Init 1 */
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 84-1;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 65535;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM11_Init 2 */

  /* USER CODE END TIM11_Init 2 */

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
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA4 PA8 PA9 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


  /* USER CODE BEGIN MX_GPIO_Init_2 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // <--- Open Drain
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void Task_DHT(void *argument)
{
    DHT11_Init();
    uint8_t temp = 0, humidity = 0;

    // Save the original priority (BelowNormal) to restore it later
    osPriority_t normalPriority = osThreadGetPriority(Task1_Handle);

    for(;;)
    {
        // 1. Boost priority to prevent other tasks from interrupting the timing
        osThreadSetPriority(Task1_Handle, osPriorityRealtime);

        // 2. Call the updated Read function (defined below)
        int8_t status1 = DHT11_ReadData(GPIO_PIN_8, &temp, &humidity);

        // 3. Immediately lower priority back to let other tasks run
        osThreadSetPriority(Task1_Handle, normalPriority);

        if (status1 == 0) // Success
        {
            osMutexAcquire(dataMutex, osWaitForever);
            gData.temp = temp;
            gData.humidity = humidity;
            osMutexRelease(dataMutex);
        }
        else
        {
            // Optional: Set to 0 or leave previous value if read fails
            // gData.temp = 0;
        }

        // DHT11 needs at least 2 seconds between reads to remain stable
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
void Task_RTC(void *argument)
{
    uint8_t h, m, s, d, mo, y;

    for(;;)
    {
        rtc_get_time_date(&h, &m, &s, &d, &mo, &y);

        osMutexAcquire(dataMutex, osWaitForever);
        gData.hr = h;
        gData.min = m;
        gData.sec = s;
        osMutexRelease(dataMutex);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
void Task_BMP(void *argument)
{
    int32_t temp_adc, pressure_adc;
    int32_t comp_temp, comp_pressure, comp_alt;
      BMP_Init();
    for(;;)
    {
        BMP_ReadData(&temp_adc, &pressure_adc);
        BMP_ProcessData(temp_adc, pressure_adc, &comp_temp, &comp_pressure, &comp_alt);

        osMutexAcquire(dataMutex, osWaitForever);
        gData.pressure = comp_pressure;
        osMutexRelease(dataMutex);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void Task_MQ135(void *argument)
{
    int32_t ppm, sum;

    for(;;)
    {
        sum = 0;
        for(int i = 0; i < 10; i++)
        {
            MQ135_Read(&ppm);
            sum += ppm;
            vTaskDelay(pdMS_TO_TICKS(50));
        }

        osMutexAcquire(dataMutex, osWaitForever);
        gData.ppm = sum / 10;
        osMutexRelease(dataMutex);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void Task_LCD(void *argument)
{
    // 1. COLORS (RGB888 format as expected by your bsp_lcd_convert_rgb888_to_rgb666)
    const uint32_t BG_COLOR     = 0x800000; // Dark Red
    const uint32_t TEXT_COLOR   = 0xFFFFFF; // White
    const uint32_t LABEL_COLOR  = 0x00FFFF; // Cyan
    const uint32_t VALUE_COLOR  = 0x00FF00; // Green

    char buffer[32];
    SystemData_t localCopy = {0};
    SystemData_t prevData = {0};
    static uint16_t pos = 0;

    // 2. INITIALIZATION
    BSP_LCD_Init();
    // Fill background (Uses your optimized rect fill)
    bsp_lcd_set_background_color(BG_COLOR);

    // 3. DRAW STATIC UI (Only once)
    bsp_lcd_draw_string(30, 10, "ENV MONITOR", &Font24, LABEL_COLOR, BG_COLOR);
    bsp_lcd_fill_rect(0xFFFFFF, 10, 220, 45, 2); // Separator line

    bsp_lcd_draw_string(10, 60,  "Time :", &Font24, TEXT_COLOR, BG_COLOR);
    bsp_lcd_draw_string(10, 110, "Press:", &Font24, TEXT_COLOR, BG_COLOR);
    bsp_lcd_draw_string(10, 160, "Air  :", &Font24, TEXT_COLOR, BG_COLOR);
    bsp_lcd_draw_string(10, 210, "Temp :", &Font24, TEXT_COLOR, BG_COLOR);
    bsp_lcd_draw_string(10, 260, "Hum  :", &Font24, TEXT_COLOR, BG_COLOR);

    for(;;)
    {
        // 4. FETCH DATA
        bool dataValid = false;
        if(osMutexAcquire(dataMutex, pdMS_TO_TICKS(20)) == osOK)
        {
            localCopy = gData;
            osMutexRelease(dataMutex);
            dataValid = true;
        }

        // 5. UPDATE VALUES (Only if data is new and changed)
        if(dataValid && (memcmp(&localCopy, &prevData, sizeof(SystemData_t)) != 0))
        {
            // Note: We use trailing spaces in snprintf to clear old characters
            // without calling fill_rect, which prevents flickering.

            // TIME
            snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d",
                     localCopy.hr, localCopy.min, localCopy.sec);
            bsp_lcd_draw_string(140, 60, buffer, &Font24, VALUE_COLOR, BG_COLOR);

            // PRESSURE
            snprintf(buffer, sizeof(buffer), "%ld Pa  ", localCopy.pressure);
            bsp_lcd_draw_string(140, 110, buffer, &Font24, TEXT_COLOR, BG_COLOR);

            // AIR (Conditional Color)
            uint32_t airColor = (localCopy.ppm > 400) ? 0xFF0000 : 0x00FF00;
            snprintf(buffer, sizeof(buffer), "%ld ppm  ", localCopy.ppm);
            bsp_lcd_draw_string(140, 160, buffer, &Font24, airColor, BG_COLOR);

            // TEMP
            snprintf(buffer, sizeof(buffer), "%d C  ", localCopy.temp);
            bsp_lcd_draw_string(140, 210, buffer, &Font24, 0xFFFF00, BG_COLOR);

            // HUMIDITY
            snprintf(buffer, sizeof(buffer), "%d %%  ", localCopy.humidity);
            bsp_lcd_draw_string(140, 260, buffer, &Font24, 0x00FFFF, BG_COLOR);

            prevData = localCopy;
        }

        // 6. ANIMATION (Bottom bar)
        uint16_t anim_x = 10;
        uint16_t anim_y = 290;
        uint16_t anim_w = 220;
        uint16_t anim_h = 8;
        uint16_t bar_w  = 20;

        // Clear the old bar position by drawing background over it
        bsp_lcd_fill_rect(BG_COLOR, anim_x + pos, bar_w, anim_y, anim_h);

        // Calculate new position
        pos += 10;
        if(pos > (anim_w - bar_w))
        {
            pos = 0;
        }

        // Draw new bar position
        bsp_lcd_fill_rect(0x00FFFF, anim_x + pos, bar_w, anim_y, anim_h);

        // RTOS Delay - 100ms for ~10 FPS animation
        osDelay(100);
    }
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
#ifdef USE_FULL_ASSERT
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
