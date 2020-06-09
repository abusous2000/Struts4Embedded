#include "Strust4EmbeddedConf.h"
#if S4E_USE_MQTT == 1
#include "ch.h"
#include "hal.h"
#include "CommonHelper.h"
#include "MqttHelper.h"

/* Called when publish is complete either with sucess or failure */
static void mqttPublishRequestResultCB(void *arg, err_t result){
	MqttPublishInfo_Typedef *pPublishInfo = (MqttPublishInfo_Typedef*)arg;
	if ( result == ERR_OK )
		pPublishInfo->pMqttConnection->called++;
	pPublishInfo->result = result;
	if ( pPublishInfo->pMqttConnection->called % 10 == 0 || result != ERR_OK)
		dbgprintf("+++Thd:%s--Call:%d-->>Publish result on topic:%s:%d\r\n",
				chThdGetSelfX()->name, pPublishInfo->pMqttConnection->called, pPublishInfo->topic,result);
	return;
}

static void mqttSubscriptionRequestResultCB(void * arg, err_t result) {
	MqttSubscribeInfo_Typedef *pSubscribeInfo = (MqttSubscribeInfo_Typedef*)arg;
    dbgprintf("+++Thd:%s-->Subscribe result: %d\n", chThdGetSelfX()->name, result);
    pSubscribeInfo->result = result;
    return;
}
static void mqttIncomingPublishCB(void *arg, const char *topic, u32_t tot_len){(void)tot_len;
	MqttConnection_Typedef *pMqttConnection = (MqttConnection_Typedef*)arg;

	strlcpy(pMqttConnection->incommingTopic,topic,MAX_TOPIC_NAME);
	return;
}
static void onGenericMessage(MqttConnection_Typedef *pMqttConnection, const u8_t *data, u16_t len){(void)len;
    /* Don't trust the publisher, check zero termination */
	dbgprintf("+++Thd:%s-->Handled by generic message handler. Msg from Topic:%s ==>%.*s\r\n", chThdGetSelfX()->name, pMqttConnection->incommingTopic,len, (const char *)data);

	return;
}
static MqttSubscribeInfo_Typedef *getSubscriber(MqttConnection_Typedef *pMqttConnection, char *topic){
	MqttSubscribeInfo_Typedef *pSubscribeInfo = NULL;

	hashmap_get(pMqttConnection->mapSubscribed,topic,(any_t)&pSubscribeInfo);

	return pSubscribeInfo;
}
static void mqttIncomingMessageCB(void *arg, const u8_t *data, u16_t len, u8_t flags){
	MqttConnection_Typedef *pMqttConnection = (MqttConnection_Typedef*)arg;

	dbgprintf("+++Thd:%s-->Incoming payload with length %d, flags %u\r\n", chThdGetSelfX()->name,len, (unsigned int)flags);
    if(flags & MQTT_DATA_FLAG_LAST) {
    	pMqttConnection->called++;

   		if ( len > (MAX_MQTT_PAYLOAD-1)){
			dbgprintf("****ERROR: MQTT Payload is larger than max. allowed*****\r\n", pMqttConnection->incommingTopic,len, (const char *)data);
			onGenericMessage(pMqttConnection,data,len);
			return;
    	}
    	strlcpy(pMqttConnection->payload,(char*)data,len+1);
    	bool handledActionEvent = handleActionPayloads(pMqttConnection->payload,SOURCE_EVENT_MQTT);

    	if ( handledActionEvent ){
    		dbgprintf("+++Thd:%s-->Routed Payload to AE queue.\r\n", chThdGetSelfX()->name);
    		return;
    	}

    	MqttSubscribeInfo_Typedef *pSubscribeInfo = getSubscriber(pMqttConnection,pMqttConnection->incommingTopic);
    	if ( pSubscribeInfo != NULL && pSubscribeInfo->onMessage !=  NULL){
     		pSubscribeInfo->onMessage(pMqttConnection,pSubscribeInfo);
    	}
    	else
    	if ( pMqttConnection->defaultOnMessage != NULL )
    		pMqttConnection->defaultOnMessage(pMqttConnection,pSubscribeInfo);
        else
    	    onGenericMessage(pMqttConnection,data,len);
    }
    else {
    	/* Handle fragmented payload, store in buffer, write to file or whatever */
    }
    return;
}
static void mqttConnectionCB(mqtt_client_t *client, void *arg, mqtt_connection_status_t status){
	MqttConnection_Typedef *pMqttConnection = (MqttConnection_Typedef*)arg;

	if (status == MQTT_CONNECT_ACCEPTED) {
	    pMqttConnection->error  = 0;
		if ( pMqttConnection->onConnection != NULL){
			dbgprintf("+++Thd:%s-->Successfully connected to Broker:%d:%d\n",chThdGetSelfX()->name, pMqttConnection->brokerIpAddr,pMqttConnection->brokerPort);
			/* Setup callback for incoming publish requests */
		    mqtt_set_inpub_callback(client, mqttIncomingPublishCB, mqttIncomingMessageCB, pMqttConnection);
			pMqttConnection->onConnection(pMqttConnection);
		}
   } else {
		if ( pMqttConnection->onDisconnection != NULL)
			pMqttConnection->onDisconnection(pMqttConnection,status);
   }

   return;
}
err_t mqttBrokerSubscribe(MqttConnection_Typedef *pMqttConnection, MqttSubscribeInfo_Typedef *pSubscribeInfo){
	pSubscribeInfo->pMqttConnection = pMqttConnection;
	err_t err = mqtt_subscribe(pMqttConnection->client, pSubscribeInfo->topic, pSubscribeInfo->qos,
			                   mqttSubscriptionRequestResultCB , pSubscribeInfo);

	if ( getSubscriber(pMqttConnection,(char*)pSubscribeInfo->topic) == NULL)
		hashmap_put(pMqttConnection->mapSubscribed, (char*)pSubscribeInfo->topic, (any_t)pSubscribeInfo);
    dbgprintf("+++Thd:%s-->subscribed to topic:%s with return: %d\n", chThdGetSelfX()->name, pSubscribeInfo->topic,err);

	return err;
}

err_t mqttBrokerPublishMessage(MqttConnection_Typedef *pMqttConnection, MqttPublishInfo_Typedef *pPublishInfo, char *payload){
	pPublishInfo->pMqttConnection = pMqttConnection;
    err_t err = mqtt_publish(pMqttConnection->client, pPublishInfo->topic, payload, strlen(payload),
    						 pPublishInfo->qos, pPublishInfo->retain, mqttPublishRequestResultCB, pPublishInfo);
    if ( err != ERR_OK)
    	dbgprintf("+++Thd:%s-->Publish err: %d\r\n", chThdGetSelfX()->name, err);

    return err;
}
err_t mqttBrokerConnect(MqttConnection_Typedef *pMqttConnection){
	if ( pMqttConnection->client != NULL )
		mqttBrokerDisconnect(pMqttConnection);
	pMqttConnection->called = 0;
	pMqttConnection->mapSubscribed = pMqttConnection->mapSubscribed==NULL?hashmap_new(20):pMqttConnection->mapSubscribed;;
	/*
	 *TCP_Server allocated 1600 bytes by default (see MEM_SIZE) for the its dynamic heap/pool
	 *If you use more than one MQTT broker, plz increase this pool by 1600 bytes per broker,
	 *else get null memory pointer returned from mqtt_client_new().
	 */
	pMqttConnection->client = mqtt_client_new();
	if ( pMqttConnection->client == NULL )
		return ERR_MEM;
	err_t err = mqtt_client_connect(pMqttConnection->client, &pMqttConnection->brokerIpAddr, pMqttConnection->brokerPort,
			                        mqttConnectionCB, pMqttConnection, pMqttConnection->pClientInfo);

	return err;
}
bool isMQTTBrokerConnected(MqttConnection_Typedef *pMqttConnection){
	return pMqttConnection != NULL && pMqttConnection->client != NULL && mqtt_client_is_connected(pMqttConnection->client);
}
void mqttBrokerDisconnect(MqttConnection_Typedef *pMqttConnection){
	if ( pMqttConnection->client != NULL){
		mqtt_disconnect(pMqttConnection->client);
		 /* fixme: mqtt_client_fre() is missing... */
		mem_free(pMqttConnection->client);
		pMqttConnection->client = NULL;
		//hashmap_free(pMqttConnection->mapSubscribed);
		//pMqttConnection->mapSubscribed = NULL;
	}
}
#endif

