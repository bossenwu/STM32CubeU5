/**
 ******************************************************************************
 * @file    user_app.c
 * @author  Bossen WU
 * @brief   This file provides example of user app access.
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

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "region_defs.h"
#include "user_app.h"
#include "com.h"
#include "psa_manifest/sid.h"

/** @defgroup  user_app_Private_Defines Private Defines
 * @{
 */

/**
 * @}
 */

/** @defgroup  user_app_Private_Functions Private Functions
 * @{
 */
static void user_app_print_menu(void);
static psa_status_t user_app_io_action(void);
static psa_status_t user_app_chgpwd(void);

/**
 * @}
 */

/** @defgroup  user_app_Exported_Functions Exported Functions
 * @{
 */

/**
 * @brief  Display the user app Main Menu choices on HyperTerminal
 * @param  None.
 * @retval None.
 */
void user_app_run(void)
{
  uint8_t key = 0;
  uint8_t exit = 0;

  /* Print user app welcome message */
  user_app_print_menu();

  while (exit == 0U)
  {
    key = 0U;

    /* Clean the user input path */
    COM_Flush();

    /* Receive key */
    if (COM_Receive(&key, 1U, COM_UART_TIMEOUT_MAX) == HAL_OK)
    {
      switch (key)
      {
      case '1':
        user_app_io_action();
        break;
      case '2':
        user_app_chgpwd();
        break;
      case 'x':
        exit = 1;
        break;
      default:
        printf("Invalid Number !\r");
        break;
      }
      user_app_print_menu();
    }
  }
}
/**
 * @}
 */

/** @addtogroup  user_app_Private_Functions
 * @{
 */

/**
 * @brief  Display the user app Menu choices on HyperTerminal
 * @param  None.
 * @retval None.
 */
static void user_app_print_menu(void)
{
  printf("\r\n=============== user app Menu ===================\r\n\n");
  printf("  Toggle LED in secured area ---------------------------- 1\r\n\n");
  printf("  Change password --------------------------------------- 2\r\n\n");
  printf("  Previous Menu ----------------------------------------- x\r\n\n");
}

/**
 * @brief  Display the user app on Terminal
 * @param  None.
 * @retval None.
 */
static psa_status_t user_app_io_action(void)
{
  uint8_t pwd[4];
  psa_handle_t handle = PSA_NULL_HANDLE;
  psa_status_t status;
  psa_invec in_vec[1];

  printf("  Key in 4-digit password ---------------------------------\r\n\n");
  scanf("%s", pwd);
  printf("Input is %s \n", pwd);
  printf("sending to secure user app to handle... \r\n");

  in_vec[0].base = pwd;
  in_vec[0].len = sizeof(pwd);

  handle = psa_connect(TFM_USER_APP_IOCTL_SID,
                       TFM_USER_APP_IOCTL_VERSION);
  if (!PSA_HANDLE_IS_VALID(handle))
  {
    return PSA_HANDLE_TO_ERROR(handle);
  }

  status = psa_call(handle, PSA_IPC_CALL,
                    in_vec, 1,
                    NULL, NULL);
  psa_close(handle);
  printf("status = %d\r\n", status);
  return status;
}

/**
 * @brief  Change the password stored in ITS
 * @param  None.
 * @retval None.
 */
static psa_status_t user_app_chgpwd(void)
{
  uint8_t input[8];
  psa_handle_t handle = PSA_NULL_HANDLE;
  psa_status_t status;
  psa_invec in_vec[1];

  printf("  Key in 4-digit old password and 4-digit new password-----\r\n\n");
  scanf("%s", input);
  printf("Input is %s \n", input);
  printf("sending to secure user app to handle... \r\n");

  in_vec[0].base = input;
  in_vec[0].len = sizeof(input);

  handle = psa_connect(TFM_USER_APP_CHGPWD_SID,
                       TFM_USER_APP_CHGPWD_VERSION);
  if (!PSA_HANDLE_IS_VALID(handle))
  {
    return PSA_HANDLE_TO_ERROR(handle);
  }

  status = psa_call(handle, PSA_IPC_CALL,
                    in_vec, 1,
                    NULL, NULL);
  psa_close(handle);

  if (status != PSA_SUCCESS)
  {
    printf("status = %d\r\n", status);
    printf("Password update is failed. \r\n");
  }
  else

    printf("Password update is completed. \r\n");

  return status;
}

/**
 * @}
 */
