# Struts4Embedded
[![Watch demo](https://img.youtube.com/vi/peghfjOStV0/0.jpg)](https://www.youtube.com/watch?v=peghfjOStV0)

# Brief Description
Implementation of Model View Controller (MVC) framework/pattern called Struts for embedded environments ([**Struts4Embedded**](https://en.wikipedia.org/wiki/Apache_Struts_1)). This framework mimics a framework that was popular 18 years ago in the Java community called([**Struts**](https://en.wikipedia.org/wiki/Apache_Struts_1)).
As a use case example, an [**MP3 Player**](https://github.com/abusous2000/MP3PlayerUsingSTM32F7) was implement which could be controlled via MQTT messages (via LWIP Ethernet or via WiFi Module ESP8266) or via a simple web interface. Local MQTT broker (Mosquito) was used to send and to receive messages and commands with the MCU; also connectivity to an AWS IOT MQTT was also demonstrated.

# Purpose
- The Embedded community lacks frameworks that could speed-up & ease development. The opposite exists in the Java and JavaScript communities who evolved many useful frameworks (i.e. Spring, Camel, Hibernate, JQuery, ...etc).
- To show how embedded projects could be documented in diagrams (more like UML), sadly the IoT community is way behind on this subject.
- I hope this presentation motivates (or inspires) many to extend this simple reusable framework into an open source project that can energize the open source community.
- This project could easily be reused to control motors/drones or any other use cases, so please feel free to be creative with how to how to use it. More on that later.

# Benefits of Frameworks
IMHO, a solid & well designed frameworks can make projects 
- faster to market,
- agile, 
- moduler, 
- scalable, 
- loosely coupled, 
- adaptive, and most importantly 
- it makes projects easier to debug & maintain.

# Architectural Overview
[![MP3Player with TM32F7 Architectural Overview](https://raw.githubusercontent.com/abusous2000/MP3PlayerUsingSTM32F7/master/MP3PlayerWithSTM32F7ArchitecturalOverview.png)]
[**Click here for High Resolution Diagram in PDF**](https://github.com/abusous2000/MP3PlayerUsingSTM32F7raw/master/STM32F769i-MP3Player.pdf)


# Why Using MVC is Very Useful?
The MOST important reason for implementing MVC in your project that **it forces your application to decouple the presentation layer (view) from its data structure (model) and all decoupled from the business logic**. When MVC not implemented, you will end up with a system that couples all of these layers together, and that makes projects hard to upgrade, maintain & debug. 
Therefore, when MVC is properly implemented, it leads to: 
- Highly modular code which automatically promotes reuse, 
- Programs are more cohesive & the code is logically structured, and 
- Project becomes much easier to maintain, upgrade, & debug.

# Hardware
- [STM32F769i Discovery boards](https://os.mbed.com/platforms/ST-Discovery-F769NI/)
- [Accessories shield](https://os.mbed.com/platforms/ST-Discovery-F769NI/) from WaveShare
- [WiFi Module (ESP8266)](https://www.ebay.com/p/1452674900?iid=351771867415) & its [USB flasher](https://www.ebay.com/i/153666712625)
- [Logic Analyzer](https://www.ebay.com/itm/USB-Logic-Analyzer-24MHZ-8-Channel-12C-UART-SPI-IIC-CAN-1Wire-Debug-For-ARM-FPGA/162980270582) (cheap one will do)
- 2 GB SD Card

# Peripherals & Protocols Used
- ADC (to read volume from POT)
- Timer (to trigger reading ADC)
- SSD1306 OLED
- SAI w/ WM8994 decoder to convert PCM into audio
- I2C used to communicate with control audio decoder
- SPI used to read files from SD Card
- UART for serial communication with WiFi Module
- VCP for debugging
- 5 button JoyStick (read via ChibiOS Port Abstraction Layer PAL).

# Used Patterns 
- Struts 4 Embedded which relies a lot on the command & MVC patterns
- Worker threads blocking on a priority queue (not yet implemented)
- Facade pattern has been implemented between modules for extra decoupling/isolation
- Function Template
- Inter-thread communication with Mailbox pattern, and semaphores
- Pooled memory objected
- Hashmap & linked lists

# Installation
- Setup your environment 1st by installing ChibiStudio, and cloning ChibiOS
- Clone Sturts4Embedded into ChibiOS' demo/STM32 folder
- Configure your ethernet port by changing IP, Gateway & network masks in lwipopts.h 
- Configure & start OpenOCD GDP for your board
- Flash and debug
- Monitor your serial output for debug messages.

# 3rd Party Sub-Systems
- FatFS (open source)
- Madlib used to decode MP3 to PCM
- LWIP & MQTT- AWS IOT- Node-RED

# Evolution and credits
Todo 
# Why ChibiOS was Used?
If you are doing a simple project, you can get away without using RTOS. However, if you have medium to complex project, it is problematic to program and design a project without a reliable RTOS
## Pros
- RTOS has been nicely integrated with HAL
- Small & very easy to reverse engineer
- Solid patterns were implemented: Especially the layering pattern & with clear and predictable interfaces across many peripherals
- prompt response on message boards
- descent documentation & excellent support via message board
- very small footprint and top notch performance

## Cons
- depends on few people
- no multi CPUs support, at least not yet
- Mostly limited to STM32 product line, however, they are adding support to other MCUs. 
- I wish if there is support for ESP32; that will be a game changer
- NO SAI Driver (although there is a driver for I2S)

# Why STM32 HAL Library Wasn't Used?
In my humble opinion, STM32 HAL Library
- Has a large footprint
- Hard to understand (reverse engineer)
- Only lately became stable
- Frankly, I found that ChibiOS is way more stable, well documented & popular

# Lessons learned
- Hard to debug with STM32F7, you need to compile a patch OpenOCD
- Most if not all of the bugs I faced & made me spin my wheels for hours are related to improper peripheral configurations. Therefore, you have to read the data sheets carefully before you code, else you will waste lots of time debugging strange bugs.
- ChibiOS has small circular buffers for UART, I'd to increased them
- Flashing WiFi Module ESP8266 with Arduino Uno was problematic, I believe not enough current was supplied. I had to buy a special flasher
- Especially for Java & C# programmers, when debugger behaves in strange ways, immediately suspect that you have corrupted your memory/stack. 

# To Do List/Issues
- Although this example is coupled with ChibiOS & STM32, easily it could be extended to other RTOSs & other MCUs. Most of the code has been refactored to make it easy to migrate this design to other MCUs and other RTOSs.
- Integrate SSL & AWS IOT directly; where currently it is being re-directed via Node-RED
- Sound contains hissing sounds, might be related to clock
- The communication protocol between the WiFi Module & STM32F7 is based on UART which is unreliable communication unlike TCP/IP. 
- Integrate MicroPython scriptlets if possible, especially for the action methods
- Port to CPP
- Formal documentation