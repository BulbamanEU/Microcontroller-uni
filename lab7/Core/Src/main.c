/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body for Three-Lane Runner Game
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "stm32f429i_discovery_lcd.h"

/* Private variables ---------------------------------------------------------*/
DMA2D_HandleTypeDef hdma2d;
LTDC_HandleTypeDef hltdc;
SPI_HandleTypeDef hspi5;
SDRAM_HandleTypeDef hsdram1;

/* Game variables */
typedef enum {
    STATE_READY,
    STATE_RUNNING,
    STATE_GAME_OVER
} GameState_t;

GameState_t current_state = STATE_READY;
int player_lane = 1; // 0, 1, 2
int obstacle_lane = 0;
int obstacle_y = -40;
int score = 0;
uint32_t last_tick = 0;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_FMC_Init(void);
static void MX_LTDC_Init(void);
static void MX_SPI5_Init(void);
static void MX_DMA2D_Init(void);
void Game_Update(void);
void Game_Render(void);
void Reset_Game(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();
  srand(HAL_GetTick());
  SystemClock_Config();

  MX_GPIO_Init();
  MX_FMC_Init();
  MX_LTDC_Init();
  MX_SPI5_Init();
  MX_DMA2D_Init();

  /* Initialize LCD */
  BSP_LCD_Init();
  BSP_LCD_LayerDefaultInit(LCD_FOREGROUND_LAYER, LCD_FRAME_BUFFER);
  BSP_LCD_SelectLayer(LCD_FOREGROUND_LAYER);
  BSP_LCD_DisplayOn();
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  
  Reset_Game();

  while (1)
  {
    /* Button Polling */
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET) { // START
        if (current_state != STATE_RUNNING) {
            Reset_Game();
            current_state = STATE_RUNNING;
            HAL_Delay(200); // Debounce
        }
    }

    if (current_state == STATE_RUNNING) {
        if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2) == GPIO_PIN_RESET) { // LEFT
            if (player_lane > 0) player_lane--;
            HAL_Delay(150); // Debounce
        }
        if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3) == GPIO_PIN_RESET) { // RIGHT
            if (player_lane < 2) player_lane++;
            HAL_Delay(150); // Debounce
        }

        /* Update Game logic every 100ms */
        if (HAL_GetTick() - last_tick >= 100) {
            last_tick = HAL_GetTick();
            Game_Update();
            Game_Render();
        }
    } else {
        /* Static screen for READY or GAME OVER */
        Game_Render();
        HAL_Delay(50);
    }
  }
}

void Reset_Game(void) {
    player_lane = 1;
    obstacle_lane = rand() % 3;
    obstacle_y = -40;
    score = 0;
    current_state = STATE_READY;
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
}

void Game_Update(void) {
    if (current_state != STATE_RUNNING) return;

    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET); // Active game LED
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);

    obstacle_y += 15; // Move obstacle down

    /* Check Collision */
    // Player is at Y=260, height 40. Obstacle height 40.
    if (obstacle_y + 40 >= 260 && obstacle_y <= 300) {
        if (obstacle_lane == player_lane) {
            current_state = STATE_GAME_OVER;
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET); // Collision LED
        }
    }

    /* Check if passed */
    if (obstacle_y > 320) {
        score++;
        obstacle_y = -40;
        obstacle_lane = rand() % 3;
    }
}

void Game_Render(void) {
    char msg[20];
    
    // Clear Screen (or just lanes)
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    
    // Draw Lanes
    BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
    BSP_LCD_DrawLine(80, 0, 80, 320);
    BSP_LCD_DrawLine(160, 0, 160, 320);

    // Draw Player
    BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
    BSP_LCD_FillRect(player_lane * 80 + 20, 260, 40, 40);

    // Draw Obstacle
    if (current_state == STATE_RUNNING || current_state == STATE_GAME_OVER) {
        BSP_LCD_SetTextColor(LCD_COLOR_RED);
        BSP_LCD_FillRect(obstacle_lane * 80 + 20, obstacle_y, 40, 40);
    }

    // Draw Score
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font16);
    sprintf(msg, "SCORE: %d", score);
    BSP_LCD_DisplayStringAt(0, 10, (uint8_t*)msg, CENTER_MODE);

    // Draw Status
    if (current_state == STATE_READY) {
        BSP_LCD_DisplayStringAt(0, 150, (uint8_t*)"PRESS START", CENTER_MODE);
    } else if (current_state == STATE_RUNNING) {
        BSP_LCD_DisplayStringAt(0, 30, (uint8_t*)"RUNNING", CENTER_MODE);
    } else if (current_state == STATE_GAME_OVER) {
        BSP_LCD_SetTextColor(LCD_COLOR_RED);
        BSP_LCD_DisplayStringAt(0, 150, (uint8_t*)"GAME OVER", CENTER_MODE);
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  HAL_PWREx_EnableOverDrive();

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

static void MX_DMA2D_Init(void)
{
  hdma2d.Instance = DMA2D;
  hdma2d.Init.Mode = DMA2D_M2M;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
  hdma2d.Init.OutputOffset = 0;
  hdma2d.LayerCfg[1].InputOffset = 0;
  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0;
  HAL_DMA2D_Init(&hdma2d);
  HAL_DMA2D_ConfigLayer(&hdma2d, 1);
}

static void MX_LTDC_Init(void)
{
  LTDC_LayerCfgTypeDef pLayerCfg = {0};
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 7;
  hltdc.Init.VerticalSync = 3;
  hltdc.Init.AccumulatedHBP = 14;
  hltdc.Init.AccumulatedVBP = 5;
  hltdc.Init.AccumulatedActiveW = 254;
  hltdc.Init.AccumulatedActiveH = 325;
  hltdc.Init.TotalWidth = 260;
  hltdc.Init.TotalHeigh = 327;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  HAL_LTDC_Init(&hltdc);
  
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 240;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 320;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg.FBStartAdress = LCD_FRAME_BUFFER;
  pLayerCfg.ImageWidth = 240;
  pLayerCfg.ImageHeight = 320;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0);
}

static void MX_SPI5_Init(void)
{
  hspi5.Instance = SPI5;
  hspi5.Init.Mode = SPI_MODE_MASTER;
  hspi5.Init.Direction = SPI_DIRECTION_2LINES;
  hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi5.Init.NSS = SPI_NSS_SOFT;
  hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi5.Init.CRCPolynomial = 10;
  HAL_SPI_Init(&hspi5);
}

static void MX_FMC_Init(void)
{
  FMC_SDRAM_TimingTypeDef SdramTiming = {0};
  hsdram1.Instance = FMC_SDRAM_DEVICE;
  hsdram1.Init.SDBank = FMC_SDRAM_BANK1;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_1;

  SdramTiming.LoadToActiveDelay = 2;
  SdramTiming.ExitSelfRefreshDelay = 7;
  SdramTiming.SelfRefreshTime = 4;
  SdramTiming.RowCycleDelay = 7;
  SdramTiming.WriteRecoveryTime = 2;
  SdramTiming.RPDelay = 2;
  SdramTiming.RCDDelay = 2;
  HAL_SDRAM_Init(&hsdram1, &SdramTiming);
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Buttons: PA0 (START), PE2 (LEFT), PE3 (RIGHT) */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* LEDs: PG13 (Green), PG14 (Red) */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

void Error_Handler(void)
{
  __disable_irq();
  while (1) {}
}
