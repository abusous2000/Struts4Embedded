#include "ch.h"
#include "hal.h"
#include "Strust4Embedded.h"
#include "JsonParser.h"
#define JSMN_STATIC
#include "jsmn.h"

static jsmn_parser jsmnParser;
static jsmntok_t   jsmnToken[24];
static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

bool findJsonPayload(char *envelope, char *jsonPL, int size){
  bool foundPayload = false;
  int payloadStartAt = indexOf(envelope,PAYLOAD_PREFIX);
  if ( payloadStartAt >= 0 ){
     int payloadEndsAt = indexOf(envelope,PAYLOAD_SUFFIX);
     if ( payloadEndsAt >= 0 ){
        memset(jsonPL,0,size);
        substring(envelope,jsonPL,payloadStartAt+strlen(PAYLOAD_PREFIX)+1,payloadEndsAt-strlen(PAYLOAD_PREFIX));
        foundPayload = true;
     }
  }
  return foundPayload;
}

ActionEventPayload_Typedef *parseActionEventPayload(char *jsonPayload, ActionEventPayload_Typedef *pAEPayload){
  //dbgprintf("Attempting to parse:%s\n",jsonPayload);
  jsmn_init(&jsmnParser);
  int r = jsmn_parse(&jsmnParser, jsonPayload, strlen(jsonPayload), jsmnToken, sizeof(jsmnToken) / sizeof(jsmnToken[0]));
  if (r < 0) {
    dbgprintf("Failed to parse JSON: %d\n", r);
    return NULL;
  }
  /* Assume the top-level element is an object */
  if (r < 1 || jsmnToken[0].type != JSMN_OBJECT) {
    dbgprintf("Object expected\n");
    return NULL;
  }
  int tCount = jsmn_parse(&jsmnParser, jsonPayload, strlen(jsonPayload), jsmnToken, sizeof(jsmnToken)/sizeof(*jsmnToken));
  for (int i = 0; i != tCount; ++i){
     jsmntok_t *token = jsmnToken + i;
     char * type = 0;
     switch (token->type){
       case JSMN_PRIMITIVE:
         type = "PRIMITIVE";
         break;
       case JSMN_OBJECT:
         type = "OBJECT";
         break;
       case JSMN_ARRAY:
         type = "ARRAY";
         break;
       case JSMN_STRING:
         type = "STRING";
         break;
       default:
         type = "UNDEF";
     }
     (void)type;
     if (jsoneq(jsonPayload, &jsmnToken[i], "actionEvent") == 0) {
         strlcpy(pAEPayload->actionEvent, jsonPayload + jsmnToken[i + 1].start, jsmnToken[i + 1].end - jsmnToken[i + 1].start+1);
         dbgprintf("- actionEvent: %s\n", pAEPayload->actionEvent);
     }
     else
     if (jsoneq(jsonPayload, &jsmnToken[i], "data") == 0) {
         strlcpy(pAEPayload->data, jsonPayload + jsmnToken[i + 1].start, jsmnToken[i + 1].end - jsmnToken[i + 1].start+1);
         dbgprintf("- actionEventData: %s\n", pAEPayload->data);
     }
  }
   return pAEPayload;
}
static const char *JSON_NAME_VALUE_STRING ="{ \"nameValuePair\": [\"maxAEThreads;1\", \"ssid;ROR194\", \"ssidPassword;salah987\"]}";
#define NVP_TOK    "|"
map_t *parseMap(char *jsonPayload,  map_t *mymap){
	  jsmn_init(&jsmnParser);
	  int r = jsmn_parse(&jsmnParser, jsonPayload, strlen(jsonPayload), jsmnToken, sizeof(jsmnToken) / sizeof(jsmnToken[0]));
	  if (r < 0) {
	    dbgprintf("Failed to parse JSON: %d\n", r);
	    return NULL;
	  }
	  /* Assume the top-level element is an object */
	  if (r < 1 || jsmnToken[0].type != JSMN_OBJECT) {
	    dbgprintf("Object expected\n");
	    return NULL;
	  }
	  int tCount = jsmn_parse(&jsmnParser, jsonPayload, strlen(jsonPayload), jsmnToken, sizeof(jsmnToken)/sizeof(*jsmnToken));
	  for (int i = 0; i != tCount; ++i){
	     jsmntok_t *token = jsmnToken + i;
	     char * type = 0;
	     switch (token->type){
	       case JSMN_PRIMITIVE:
	         type = "PRIMITIVE";
	         break;
	       case JSMN_OBJECT:
	         type = "OBJECT";
	         break;
	       case JSMN_ARRAY:
	         type = "ARRAY";
	         break;
	       case JSMN_STRING:
	         type = "STRING";
	         break;
	       default:
	         type = "UNDEF";
	     }
	     (void)type;
	     if (jsoneq(jsonPayload, &jsmnToken[i], "nameValuePair") == 0) {
	    	 char *name =NULL, *value = NULL;

	         if (jsmnToken[i + 1].type != JSMN_ARRAY) {
	           continue; /* We expect groups to be an array of strings */
	         }
	         for (int j = 0; j < jsmnToken[i + 1].size; j++) {
	           char     nameValuePair[60] = {0};
	           char     *context    = NULL;
	           jsmntok_t *g = &jsmnToken[i + j + 2];
		       strlcpy(nameValuePair, jsonPayload + g->start, g->end - g->start+1);

		       name  = strtok_r((char *)nameValuePair,NVP_TOK,&context);
		       value = strtok_r(NULL,NVP_TOK,&context);
		       NameValuePairTypeDef *hashEntry = (NameValuePairTypeDef*)chHeapAlloc(NULL,sizeof(NameValuePairTypeDef));
		       strlcpy(hashEntry->key,name,       KEY_MAX_LENGTH);
		       strlcpy(hashEntry->value,value,    VALUE_MAX_LENGTH);
		       hashmap_put(mymap, hashEntry->key, hashEntry);
	         }
	         i += jsmnToken[i + 1].size + 1;
	     }
	  }

	return mymap;
}
