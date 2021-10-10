/*
    PLAY Embedded - Copyright (C) 2006..2015 Rocco Marco Guglielmi

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    userconf.h
 * @brief   Userlib configuration header.
 * @details Userlib configuration file, this file allows to enable or disable the
 *          various device drivers from your application. You may also use
 *          this file in order to override the device drivers default settings.
 *
 * @addtogroup USER_CONF
 * @{
 */

#ifndef _USERCONF_H_
#define _USERCONF_H_

/**
 * @brief   Enables the RF subsystem.
 */
#if !defined(USERLIB_USE_RF) && USERLIB_USE_RF != 0
#define USERLIB_USE_RF              0
#endif

/*===========================================================================*/
/* RF driver related settings.                                              */
/*===========================================================================*/

/**
 * @brief   Enables the @p rfAcquireBus() and @p rfReleaseBus() APIs.
 * @note    Disabling this option saves both code and data space.
 */
#if !defined(RF_USE_MUTUAL_EXCLUSION) || defined(__DOXYGEN__)
#define RF_USE_MUTUAL_EXCLUSION     TRUE
#endif

/**
 * @brief   TX payload length
 * @note    Should be less or equal to NRF24L01_MAX_PAYLEN
 */
#define RF_PAYLEN                   NRF24L01_MAX_PAYLEN
#endif /* _USERCONF_H_ */

/** @} */
