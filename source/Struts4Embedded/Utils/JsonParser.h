#ifndef JsonParser_h
#define JsonParser_h
#include "ActionEvents.h"
#include "hashmap.h"

#ifdef __cplusplus
 extern "C" {
#endif

ActionEventPayload_Typedef *parseActionEventPayload(char *jsonPayload, ActionEventPayload_Typedef *pAEPayload);
map_t *parseMap(char *jsonPayload,  map_t *mymap);

#ifdef __cplusplus
}
#endif
#endif
