/*
 * Strust4Embedded.h
 *
 *  Created on: Dec 12, 2019
 *      Author: abusous2000
 */

#ifndef CFG_STRUST4EMBEDDED_H_
#define CFG_STRUST4EMBEDDED_H_

#include "Strust4EmbeddedConf.h"

#define PAYLOAD_PREFIX          "<pl>"
#define PAYLOAD_SUFFIX          "</pl>"

#include "ch.h"
#include "hal.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dbgtrace.h>
#include "ActionEventsThd.h"
#include "BlinkerThd.h"
#include "WifiCommunicationThd.h"
#include "web.h"
#include "MqttHelper.h"
#include "CPal.h"
#include "CommonHelper.h"
#include "JsonParser.h"
#include "hashmap.h"
#include "emlist.h"
#include "RTCHelper.h"
#include "fault_handlers.h"

#endif /* CFG_STRUST4EMBEDDED_H_ */
