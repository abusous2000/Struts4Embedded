/*
 * FaultHandling.c
 *
 *  Created on: Apr 1, 2020
 *      Author: abusous2000
 */

#include "Strust4EmbeddedConf.h"

#if S4E_USE_FAULT_HANDLER != 0
#include "fault_handlers.h"
//#include "SEGGER_SYSVIEW_ChibiOS.h"

/*PORT_ENABLE_GUARD_PAGES
 * 1- Define FAULT_INFO_HOOK in halconf.h & then put a break point on line 36 in hardfault_handler_v7m.S
 * 2- Step into the following method _fault_info_hook()
 * 3- look for the thread it crashes into plus the memory location; use ChibiOS Debug Eclipse plugin
 * 4- Clone this project
 *    git clone https://github.com/dismirlian/ChibiOS-Contrib.git
 *    and point the this var in the make file to the above folder CHIBIOS_CONTRIB
 * 5- add the following to your Makefile
 *    include $(CHIBIOS_CONTRIB)/os/common/ports/ARMCMx/compilers/GCC/utils/fault_handlers_v7m.mk
 * 6- ALWAYS put break point in _print_message() chSysHalt()... ALWAYS. You catch things early
 *    & in fault_handlers_v7m.c
 * Note the you may have to disassemble the code & to consult the map files to figure out the exact location
 * see this link for details
 * http://www.chibios.com/forum/viewtopic.php?f=3&t=5246
 * see this video too https://www.youtube.com/watch?v=E52v1p6CN5A
 */
extern void _print_message(const char *str);
void _fault_info_hook(const struct fault_info *info) {
	(void)info;
//	 SEGGER_SYSVIEW_PrintfHost("%s\r\n",info->decoded_info_string);
//	 _print_message(info->decoded_info_string);
}

#endif
