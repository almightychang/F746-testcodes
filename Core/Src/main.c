/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cocktail.h"
#include "usart.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void
task_block(Cocktail_TaskTypeDef *task)
{
    static uint32_t cnt = 0;
    char            buf[100];
    sprintf(buf, "blocking-1 %d\n", cnt++);
    HAL_UART_Transmit(&huart3, buf, strlen(buf), 10);

    if(cnt > 10) return;
    SET_NEXT_TASK(task, NEW_TASK(ID_POLLING, task_block, NULL));
}
void
task_block2(Cocktail_TaskTypeDef *task)
{
    static uint32_t cnt = 0;
    char            buf[100];
    sprintf(buf, "blocking-2 %d\n", cnt++);
    HAL_UART_Transmit(&huart3, buf, strlen(buf), 10);

    if(cnt > 10) return;
    SET_NEXT_TASK(task, NEW_TASK(ID_POLLING, task_block2, NULL));
}
void task3(Cocktail_TaskTypeDef *task);
void task2(Cocktail_TaskTypeDef *task);
void
task4(Cocktail_TaskTypeDef *task)
{
    char buf[100];
    sprintf(buf, "task4!\n");
    HAL_UART_Transmit(&huart3, buf, strlen(buf), 10);
    //    SET_NEXT_TASK(task, NEW_TASK(COCKTAIL_EXTI_ID(GPIO_PIN_13), task2,
    //    NULL));
}

void
task1(Cocktail_TaskTypeDef *task)
{
    char buf[100];
    sprintf(buf, "task1!\n");
    HAL_UART_Transmit(&huart3, buf, strlen(buf), 10);
}

void
task2(Cocktail_TaskTypeDef *task)
{
    char buf[100];
    sprintf(buf, "task2!\n");
    HAL_UART_Transmit(&huart3, buf, strlen(buf), 10);

    Cocktail_EventTypeDef *event = NEW_EVENT(task->_pcb);
    Cocktail_PcbTypeDef   *pcb;
    INIT_PROCESS(pcb);
    PUT_TASK(pcb, NEW_TASK(ID_POLLING, task_block, NULL));
    WAIT_FOR(event, pcb);

    INIT_PROCESS(pcb);
    PUT_TASK(pcb, NEW_TASK(ID_POLLING, task_block2, NULL));
    WAIT_FOR(event, pcb);

    //    SET_NEXT_TASK(task,
    //                  NEW_TASK(COCKTAIL_TIM_PERIOD_ELAPSED_ID(TIM1), task4,
    //                  NULL));
}

void
task3(Cocktail_TaskTypeDef *task)
{
    char buf[100];
    sprintf(buf, "task3!\n");
    HAL_UART_Transmit(&huart3, buf, strlen(buf), 10);
    // Cocktail_addTask(task->_pcb, Cocktail_newTask(task2, NULL));
    // sprintf(buf, "put task2 to %p\n", task->_pcb);
    // HAL_UART_Transmit(&huart3, buf, strlen(buf), 10);
}
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int
main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU
     * Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the
     * Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USART3_UART_Init();
    MX_USB_OTG_FS_PCD_Init();
    MX_TIM1_Init();
    MX_UART4_Init();
    MX_USART6_Init();
    /* USER CODE BEGIN 2 */
    Cocktail_Init();

    Cocktail_PcbTypeDef *process1;
    INIT_PROCESS(process1);

    PUT_TASK(process1, NEW_TASK(ID_POLLING, task1, NULL));
    PUT_TASK(process1, NEW_TASK(ID_POLLING, task2, NULL));
    PUT_TASK(process1, NEW_TASK(ID_POLLING, task3, NULL));

    SCHEDULE(process1);

    HAL_TIM_Base_Start_IT(&htim1);

    char buf[100];
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while(1)
    {
        EXECUTE(sched[ID_POLLING]);
        // HAL_Delay(100);
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void
SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

    /** Configure LSE Drive Capability
     */
    HAL_PWR_EnableBkUpAccess();

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_BYPASS;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM       = 4;
    RCC_OscInitStruct.PLL.PLLN       = 72;
    RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ       = 3;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler(); }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void
Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state
     */
    __disable_irq();
    while(1) {}
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
void
assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line
       number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
       file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
