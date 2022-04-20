/**
  ******************************************************************************
  * @file    tfm_user_app.h
  * @author  Bossen WU
  * @brief   This file contains definitions for tkms application examples.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TFM_USER_APP_H
#define TFM_USER_APP_H

#ifdef __cplusplus
extern "C" {
#endif


/* Exported prototypes ------------------------------------------------------ */
psa_status_t tfm_user_app_init(void);

#ifdef __cplusplus
}
#endif

#endif /* TFM_USER_APP_H */