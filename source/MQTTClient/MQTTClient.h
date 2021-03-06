/*
 * Ethernet.h
 *
 *  Created on: Mar 27, 2020
 *      Author: abusous2000
 */

#ifndef SOURCE_MQTTCLIENT_MQTTCLIENT_H_
#define SOURCE_MQTTCLIENT_MQTTCLIENT_H_
#include "Strust4EmbeddedConf.h"

#if S4E_USE_ETHERNET != 0
#include "MqttHelper.h"
#include "ccportab.h"
#ifdef __cplusplus
 extern "C" {
#endif
void initMQTTClient(void);
void mqttBrokerHealthCheck(void);
void sendToDefaultMQTTQueue(char *payload);
void sendToTopicMQTTQueue(MqttPublishInfo_Typedef *pPublishInfo, char *payload);
bool isDefaultMQTTBrokerConnected(void);
void publishStatusToBroker(void);
void onDefaultMQTTBrokerDisconnect(MqttConnection_Typedef *pMqttConnection);
void onDefaultMQTTBrokerConnect(MqttConnection_Typedef *pMqttConnection);
void reconnectDefaultMQTTBroker(void);
#ifdef __cplusplus
}
#endif
#endif/* S4E_USE_ETHERNET  != 0 */

#endif /* SOURCE_MQTTCLIENT_MQTTCLIENT_H_ */
