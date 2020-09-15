/*
 * MqttHelper.h
 *
 *  Created on: Mar 26, 2020
 *      Author: abusous2000
 */
#include "Strust4EmbeddedConf.h"
#if S4E_USE_MQTT != 0
#ifndef SOURCE_MQTT_MQTTHELPER_H_
#define SOURCE_MQTT_MQTTHELPER_H_
#include "ch.h"
#include "hal.h"
#include <dbgtrace.h>
#include "lwip/pbuf.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"
#include "lwip/netif.h"
#include <string.h>
#include "hashmap.h"

#ifndef GET_TIME_TOPIC_NAME
#define GET_TIME_TOPIC_NAME      "dev/getTime"
#endif

struct MqttConnection_Typedef;
struct MqttSubscribeInfo_Typedef;
typedef void (*onMessage_t)(struct MqttConnection_Typedef *pMqttConnection, struct MqttSubscribeInfo_Typedef *pSubscribeInfo);
typedef void (*onConnection_t)(struct MqttConnection_Typedef *pMqttConnection);
typedef void (*onDisconnection_t)(struct MqttConnection_Typedef *pMqttConnection, mqtt_connection_status_t status);

typedef struct MqttConnection_Typedef {
	mqtt_client_t 			    		*client;
	ip_addr_t                   		brokerIpAddr;
	uint16_t                    		brokerPort;
	struct mqtt_connect_client_info_t  *pClientInfo;
	char							    incommingTopic[MAX_TOPIC_NAME];
	char 								*payload;
	onMessage_t	    					defaultOnMessage;
	onConnection_t						onConnection;
	onDisconnection_t					onDisconnection;
	uint32_t                    		called;
	uint32_t                    		error;
	map_t 								*mapSubscribed;
} MqttConnection_Typedef;
typedef struct MqttSubscribeInfo_Typedef {
	const char							*topic;
	uint8_t								qos;
	err_t                       		result;
	onMessage_t	    					onMessage;
	MqttConnection_Typedef				*pMqttConnection;
} MqttSubscribeInfo_Typedef;
typedef struct MqttPublishInfo_Typedef {
	const char							*topic;
	uint8_t								qos;
	err_t                       		result;
	uint8_t								retain;
	MqttConnection_Typedef				*pMqttConnection;
} MqttPublishInfo_Typedef;

#ifdef __cplusplus
 extern "C" {
#endif

err_t mqttBrokerSubscribe(MqttConnection_Typedef *pMqttConnection, MqttSubscribeInfo_Typedef *pSubscribeInfo);
err_t mqttBrokerPublishMessage(MqttConnection_Typedef *pMqttConnection, MqttPublishInfo_Typedef *pPublishInfo, char *payload);
err_t mqttBrokerConnect(MqttConnection_Typedef *pMqttConnection);
void mqttBrokerDisconnect(MqttConnection_Typedef *pMqttConnection);
bool isMQTTBrokerConnected(MqttConnection_Typedef *pMqttConnection);

#ifdef __cplusplus
}
#endif


#endif /* SOURCE_MQTT_MQTTHELPER_H_ */
#endif /* S4E_USE_MQTT */
