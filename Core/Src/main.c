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
#include "switch.h"
#include "usart.h"
#include <stdatomic.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern struct Cocktail_SleepQueueDef sleep_queue;
extern Cocktail_PcbTypeDef          *ptimer;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void
monitor_ptimer()
{
    char buf[100];
    sprintf(buf, "\tptimer(%p)\n", ptimer);
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), 10);
}

void
monitor_exti()
{
    // char buf[100];
    //     sprintf(buf, "\texti: %p\n",
    //             sched[COCKTAIL_TIM_PERIOD_ELAPSED_ID(TIM1)]->pcb_queue.stqh_first);
    //     HAL_UART_Transmit(&huart3, buf, strlen(buf), 10);
}

void
monitor_sleep()
{
    char buf[100];
    sprintf(buf, "\tsleep: ");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), 10);
    Cocktail_SleepTypeDef *s;
    STAILQ_FOREACH(s, &sleep_queue, entries)
    {
        sprintf(buf, "%p ", s->key->caller->publisher);
        HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), 10);
    }
    sprintf(buf, "\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), 10);
}

void
monitor_sched(uint8_t id)
{
    char buf[100];
    sprintf(buf, "\tsched(%d) ", id);
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), 10);

    // Cocktail_PcbTypeDef *p;
    //  sprintf(buf, "f(%p) l(%p): ", STAILQ_FIRST(&sched[id]->pcb_queue),
    //          STAILQ_LAST(&sched[id]->pcb_queue, __Cocktail_PcbTypeDef,
    //                      sched_entries));
    //  HAL_UART_Transmit(&huart3, buf, strlen(buf), 10);
    //  STAILQ_FOREACH(p, &sched[id]->pcb_queue, sched_entries)
    //  {
    //     sprintf(buf, "%p ", p);
    //      HAL_UART_Transmit(&huart3, buf, strlen(buf), 10);
    //  }
    //  sprintf(buf, "\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), 10);
}

void
monitor_all()
{

    monitor_ptimer();
    monitor_exti();
    monitor_sleep();

    monitor_sched(0);
    monitor_sched(1);
    monitor_sched(2);
}

Cocktail_PcbTypeDef *
get_subprocess(TASK_Fn task_fn)
{
    Cocktail_PcbTypeDef *sub_process;
    INIT_PROCESS(sub_process);
    PUT_TASK(sub_process, NEW_TASK(COCKTAIL_POLLING_ID, task_fn, NULL));
    return sub_process;
}
extern struct Cocktail_SleepQueueDef sleep_queue;
TASK_FN(task_block1)
{
    static uint32_t cnt = 0;
    TRACE(LOG_CRITICAL, "task 1");
    if(++cnt > UINT32_MAX) return;
    SLEEP(50);
    SET_NEXT_TASK(NEW_TASK(COCKTAIL_POLLING_ID, task_block1, NULL));
}
TASK_FN(task_block2)
{
    static uint32_t cnt = 0;
    char            buf[100];
    sprintf(buf, "blocking-2 %lu\n", cnt);
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), 10);

    if(++cnt > 10) return;
    SET_NEXT_TASK(NEW_TASK(COCKTAIL_POLLING_ID, task_block2, NULL));
}
TASK_FN(task_block3)
{
    static uint32_t cnt = 0;
    char            buf[100];
    sprintf(buf, "blocking-3 %lu\n", cnt);
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), 10);

    if(++cnt > 10) return;
    SET_NEXT_TASK(NEW_TASK(COCKTAIL_POLLING_ID, task_block3, NULL));
}
TASK_FN(task2);
TASK_FN(task3);
TASK_FN(task4)
{
    char buf[100];
    sprintf(buf, "task4!\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), 10);
    //    SET_NEXT_TASK(task, NEW_TASK(COCKTAIL_EXTI_ID(GPIO_PIN_13), task2,
    //    NULL));
}

TASK_FN(task1)
{
    char buf[100];
    sprintf(buf, "task1!\n");
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), 10);
}

TASK_FN(task2)
{
    char     buf[100];
    uint32_t t = HAL_GetTick();
    sprintf(buf, "[%lu] task2!\n", t);
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), 10);

    // WAIT_FOR(get_subprocess(task_block1));
    WAIT_FOR(get_subprocess(task_block2));
    WAIT_FOR(get_subprocess(task_block3));

    SLEEP(7);
    //        SET_NEXT_TASK(task,
    //                      NEW_TASK(COCKTAIL_TIM_PERIOD_ELAPSED_ID(TIM1),
    //                      task4, NULL));
}

TASK_FN(task3)
{
    uint32_t       t   = HAL_GetTick();
    static uint8_t cnt = 0;
    char           buf[100];
    sprintf(buf, "[%lu] task3 - %d\n", t, cnt++);
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), 10);

    SLEEP(50);
    if(cnt > 5) return;
    SET_NEXT_TASK(NEW_TASK(COCKTAIL_POLLING_ID, task3, NULL));
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

    SCHEDULE(get_subprocess(task_block1));

    PUT_TASK(process1, NEW_TASK(COCKTAIL_POLLING_ID, task1, NULL));
    PUT_TASK(process1, NEW_TASK(COCKTAIL_POLLING_ID, task2, NULL));
    PUT_TASK(process1, NEW_TASK(COCKTAIL_POLLING_ID, task3, NULL));

    // SCHEDULE(process1);
    HAL_TIM_Base_Start_IT(&htim1);

    Switch_InitTypeDef sw1;
    sw1.GPIO_Port      = GPIOC;
    sw1.GPIO_Pin       = GPIO_PIN_13;
    sw1.mode           = SW_EXTI_MODE;
    sw1.sched_id       = COCKTAIL_EXTI_ID(GPIO_PIN_13);

    Switch_TypeDef *sw = Switch_Init(sw1);
    sw->falling_event  = process1;

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while(1)
    {
        // EXECUTE(sched[COCKTAIL_POLLING_ID]);
        Cocktail_Run();
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
