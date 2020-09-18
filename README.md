[![Watch demo](https://img.youtube.com/vi/VAiZSbqR4mY/0.jpg)](https://youtu.be/VAiZSbqR4mY)

- [Brief Description](#Brief-Description)
- [Use Cases (Proof of Concepts)](##use-cases-proof-of-concepts)
- [Architectural Overview](#Architectural-Overview)
- [Supported Boards](#Supported-Boards)
- [Projects Setup](#Projects-Setup)
- [Licencing](#Licencing)
- [About Me](#About-Me)

# Brief Description
**[Struts4Embedded (S4E)](https://github.com/abusous2000/Struts4Embedded)** is a *MVC framework* designed for embedded systems; it mimics Java's **[Struts 1.0](https://en.wikipedia.org/wiki/Apache_Struts_1)** that was popular 18+ years ago. Basically it's a pattern that decouples the model (data structure) from your view (UI); and all are decoupled from the controller (business logic), which often results in projects that are *faster to market, easy to manage, maintain & scale*. For now, S4E is coupled to **[ChibiOS RTOS](https://www.chibios.org/)** only; therefore S4E is independent of any 3rd party libraries (i.e LWIP, MQTT, FATFS,...etc).
 
# Proof of Concepts (Tested Use Cases)
In this release, *three IOT use cases* were provided as a proof of concepts to help developers get started:-
1. The [1st use case](https://github.com/abusous2000/Struts4Embedded) reads POT values and sends them as JSON messages (via MQTT broker) to a Node-Red instance where the data is displayed on the UI Dashboard. Once messages reaches Node-Red, they could be rerouted to AWS IOT, or persisted to data stores (i.e. ElasticSearch, DynamoDB, & SQLLite3 were demonstrated). Commands to the embedded application could be sent from UI Dashboard (or AWS IOT) via JSON messages routed via MQTT broker.
2. the [2nd use case](https://github.com/abusous2000/MP3PlayerUsingSTM32F7) demonstrates controlling and monitoring the state of a MP3 Player from a Node-Red's UI Dashboard; and if *ESP8266 (WiFi Module)* is attached to STM32F769i, it could be controlled as well via RESTful Web Service (web server) hosted on ESP8266. Also a GUI was built to control the player using *uGFX library* (based on V2.7 with slight modification) with ST32F746 & STM32F769i boards. **[Click here](https://youtu.be/VAiZSbqR4mY)** to view a demo.
3. the [3rd use case](https://github.com/abusous2000/Struts4EmbeddedMPU6050) demonstrates reading MPU6050 accelerometer & gyro values and plotting them onto a Node-Red UI Dashboard.

It should be noted; that all three use case:
- were demonstrated using FATFS (w/ SDIO and SPI), ADC; GPT, 5-Button JoyStick, SSD1306 OLED (via I2C), I2S, SAI in a [reusable helper modules](https://github.com/abusous2000/Struts4Embedded/tree/master/source/MQTTClient); that you can [include/exclude](https://github.com/abusous2000/Struts4Embedded/blob/master/cfg/stm32f407_discovery/Strust4EmbeddedConf.h#L27) as you see fit. See *[./source](https://github.com/abusous2000/Struts4Embedded/tree/master/source)* folder for details
- Also demonstrated, how to send S4E wireless control signals (via *[FlySky-i6s](https://github.com/abusous2000/Struts4Embedded/tree/master/source/Controls/PPMFrameDecoder)* RC or vial *[eByte Lora](https://github.com/abusous2000/Struts4Embedded/tree/master/source/Controls/eByteLora)*). 
- easily any of the use cases could be modified/extended to fit other embedded projects. 

# Architectural Overview

![MP3Player with TM32F7 Architectural Overview](https://raw.githubusercontent.com/abusous2000/MP3PlayerUsingSTM32F7/master/docs/STM32F769i-MP3Player.png)

**[Click here](https://github.com/abusous2000/MP3PlayerUsingSTM32F7raw/master/docs/STM32F769i-MP3Player.pdf)** for High Resolution Diagram in PDF

This architectural diagram for the MP3 Player use case with STM32F769i board. Please pay attention to the sequenced steps; and from there you should be able to trace the system & have a good overview.

*Struts4Embedded* framework is based on [actions & events](https://github.com/abusous2000/Struts4Embedded/blob/master/source/Struts4Embedded/Threads/ActionEventsThd.h#L27) that are statically [predefined](https://github.com/abusous2000/Struts4Embedded/blob/master/source/ActionEvents/ActionEvents.c#L157) for each use case (or project). Those actions & events could be [triggered/invoked](https://github.com/abusous2000/Struts4Embedded/blob/master/source/Controls/ButtonsLEDs/ButtonLEDs.c#L17) by sending events to a queue (mailbox), and worker thread(s) [consume and execute](https://github.com/abusous2000/Struts4Embedded/blob/master/source/ActionEvents/ActionEvents.c#L39) those predefined actions. This is very similar to the *[command pattern](https://en.wikipedia.org/wiki/Command_pattern)* where the senders and the consumers are decoupled. For more details, pay attention in *[./source/ActionEvents](https://github.com/abusous2000/Struts4Embedded/tree/master/source/ActionEvents)* folder per each project use case. The core of the architecture is very ease to follow in this file *[./source/Struts4Embedded/Threads/ActionEventsThd.c](https://github.com/abusous2000/Struts4Embedded/blob/master/source/Struts4Embedded/Threads/ActionEventsThd.c)* which is under 250 lines of code

# Supported Boards
The framework was tested using the following development boards: STM32F407 Discovery, [STM32F407VET6 Blackboard](https://os.mbed.com/users/hudakz/code/STM32F407VET6_Hello/shortlog/), [Seeed Arch Max 1.1](https://www.seeedstudio.com/Arch-Max-v1-1-p-2632.html), ST32F746 & STM32F769i discovery boards, STM32F446re-Nucleo, STM32F429zi-Nucleo, & STM32F746zg-Nucleo.

# Projects Setup
- If you don't already have [ChibiOS 20.3.x](https://osdn.net/projects/chibios/releases/72607), please download it first. From now on, where you have expanded it, it will be referred to by *ChibiOS*
- Clone each use case's repository from github into you *ChibiOS/demo/STM32* folder, and then import the make projects into ChibiStudio like any other project. 
- *LWIP* zip file is expected to be expanded in *ChibiOS/ext* folder; same goes for *FATFS*.
- If you decide to use uGFX, then please clone the modified version of [uGFX v2.7](https://github.com/abusous2000/uGFX-2.7) in *ChibiOS/ext* folder; this version has been modified slightly. Note that recent versions were very hard to integrate with ChibiOS.
- In *ChiboOS/* root folder; please clone [ChibiOS-Contrib](https://github.com/ChibiOS/ChibiOS-Contrib); S4E is dependent on it.
- Per each development boards, each has its own [configuration](https://github.com/abusous2000/Struts4Embedded/blob/master/cfg/stm32f769i_discovery/Strust4EmbeddedConf.h) & [make](https://github.com/abusous2000/Struts4Embedded/blob/master/make/stm32f769i_discovery.make) files. A configuration file per board is located into *[./conf/BoardName/Struts4EmbeddedConf.h](https://github.com/abusous2000/Struts4Embedded/blob/master/cfg/stm32f769i_discovery/Strust4EmbeddedConf.h)* where you'll find all specific configuration parameters per board (inclusive of all used pins & drivers). And make files are all grouped in *[/.make](https://github.com/abusous2000/Struts4Embedded/tree/master/make)* folder. Therefore this is a great starting point to understand project dependencies.
- Static IP 10.42.0.10 was chosen running on network 10.42.0.X which you can easily change in *[./conf/lwipopts.h](https://github.com/abusous2000/Struts4Embedded/blob/master/cfg/lwipopts.h#L87)*
- With boards with Arduino headers, I found that using [Waveshare's Accessory Shield](https://www.waveshare.com/accessory-shield.htm) to be very helpful. It has SSS1306 LCD, POT, buzzer, RGB light, RTC clock, 5 button joystick, temperature & accelerometer sensors. I suggest buying one (under $18); it made my life very easy. 
- For development boards without ethernet modules, I used LAN8720; for some reason I wasn't able to make ChibiOS work with DP83848!
- Make sure that you test Node-Red UI Dashboard as the last step. Therefore,  first make sure that your MQTT broker is receiving and sending messages to your application. It should be noted that I tested with Node-Red V1.0.4, since then recent versions were released
- Node-Red Flows can be found in *[./node-red](https://github.com/abusous2000/Struts4Embedded/tree/master/node-red)* folder, and this current deployment is dependent on the following node modules: dashboard, contrib-fs, elasticsearch7, sqlite3, rbe, tail & binance. Therefore please install them before importing the flows.
- *Mosquito MQTT Broker* was used as the primary MQTT broker; other online MQTT brokers (i.e. Dashboard Broker) were tested and they were just fine. Please inspect *[./conf/lwipopts.h](https://github.com/abusous2000/Struts4Embedded/blob/master/cfg/lwipopts.h#L87)* for MQTT broker and network parameters.
- See folder *[./source/ESP8266WiFiModule](https://github.com/abusous2000/Struts4Embedded/tree/master/source/ESP8266WiFiModule)* for the Arduino sketch that runs the Web Service (web Server) on ESP8266. Please change to your WiFi's SSID & password at the top of the [sketch](https://github.com/abusous2000/Struts4Embedded/blob/master/source/ESP8266WiFiModule/ESP8266WiFiModule.ino#L23).
- If you decide to use *[FlySky-i6s](https://github.com/abusous2000/Struts4Embedded/tree/master/source/Controls/PPMFrameDecoder)* to send control signals via RC, please note the following:
   - configure RC output to be PPM & i-Bus in the system config menu & make sure it is already bounded to the receiver. 
   - All you need is to wire 3 pins: ground, 5V, and PPM. 
   - Note that current configuration by default ignores every other 9 frames, which could be [adjusted by changing MAX_FRAMES_TO_COLLECT](https://github.com/abusous2000/Struts4Embedded/blob/master/source/Controls/PPMFrameDecoder/PPMFrameDecoder.h#L67). This is needed because PPM frames are coming fast; & this is needed to reach steady state quickly else flickering states will be reported (especially for buttons).
   - Also note that if use SD-Card; you may have to power the receiver externally and not directly powered from the board's pins (unless the board is powered externally and not from USB).
- If you decide to use *[eByte Lora](https://github.com/abusous2000/Struts4Embedded/tree/master/source/Controls/eByteLora)* to send control signals via *[eByte Lora Modules](https://www.aliexpress.com/i/4000516220084.html)*, please note the following:
   - The library is designed to send a [Lora Frame](https://github.com/abusous2000/Struts4Embedded/blob/master/source/Controls/eByteLora/EByteLora.h#L224), that includes a payload.
   - the payload could be [overridden in the configuration file](https://github.com/abusous2000/Struts4Embedded/blob/master/cfg/stm32f446re_nucleo/Strust4EmbeddedConf.h#L113). By the default the payload send the state of [8 buttons plus the state of one POT value](https://github.com/abusous2000/Struts4Embedded/blob/master/source/Controls/eByteLora/EByteLora.h#L238). This easily could be extended or overridden in the config file.
   - by default the maximum payload size is limited to 40 bytes. Easily you can override this in the config file. Note that eByte limits the total frame size to 512.

- The MP3 Player expects the files to be played to be in folder */music*; which you can override in *main.c*. The audio driver could be found in *[./source/AudioDriver](https://github.com/abusous2000/MP3PlayerUsingSTM32F7/tree/master/source/AudioDriver)*.
- If you test with either STM32F407VET6 Blackboard, Seeed Arch Max 1.1 or STM32F407 Discovery boards, then use the relevant board files provided here in *[./make/make/backup/](https://github.com/abusous2000/Struts4Embedded/tree/master/make/backup)* folder; it contain the board files that were used. 
- Every project was compiled & tested with [C++ & STL Templates](https://github.com/abusous2000/Struts4Embedded/blob/master/main.cpp) as well. However, please note the size of C++ binaries shall increase by at least 300k, and your RAM usage increases as well. The make files, used different links, and if you get a link error, you can copy them from *[./make/make/backup/](https://github.com/abusous2000/Struts4Embedded/tree/master/make/backup)*. I recommend copying all _./Struts4Embedded/make/*.ld_ files to *ChibiOS/os/common/startup/ARMCMx/compilers/GCC/ld*
- all projects have their [ChibiOS debug](https://github.com/abusous2000/Struts4Embedded/blob/master/cfg/stm32f769i_discovery/chconf.h#L517) options turned on; same goes for compilation. Therefore the default make files are not geard for the release versions.

Please note that above setup instructions are the defaults defined in make, header and configuration files. They are not written in stone; therefore if you don't like them please be free to change.

# Licencing
Basically Apache 2.0 has been adopted, therefore please be free to use as you see fit and I shall welcome your critical feedback. I hope this framework will help others in speeding up the development process using a tested & well designed framework. Therefore please be free to use and upgrade as you see fit.

# About Me
Finally, I would like to tell you that prior to 5 to 6 months ago, I had limited exposure to embedded systems or ChibiOS (not even Adruino). However, it was easy for me to pick since I come from a strong architectural background in the Java domain. Frankly, the last time I coded in C/C++ was 20+ years ago! Therefore I'm new to this field, however, ChibiOS really made my job a lot easier. It is a well designed RTOS, and if it wasn't available I was about to do something very similar. The honest truth, it is a well designed, intuitive, very easy to use, well supported & documented, & very easy to reverse engineer.