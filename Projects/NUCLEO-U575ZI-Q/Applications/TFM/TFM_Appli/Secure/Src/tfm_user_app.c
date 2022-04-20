/**
  ******************************************************************************
  * @file    tfm_user_app.c
  * @author  Bossen WU
  * @brief   This file provides example of user app access.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#ifdef TFM_PSA_API
#include "psa_manifest/tfm_user_app.h"
#include "psa/client.h"
#include "psa/service.h"
#include "psa/internal_trusted_storage.h"
#include "tfm_log.h"
#include "string.h"
#include "stm32u5xx_hal.h"
#include "flash_layout.h"

#ifdef TFM_PARTITION_USER_APP
#define PWD_UID 0x0BEE
typedef psa_status_t (*user_app_func_t)(const psa_msg_t *msg);


static void led_safe_status(void)
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
}

static void led_unsafe_status(void)
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
}

static psa_status_t tfm_user_app_io(const psa_msg_t *msg)
{
    psa_status_t status = PSA_SUCCESS;
    size_t bytes_read = 0;
    uint8_t input_password[4];
    uint8_t stored_password[4];
    const psa_storage_uid_t uid = PWD_UID;
    size_t data_len;
    LOG_MSG("\r\nuser app ipc called\r\n");
    if (msg->in_size[0] != sizeof(input_password))
    {
        status = PSA_ERROR_INVALID_ARGUMENT;
        LOG_MSG("\r\nInput size is not correct\r\n");
        goto error_exit;
    }
    bytes_read = psa_read(msg->handle, 0,
                          &input_password, sizeof(input_password));
                          
    if (bytes_read != sizeof(input_password)) {
        status =  PSA_ERROR_INVALID_ARGUMENT;
        LOG_MSG("\r\nRead out bytes are not correct, %d\r\n",bytes_read);
        goto error_exit;
    }
    /* read PIn store in Secure Storage */
      /* read UID */
      status = psa_its_get(uid, 0, 4, stored_password, &data_len);

    if (status != PSA_SUCCESS) {
      memset(stored_password, sizeof(stored_password),0);
      status = PSA_ERROR_STORAGE_FAILURE;
      goto error_exit;
    }    
    /* need Protected Storage or ITS access to get the pin */
    if (memcmp(input_password, stored_password, data_len) == 0)
    {
      memset(stored_password, sizeof(stored_password),0);
      /* peripheral action */
      led_safe_status();
      LOG_MSG("\r\nPassword is correct !!\r\n");
    }
    else
    {
      LOG_MSG("\r\nPassword is not correct !!\r\n");
      LOG_MSG("\r\nInput is %s\r\n", input_password);
      LOG_MSG("\r\nstored is %s\r\n", stored_password);
      memset(stored_password, sizeof(stored_password),0);
      led_unsafe_status();
      status = PSA_ERROR_INVALID_ARGUMENT;
    }
error_exit:    
    memset(input_password, sizeof(input_password),0);

    return status;
}

static void tfm_user_app_signal_handle(psa_signal_t signal, user_app_func_t pfn)
{
    psa_msg_t msg;
    psa_status_t status;

    status = psa_get(signal, &msg);
    switch (msg.type) {
    case PSA_IPC_CONNECT:
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    case PSA_IPC_CALL:
        status = (psa_status_t)pfn(&msg);
        psa_reply(msg.handle, status);
        break;
    case PSA_IPC_DISCONNECT:
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    default:
        psa_panic();
    }
}

static void init_pwd(void)
{
  struct psa_storage_info_t storage_flags;
  const psa_storage_uid_t uid = PWD_UID;
  psa_status_t status = psa_its_get_info(uid,
                              &storage_flags);
  if (status == PSA_ERROR_DOES_NOT_EXIST)
  {
    uint8_t pwd[4]={'1', '2', '3', '4'};

    psa_its_set(uid, 4, pwd, PSA_STORAGE_FLAG_WRITE_ONCE);
    memset(pwd,sizeof(pwd),0);
    LOG_MSG("\r\n Initializing passwrod....\r\n");
  }
  else
    LOG_MSG("\r\n Password was set previously\r\n");
}
psa_status_t tfm_user_app_init(void)
{
   psa_signal_t signals = 0;
   LOG_MSG("\r\nSetting pawsword\r\n");
   init_pwd();
   LOG_MSG("\r\nPawsword is SET!!\r\n");

   while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        if (signals & TFM_USER_APP_IOCTL_SIGNAL) {
            tfm_user_app_signal_handle(TFM_USER_APP_IOCTL_SIGNAL,
                                 tfm_user_app_io);
        } 
        else {
            psa_panic();
        }
    }
}

#endif
#endif