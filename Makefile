##############################################################################
# Multi-project makefile rules
#

all:
#	@echo === Building for seeed_arch_max ===============================
#	+@make --no-print-directory -f make/seeed_arch_max.make all
#	+@make --no-print-directory -f make/seeed_arch_max-C++.make all	
#	@echo === Building for stm32f407_blackboard ===============================
#	+@make --no-print-directory -f make/stm32f407_blackboard.make all
#	+@make --no-print-directory -f make/stm32f407_blackboard-C++.make all	
#	@echo === Building for stm32f407_discovery ===============================
#	+@make --no-print-directory -f make/stm32f407_discovery.make all
#	+@make --no-print-directory -f make/stm32f407_discovery-C++.make all	
#	@echo === Building for stm32f429i_discovery ===============================
#	+@make --no-print-directory -f make/stm32f429i_discovery.make all
#	+@make --no-print-directory -f make/stm32f429i_discovery-C++.make all	
	@echo === Building for stm32f429zi-nucleo ===============================
	+@make --no-print-directory -f make/stm32f429zi_nucleo.make all
	+@make --no-print-directory -f make/stm32f429zi_nucleo-C++.make all	
	@echo === Building for stm32f446re-nucleo ===============================
	+@make --no-print-directory -f make/stm32f446re_nucleo.make all
#	+@make --no-print-directory -f make/stm32f446re_nucleo-C++.make all	
#	@echo === Building for STM32F746-Discovery ===============================
#	+@make --no-print-directory -f make/stm32f746_discovery-C++.make all
#	+@make --no-print-directory -f make/stm32f746_discovery.make all
#	@echo === Building for STM32F746zg-nucleo ===============================
#	+@make --no-print-directory -f make/stm32f746zg_nucleo-C++.make all
#	+@make --no-print-directory -f make/stm32f746zg_nucleo.make all
#	@echo === Building for STM32F769i-Discovery ===============================	
#	+@make --no-print-directory -f make/stm32f769i_discovery-C++.make all	
#	+@make --no-print-directory -f make/stm32f769i_discovery.make all	
	@echo ====================================================================
	@echo

clean:
	@echo
	+@make --no-print-directory -f make/seeed_arch_max.make clean		
	+@make --no-print-directory -f make/stm32f407_blackboard.make clean		
	+@make --no-print-directory -f make/stm32f407_discovery.make clean		
	+@make --no-print-directory -f make/stm32f429i_discovery.make clean		
	+@make --no-print-directory -f make/stm32f429zi_nucleo.make clean	
	+@make --no-print-directory -f make/stm32f446re_nucleo.make clean	
	+@make --no-print-directory -f make/stm32f746_discovery.make clean
	+@make --no-print-directory -f make/stm32f746zg_nucleo.make clean
	+@make --no-print-directory -f make/stm32f769i_discovery.make clean
	@echo

#
##############################################################################
