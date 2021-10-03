/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Simon Goldschmidt
 *
 */
#ifndef LWIP_HDR_LWIPOPTS_H__
#define LWIP_HDR_LWIPOPTS_H__

/*
 * #######Changes made to get LWIP, web server & MQTT to work ChibiOS#######
 * - add lwip.mk to the Makefile and make sure you add to it  $(MQTTFILES) to compile MQTT subsystem
 * - Override the default static IP if needed, see LWIP_IPADDR macro for details in lwipopts.h.
 * - To accommodate bigger MQTT messages, I had to increase MQTT_VAR_HEADER_BUFFER_LEN to 512 in lwipopts.h
 * - web.c didn't work from the get go; to make it work I had to make some changes into lwipopts.h;
 *    i.e. TCPIP_MBOX_SIZE was zero and now it is MEMP_NUM_PBUF (16), same thing
 *    DEFAULT_TCP_RECVMBOX_SIZE & for DEFAULT_ACCEPTMBOX_SIZE else the system will hang
 *  - MEMP_NUM_SYS_TIMEOUT in lwipopts.h I had to change this parameter; otherwise the system will crashes for some reason
 */
/* Fixed settings mandated by the ChibiOS integration.*/
#include "static_lwipopts.h"

/* Optional, application-specific settings.*/
#if !defined(TCPIP_MBOX_SIZE)
#define TCPIP_MBOX_SIZE                 MEMP_NUM_PBUF
#define DEFAULT_TCP_RECVMBOX_SIZE       MEMP_NUM_PBUF
#define DEFAULT_ACCEPTMBOX_SIZE			MEMP_NUM_PBUF
#define MEMP_NUM_SYS_TIMEOUT            40
#define LWIP_SEND_TIMEOUT               500
#define LWIP_SO_SNDTIMEO                1
#endif
/*
 * Add 1600 bytes to heap memory used by the tcp_thread
 * for every MQTT broker you listen to
 */
#define MEM_SIZE						3200 /* Increase to 3062 to handle 2 MQTT brokers*/
#if !defined(TCPIP_THREAD_STACKSIZE)
#define TCPIP_THREAD_STACKSIZE          1024
#endif
#if !defined(LWIP_THREAD_STACK_SIZE)
#define LWIP_THREAD_STACK_SIZE          1024
#endif
#if !defined(MQTT_OUTPUT_RINGBUF_SIZE)
#define MQTT_OUTPUT_RINGBUF_SIZE        512
#endif
#if !defined(MQTT_VAR_HEADER_BUFFER_LEN)
#define MQTT_VAR_HEADER_BUFFER_LEN      512
#endif
/* Use ChibiOS specific priorities. */
#if !defined(TCPIP_THREAD_PRIO)
#define TCPIP_THREAD_PRIO               (LOWPRIO + 1)
#endif
#if !defined(LWIP_THREAD_PRIORITY)
#define LWIP_THREAD_PRIORITY            (LOWPRIO)
#endif
#define LWIP_TCPIP_TIMEOUT              1
#define LWIP_SOCKET                     1
#define LWIP_COMPAT_SOCKETS             1
#define LWIP_SO_SNDTIMEO                1
#define LWIP_SO_RCVTIMEO                1

#define MQTT_BROKER 					IPADDR4_INIT_BYTES(172, 17, 0, 2)
#define MQTT_DASHBOARD_BROKER 			IPADDR4_INIT_BYTES(18,195,236,123)
#define HOST_IP_ADDRESS					IPADDR4_INIT_BYTES(10, 42, 0, 10)
#define MQTT_BROKER_PORT				1883
#define MAX_TOPIC_NAME          		128
#define MAX_MQTT_PAYLOAD 				MQTT_VAR_HEADER_BUFFER_LEN
#if !defined(LWIP_IPADDR)
#define LWIP_IPADDR(p)                  IP4_ADDR(p, 10, 42, 0, 10)//IP4_ADDR(p, 192, 168, 1, 10)
#define LWIP_GATEWAY(p)                 IP4_ADDR(p, 10, 42, 0, 1)//IP4_ADDR(p, 192, 168, 1, 1)
#define LWIP_IPADDR2(p)                 IP4_ADDR(p, 192, 168, 1, 10)
#define LWIP_GATEWAY2(p)                IP4_ADDR(p, 192, 168, 1, 1)
#endif

#ifdef WICED_LWIP_DEBUG
//#define LWIP_DEBUG
#define MEMP_OVERFLOW_CHECK            ( 2 )
#define MEMP_SANITY_CHECK              ( 1 )

#define MEM_DEBUG                      (LWIP_DBG_ON)
#define MEMP_DEBUG                     (LWIP_DBG_ON)
#define PBUF_DEBUG                     (LWIP_DBG_ON)
#define API_LIB_DEBUG                  (LWIP_DBG_ON)
#define API_MSG_DEBUG                  (LWIP_DBG_ON)
#define TCPIP_DEBUG                    (LWIP_DBG_ON)
#define NETIF_DEBUG                    (LWIP_DBG_ON)
#define SOCKETS_DEBUG                  (LWIP_DBG_ON)
#define IP_DEBUG                       (LWIP_DBG_ON)
#define IP_REASS_DEBUG                 (LWIP_DBG_ON)
#define RAW_DEBUG                      (LWIP_DBG_ON)
#define ICMP_DEBUG                     (LWIP_DBG_ON)
#define UDP_DEBUG                      (LWIP_DBG_ON)
#define TCP_DEBUG                      (LWIP_DBG_ON)
#define TCP_INPUT_DEBUG                (LWIP_DBG_ON)
#define TCP_OUTPUT_DEBUG               (LWIP_DBG_ON)
#define TCP_RTO_DEBUG                  (LWIP_DBG_ON)
#define TCP_CWND_DEBUG                 (LWIP_DBG_ON)
#define TCP_WND_DEBUG                  (LWIP_DBG_ON)
#define TCP_FR_DEBUG                   (LWIP_DBG_ON)
#define TCP_QLEN_DEBUG                 (LWIP_DBG_ON)
#define TCP_RST_DEBUG                  (LWIP_DBG_ON)
#define PPP_DEBUG                      (LWIP_DBG_ON)
#define ETHARP_DEBUG                   (LWIP_DBG_ON)
#define IGMP_DEBUG                     (LWIP_DBG_ON)
#define INET_DEBUG                     (LWIP_DBG_ON)
#define SYS_DEBUG                      (LWIP_DBG_ON)
#define TIMERS_DEBUG                   (LWIP_DBG_ON)
#define SLIP_DEBUG                     (LWIP_DBG_ON)
#define DHCP_DEBUG                     (LWIP_DBG_ON)
#define AUTOIP_DEBUG                   (LWIP_DBG_ON)
#define SNMP_DEBUG                     (LWIP_DBG_ON)
#define SNMP_MIB_DEBUG                 (LWIP_DBG_ON)
#define DNS_DEBUG                      (LWIP_DBG_ON)

#define LWIP_DBG_TYPES_ON              (LWIP_DBG_ON)   /* (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT) */
#endif

//#define LWIP_TCP_KEEPALIVE              0
#endif /* LWIP_HDR_LWIPOPTS_H__ */
