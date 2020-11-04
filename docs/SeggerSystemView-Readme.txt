-Make sure to turn off debugger before running SystemView
-turn off printfs in chconf.h by setting to DEBUG_TRACE_PRINT FALSE, esle UART will be consuming so much of the timeline
-in chconf.h set INCLUDE_SEGGER_JLINK to 1 in 
-Add #include "chHooks.h" in chconf.h post CH_CFG_THREAD_EXTRA_FIELDS (remove what was in included in chHooks.h)
-Target Connection CORTEX-M4
-Target Interface: SWD
-for Interface speed either 4000 or 8000 
-If you get error: "Could not find RTT Control Block. Timeout: 20000 ms", then in search address range choose: 0x20000000 192000
-Plz watch : https://www.youtube.com/watch?v=TH2aCiU9Jyo
             https://www.youtube.com/watch?v=FklzdUt97gE
             https://www.youtube.com/watch?v=1KQ0647NzCo