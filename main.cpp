#include <iostream>
#include <string>
#include <vector>
#include "ch.hpp"
#include <main.c>

using namespace chibios_rt;
void initMain(void){
   halInit();
   System::init();
 /*
  * Notes About C++ in Embedded w/ ChibiOS
  * -C++ with STL shall increase the size of your application (bin file) by at least 300 to 400k
  * -Always instantiate instance or classes/objects after you initialize the OS, else it will crash
  * -Add a dummy method void _extern "C" { fini(void) { } }
  * -Add $(CHIBIOS)/os/various/syscalls.c to C source files CSRC
  * -Add main.cpp  to C++ source files CPPSRC
  * -Note that running this in STM32F746 will barely run; almost all RAM3 will be used. I could not find a way of running the threads or BSS in RAM0.
  *  There is something strange happens that crashes the system.
  * -For STM32F7xx, the link file changed
  *      REGION_ALIAS("VARIOUS_FLASH", flash1);
  *    to
  *      REGION_ALIAS("VARIOUS_FLASH", flash0);
  * -Note changes in stm32f769i_discoveryC++.make:
  *     -Added to USE_OPT: -lm -lstdc++
  *     -Added to USE_CPPOPT: -std=gnu++11
  *     -Set USE_LTO = no
  *     -LDSCRIPT was changed to = $(STARTUPLD)/STM32F76xxI-C++.ld
  *     -add to CPPSRC = $(ALLCPPSRC) \
         	 	 	 	   main.cpp
        -used this script to use g++ linker: $(RULESPATH)/arm-none-eabi-C++.mk
     -See folder /os/common/startup/ARMCMx/compilers/GCC/ld for the changed scripts. Pay attention to how
      __exidx_start & __exidx_end sections are being calculated in rules_code-C++..ld.
  */
    std::vector<int> g1;

    for (int i = 1; i <= 5; i++)
         g1.push_back(i);
    std::string *appName = new std::string("MP3Player w/ STM32F7");
    const char * str = appName->c_str();
    bool rc = false;
    try
      {
        throw 20;
      }
      catch (...)
      {
    	  if ( !rc )
    	     rc = true;
      }
}
extern "C" {
__attribute__((used))
void _fini(void) { }
}
