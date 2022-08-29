/*
 * Ethernet.c
 *
 *  Created on: Mar 27, 2020
 *      Author: abusous2000
 */
#include "Strust4EmbeddedConf.h"
#if S4E_USE_ETHERNET != 0
#include "ch.h"
#include "hal.h"
#include "Strust4Embedded.h"
#include "MQTTClient.h"
#include "lwipthread.h"
#include "ccportab.h"

//A weak/virtual function that could be overridden in other modules & called once every 1 seconds
const char *getTimeMsg = "{\"output\":1,\"getTime\":\"unixtime\"}";
CC_WEAK void onDefaultMQTTBrokerConnect(MqttConnection_Typedef *pMqttConnection){(void)pMqttConnection;
	MqttPublishInfo_Typedef   getUnixTimeMQTTTopicInfo      = {.topic=GET_TIME_TOPIC_NAME, .qos = 0, .retain = 0};

	chThdSleepMilliseconds(20);
	sendToTopicMQTTQueue(&getUnixTimeMQTTTopicInfo,(char *)getTimeMsg);
}

CC_WEAK void onDefaultMQTTBrokerDisconnect(MqttConnection_Typedef *pMqttConnection){(void)pMqttConnection;
}
#if S4E_USE_MQTT != 0
static struct mqtt_connect_client_info_t clientInfo = {
  .client_id=MQTT_CLIENT_ID,
  .client_user=NULL,
  .client_pass=NULL,
  .keep_alive=30,
  .will_topic=NULL,
  .will_msg=NULL,
  .will_qos=0,
  .will_retain=0
};
static struct mqtt_connect_client_info_t clientInfo2 = {
  .client_id=MQTT_CLIENT_ID,
  .client_user=NULL,
  .client_pass=NULL,
  .keep_alive=30,
  .will_topic=NULL,
  .will_msg=NULL,
  .will_qos=0,
  .will_retain=0
};
void publishStatusToBroker(void);
static void onMessage(MqttConnection_Typedef *pMqttConnection, MqttSubscribeInfo_Typedef *pSubscribeInfo);
static void defaultOnMessage(MqttConnection_Typedef *pMqttConnection, MqttSubscribeInfo_Typedef *pSubscribeInfo);
static void onBrokerDisconnection(MqttConnection_Typedef *pMqttConnection, mqtt_connection_status_t status);
static void onBrokerConnection(MqttConnection_Typedef *pMqttConnection);

static MqttPublishInfo_Typedef   defaultMQTTTopicInfo      = {.topic=DEFAULT_MQTT_PUBLISH_TOPIC, .qos = 0, .retain = 0};
static MqttSubscribeInfo_Typedef subscribeInfo             = {.topic=MQTT_CMD_SUBSCRIBE_TOPIC, .onMessage=onMessage, .qos = 0};
//static MqttSubscribeInfo_Typedef subscribeInfo2 = {.topic=MQTT_SUBSCRIBE_TOPIC2,.onMessage=onMessage,.qos = 0};
static MqttConnection_Typedef defaultBroker      = {.brokerIpAddr=MQTT_BROKER,
                                              	   .brokerPort=MQTT_BROKER_PORT,
												   .defaultOnMessage=defaultOnMessage,
												   .onConnection=onBrokerConnection,
												   .onDisconnection=onBrokerDisconnection,
												   .mapSubscribed=NULL,
												   .payload=NULL,
												   .pClientInfo=&clientInfo};
static MqttConnection_Typedef mqttDashboardBroker   = {.brokerIpAddr=MQTT_DASHBOARD_BROKER,
                                              	   .brokerPort=MQTT_BROKER_PORT,
												   .defaultOnMessage=defaultOnMessage,
												   .onConnection=onBrokerConnection,
												   .onDisconnection=onBrokerDisconnection,
												   .mapSubscribed=NULL,
												   .payload=NULL,
												   .pClientInfo=&clientInfo2};
static void defaultOnMessage(MqttConnection_Typedef *pMqttConnection, MqttSubscribeInfo_Typedef *pSubscribeInfo){(void)pSubscribeInfo;(void)pMqttConnection;
    /* Don't trust the publisher, check zero termination */
	dbgprintf("+++Thd:%s-->Handled by generic message handler. Msg from Topic:%s ==>%.*s\r\n", chThdGetSelfX()->name, pMqttConnection->incommingTopic,pMqttConnection->payload);

	return;
}
static void onMessage(MqttConnection_Typedef *pMqttConnection, MqttSubscribeInfo_Typedef *pSubscribeInfo){(void)pSubscribeInfo;(void)pMqttConnection;
    /* Don't trust the publisher, check zero termination */
	dbgprintf("+++Thd:%s-->Msg from Topic:%s ==>%s\r\n", chThdGetSelfX()->name, pMqttConnection->incommingTopic,pMqttConnection->payload);
	return;
}
static void onBrokerConnection(MqttConnection_Typedef *pMqttConnection){
	mqttBrokerSubscribe(pMqttConnection,&subscribeInfo);
	onDefaultMQTTBrokerConnect(pMqttConnection);
    return;
}
#define MAX_RETRY_AFTER_BROKER_DISCONNECTS  3
static void onBrokerDisconnection(MqttConnection_Typedef *pMqttConnection, mqtt_connection_status_t status){(void)status;
    pMqttConnection->error++;
	dbgprintf("+++Thd:%s-->Disconnected from Broker:%d:%d for reason:%d\n",chThdGetSelfX()->name, pMqttConnection->brokerIpAddr,pMqttConnection->brokerPort, status);
	mqttBrokerDisconnect(pMqttConnection);

    if ( pMqttConnection->error < MAX_RETRY_AFTER_BROKER_DISCONNECTS ){
    	sysinterval_t timeout = 50+10*(pMqttConnection->error%10);
    	dbgprintf("+++Thd:%s-->Reconnecting......Errors=%d...retrying in %d ms\r\n",chThdGetSelfX()->name, pMqttConnection->error,timeout);
    	chThdSleepMilliseconds(timeout);
    	mqttBrokerConnect(pMqttConnection);
    }
    else
    	dbgprintf("+++Thd:%s-->No need to reconnect to broker...too many errors while reconnecting",chThdGetSelfX()->name);

    onDefaultMQTTBrokerDisconnect(pMqttConnection);
    return;
}
bool isDefaultMQTTBrokerConnected(void){
    return isMQTTBrokerConnected(&defaultBroker);
}
void reconnectDefaultMQTTBroker(void){
	if ( isDefaultMQTTBrokerConnected() == false )
		mqttBrokerConnect(&defaultBroker);
}
#if 1
void sendToTopicMQTTQueue(MqttPublishInfo_Typedef *pPublishInfo, char *payload){
	MqttConnection_Typedef *pMqttConnection=&defaultBroker;

	if ( isMQTTBrokerConnected(pMqttConnection) ){
		 err_t err = mqttBrokerPublishMessage(pMqttConnection,pPublishInfo,payload);
		 if ( err != ERR_OK){
			 //After three failed attempts; disconnect from broker. Once that is detected be the driver
			 //it will automatically call onBrokerDisconnection() and re-connect will be attempted
			 if ( ++pMqttConnection->error >= MAX_RETRY_AFTER_BROKER_DISCONNECTS)
				mqttBrokerDisconnect(pMqttConnection);
			 return;
		 }
		 else
	        pMqttConnection->error = 0;
	}
    else
	    dbgprintf("+++Thd:%s-->Broker Disconnected\r\n", chThdGetSelfX()->name);
}
#else
void sendToTopicMQTTQueue(MqttPublishInfo_Typedef *pPublishInfo, char *payload){
	MqttConnection_Typedef *pMqttConnection=&defaultBroker;

	if ( isMQTTBrokerConnected(pMqttConnection) ){
		 err_t err = mqttBrokerPublishMessage(pMqttConnection,pPublishInfo,payload);
		 if ( err != ERR_OK){

			 if ( ++pMqttConnection->error >= MAX_RETRY_AFTER_BROKER_DISCONNECTS)
				mqttBrokerDisconnect(pMqttConnection);
		 }
	}
	else
		 pMqttConnection->error = 0;

	if ( isMQTTBrokerConnected(pMqttConnection) )
		mqttBrokerPublishMessage(pMqttConnection,pPublishInfo,payload);
}
#endif
void sendToDefaultMQTTQueue(char *payload){
	sendToTopicMQTTQueue(&defaultMQTTTopicInfo,payload);
}

void publishStatusToBroker(void){
	char 					_payload[256] ={0};
	MqttConnection_Typedef  *pMqttConnection=&defaultBroker;
	chsnprintf(_payload,sizeof(_payload),"{\"track\":\"Crazy 4 u\",\"status\":\"Playing\",\"vol\":%d,\"seconds_remaining\":%d,\"timeRemaining\":\"%3:30\"}",
	                                          getCurrentVolume(),
											  getCurrentVolume()+55);
	sendToDefaultMQTTQueue(_payload);
}


#endif/*S4E_USE_MQTT*/

void initMQTTClient(void){
    lwipInit(NULL);

#if S4E_USE_WEB_SERVER != 0
    //httpd_init();/*This is LWIP version of a web server*/
    chThdCreateStatic(wa_http_server, sizeof(wa_http_server), NORMALPRIO + 1, http_server, NULL);
#endif
#if S4E_USE_MQTT != 0
    defaultBroker.payload = chHeapAlloc(NULL,MAX_MQTT_PAYLOAD);
    chDbgCheck(defaultBroker.payload != NULL);
    mqttBrokerConnect(&defaultBroker);
//    chThdSleepMilliseconds(5000);
//    mqttBrokerConnect(&mqttDashboardBroker);
#endif
}
#endif/*S4E_USE_ETHERNET*/
