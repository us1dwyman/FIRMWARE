#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-Standalone_test.mk)" "nbproject/Makefile-local-Standalone_test.mk"
include nbproject/Makefile-local-Standalone_test.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=Standalone_test
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/TREX_VCM.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/TREX_VCM.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS
SUB_IMAGE_ADDRESS_COMMAND=--image-address $(SUB_IMAGE_ADDRESS)
else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../src/api/timer.c ../src/api/api.c ../src/api/api_can.c ../src/api/api_canmotor.c ../src/api/adc.c ../src/api/api_actuator.c ../src/api/drv_spi16.c ../src/api/ee_mc_25xx.c ../src/api/api_dinput.c ../src/api/api_imu.c ../src/api/lsm6ds3.c ../src/api/mcp2515_mb.c ../src/api/can2.c ../src/KNA/ainputs.c ../src/KNA/battery_charger.c ../src/KNA/brake.c ../src/KNA/can2.c ../src/KNA/deck_actuator.c ../src/KNA/dinputs.c ../src/KNA/DMC.c ../src/KNA/fault.c ../src/KNA/HMI_interface.c ../src/KNA/imu.c ../src/KNA/lockout.c ../src/KNA/nvram.c ../src/KNA/outputs.c ../src/KNA/Qualitiy_testing.c ../src/KNA/Telemetry.c ../src/KNA/test.c ../src/KNA/test_spi.c ../src/KNA/t_rex_operation.c ../src/KNA/micro_actuator.c ../src/KNA/main_power_contactor.c ../src/KNA/main.c ../src/KNA/EEPROM.c ../src/KNA/hour_meters.c ../src/KNA/utility.c mcc_generated_files/reset.c mcc_generated_files/system.c mcc_generated_files/can1.c mcc_generated_files/clock.c mcc_generated_files/interrupt_manager.c mcc_generated_files/mcc.c mcc_generated_files/pin_manager.c mcc_generated_files/traps.c mcc_generated_files/dma.c mcc_generated_files/tmr3.c mcc_generated_files/i2c1.c mcc_generated_files/pwm.c mcc_generated_files/ext_int.c mcc_generated_files/uart2.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/659850784/timer.o ${OBJECTDIR}/_ext/659850784/api.o ${OBJECTDIR}/_ext/659850784/api_can.o ${OBJECTDIR}/_ext/659850784/api_canmotor.o ${OBJECTDIR}/_ext/659850784/adc.o ${OBJECTDIR}/_ext/659850784/api_actuator.o ${OBJECTDIR}/_ext/659850784/drv_spi16.o ${OBJECTDIR}/_ext/659850784/ee_mc_25xx.o ${OBJECTDIR}/_ext/659850784/api_dinput.o ${OBJECTDIR}/_ext/659850784/api_imu.o ${OBJECTDIR}/_ext/659850784/lsm6ds3.o ${OBJECTDIR}/_ext/659850784/mcp2515_mb.o ${OBJECTDIR}/_ext/659850784/can2.o ${OBJECTDIR}/_ext/659828548/ainputs.o ${OBJECTDIR}/_ext/659828548/battery_charger.o ${OBJECTDIR}/_ext/659828548/brake.o ${OBJECTDIR}/_ext/659828548/can2.o ${OBJECTDIR}/_ext/659828548/deck_actuator.o ${OBJECTDIR}/_ext/659828548/dinputs.o ${OBJECTDIR}/_ext/659828548/DMC.o ${OBJECTDIR}/_ext/659828548/fault.o ${OBJECTDIR}/_ext/659828548/HMI_interface.o ${OBJECTDIR}/_ext/659828548/imu.o ${OBJECTDIR}/_ext/659828548/lockout.o ${OBJECTDIR}/_ext/659828548/nvram.o ${OBJECTDIR}/_ext/659828548/outputs.o ${OBJECTDIR}/_ext/659828548/Qualitiy_testing.o ${OBJECTDIR}/_ext/659828548/Telemetry.o ${OBJECTDIR}/_ext/659828548/test.o ${OBJECTDIR}/_ext/659828548/test_spi.o ${OBJECTDIR}/_ext/659828548/t_rex_operation.o ${OBJECTDIR}/_ext/659828548/micro_actuator.o ${OBJECTDIR}/_ext/659828548/main_power_contactor.o ${OBJECTDIR}/_ext/659828548/main.o ${OBJECTDIR}/_ext/659828548/EEPROM.o ${OBJECTDIR}/_ext/659828548/hour_meters.o ${OBJECTDIR}/_ext/659828548/utility.o ${OBJECTDIR}/mcc_generated_files/reset.o ${OBJECTDIR}/mcc_generated_files/system.o ${OBJECTDIR}/mcc_generated_files/can1.o ${OBJECTDIR}/mcc_generated_files/clock.o ${OBJECTDIR}/mcc_generated_files/interrupt_manager.o ${OBJECTDIR}/mcc_generated_files/mcc.o ${OBJECTDIR}/mcc_generated_files/pin_manager.o ${OBJECTDIR}/mcc_generated_files/traps.o ${OBJECTDIR}/mcc_generated_files/dma.o ${OBJECTDIR}/mcc_generated_files/tmr3.o ${OBJECTDIR}/mcc_generated_files/i2c1.o ${OBJECTDIR}/mcc_generated_files/pwm.o ${OBJECTDIR}/mcc_generated_files/ext_int.o ${OBJECTDIR}/mcc_generated_files/uart2.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/659850784/timer.o.d ${OBJECTDIR}/_ext/659850784/api.o.d ${OBJECTDIR}/_ext/659850784/api_can.o.d ${OBJECTDIR}/_ext/659850784/api_canmotor.o.d ${OBJECTDIR}/_ext/659850784/adc.o.d ${OBJECTDIR}/_ext/659850784/api_actuator.o.d ${OBJECTDIR}/_ext/659850784/drv_spi16.o.d ${OBJECTDIR}/_ext/659850784/ee_mc_25xx.o.d ${OBJECTDIR}/_ext/659850784/api_dinput.o.d ${OBJECTDIR}/_ext/659850784/api_imu.o.d ${OBJECTDIR}/_ext/659850784/lsm6ds3.o.d ${OBJECTDIR}/_ext/659850784/mcp2515_mb.o.d ${OBJECTDIR}/_ext/659850784/can2.o.d ${OBJECTDIR}/_ext/659828548/ainputs.o.d ${OBJECTDIR}/_ext/659828548/battery_charger.o.d ${OBJECTDIR}/_ext/659828548/brake.o.d ${OBJECTDIR}/_ext/659828548/can2.o.d ${OBJECTDIR}/_ext/659828548/deck_actuator.o.d ${OBJECTDIR}/_ext/659828548/dinputs.o.d ${OBJECTDIR}/_ext/659828548/DMC.o.d ${OBJECTDIR}/_ext/659828548/fault.o.d ${OBJECTDIR}/_ext/659828548/HMI_interface.o.d ${OBJECTDIR}/_ext/659828548/imu.o.d ${OBJECTDIR}/_ext/659828548/lockout.o.d ${OBJECTDIR}/_ext/659828548/nvram.o.d ${OBJECTDIR}/_ext/659828548/outputs.o.d ${OBJECTDIR}/_ext/659828548/Qualitiy_testing.o.d ${OBJECTDIR}/_ext/659828548/Telemetry.o.d ${OBJECTDIR}/_ext/659828548/test.o.d ${OBJECTDIR}/_ext/659828548/test_spi.o.d ${OBJECTDIR}/_ext/659828548/t_rex_operation.o.d ${OBJECTDIR}/_ext/659828548/micro_actuator.o.d ${OBJECTDIR}/_ext/659828548/main_power_contactor.o.d ${OBJECTDIR}/_ext/659828548/main.o.d ${OBJECTDIR}/_ext/659828548/EEPROM.o.d ${OBJECTDIR}/_ext/659828548/hour_meters.o.d ${OBJECTDIR}/_ext/659828548/utility.o.d ${OBJECTDIR}/mcc_generated_files/reset.o.d ${OBJECTDIR}/mcc_generated_files/system.o.d ${OBJECTDIR}/mcc_generated_files/can1.o.d ${OBJECTDIR}/mcc_generated_files/clock.o.d ${OBJECTDIR}/mcc_generated_files/interrupt_manager.o.d ${OBJECTDIR}/mcc_generated_files/mcc.o.d ${OBJECTDIR}/mcc_generated_files/pin_manager.o.d ${OBJECTDIR}/mcc_generated_files/traps.o.d ${OBJECTDIR}/mcc_generated_files/dma.o.d ${OBJECTDIR}/mcc_generated_files/tmr3.o.d ${OBJECTDIR}/mcc_generated_files/i2c1.o.d ${OBJECTDIR}/mcc_generated_files/pwm.o.d ${OBJECTDIR}/mcc_generated_files/ext_int.o.d ${OBJECTDIR}/mcc_generated_files/uart2.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/659850784/timer.o ${OBJECTDIR}/_ext/659850784/api.o ${OBJECTDIR}/_ext/659850784/api_can.o ${OBJECTDIR}/_ext/659850784/api_canmotor.o ${OBJECTDIR}/_ext/659850784/adc.o ${OBJECTDIR}/_ext/659850784/api_actuator.o ${OBJECTDIR}/_ext/659850784/drv_spi16.o ${OBJECTDIR}/_ext/659850784/ee_mc_25xx.o ${OBJECTDIR}/_ext/659850784/api_dinput.o ${OBJECTDIR}/_ext/659850784/api_imu.o ${OBJECTDIR}/_ext/659850784/lsm6ds3.o ${OBJECTDIR}/_ext/659850784/mcp2515_mb.o ${OBJECTDIR}/_ext/659850784/can2.o ${OBJECTDIR}/_ext/659828548/ainputs.o ${OBJECTDIR}/_ext/659828548/battery_charger.o ${OBJECTDIR}/_ext/659828548/brake.o ${OBJECTDIR}/_ext/659828548/can2.o ${OBJECTDIR}/_ext/659828548/deck_actuator.o ${OBJECTDIR}/_ext/659828548/dinputs.o ${OBJECTDIR}/_ext/659828548/DMC.o ${OBJECTDIR}/_ext/659828548/fault.o ${OBJECTDIR}/_ext/659828548/HMI_interface.o ${OBJECTDIR}/_ext/659828548/imu.o ${OBJECTDIR}/_ext/659828548/lockout.o ${OBJECTDIR}/_ext/659828548/nvram.o ${OBJECTDIR}/_ext/659828548/outputs.o ${OBJECTDIR}/_ext/659828548/Qualitiy_testing.o ${OBJECTDIR}/_ext/659828548/Telemetry.o ${OBJECTDIR}/_ext/659828548/test.o ${OBJECTDIR}/_ext/659828548/test_spi.o ${OBJECTDIR}/_ext/659828548/t_rex_operation.o ${OBJECTDIR}/_ext/659828548/micro_actuator.o ${OBJECTDIR}/_ext/659828548/main_power_contactor.o ${OBJECTDIR}/_ext/659828548/main.o ${OBJECTDIR}/_ext/659828548/EEPROM.o ${OBJECTDIR}/_ext/659828548/hour_meters.o ${OBJECTDIR}/_ext/659828548/utility.o ${OBJECTDIR}/mcc_generated_files/reset.o ${OBJECTDIR}/mcc_generated_files/system.o ${OBJECTDIR}/mcc_generated_files/can1.o ${OBJECTDIR}/mcc_generated_files/clock.o ${OBJECTDIR}/mcc_generated_files/interrupt_manager.o ${OBJECTDIR}/mcc_generated_files/mcc.o ${OBJECTDIR}/mcc_generated_files/pin_manager.o ${OBJECTDIR}/mcc_generated_files/traps.o ${OBJECTDIR}/mcc_generated_files/dma.o ${OBJECTDIR}/mcc_generated_files/tmr3.o ${OBJECTDIR}/mcc_generated_files/i2c1.o ${OBJECTDIR}/mcc_generated_files/pwm.o ${OBJECTDIR}/mcc_generated_files/ext_int.o ${OBJECTDIR}/mcc_generated_files/uart2.o

# Source Files
SOURCEFILES=../src/api/timer.c ../src/api/api.c ../src/api/api_can.c ../src/api/api_canmotor.c ../src/api/adc.c ../src/api/api_actuator.c ../src/api/drv_spi16.c ../src/api/ee_mc_25xx.c ../src/api/api_dinput.c ../src/api/api_imu.c ../src/api/lsm6ds3.c ../src/api/mcp2515_mb.c ../src/api/can2.c ../src/KNA/ainputs.c ../src/KNA/battery_charger.c ../src/KNA/brake.c ../src/KNA/can2.c ../src/KNA/deck_actuator.c ../src/KNA/dinputs.c ../src/KNA/DMC.c ../src/KNA/fault.c ../src/KNA/HMI_interface.c ../src/KNA/imu.c ../src/KNA/lockout.c ../src/KNA/nvram.c ../src/KNA/outputs.c ../src/KNA/Qualitiy_testing.c ../src/KNA/Telemetry.c ../src/KNA/test.c ../src/KNA/test_spi.c ../src/KNA/t_rex_operation.c ../src/KNA/micro_actuator.c ../src/KNA/main_power_contactor.c ../src/KNA/main.c ../src/KNA/EEPROM.c ../src/KNA/hour_meters.c ../src/KNA/utility.c mcc_generated_files/reset.c mcc_generated_files/system.c mcc_generated_files/can1.c mcc_generated_files/clock.c mcc_generated_files/interrupt_manager.c mcc_generated_files/mcc.c mcc_generated_files/pin_manager.c mcc_generated_files/traps.c mcc_generated_files/dma.c mcc_generated_files/tmr3.c mcc_generated_files/i2c1.c mcc_generated_files/pwm.c mcc_generated_files/ext_int.c mcc_generated_files/uart2.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-Standalone_test.mk dist/${CND_CONF}/${IMAGE_TYPE}/TREX_VCM.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33EP128MC506
MP_LINKER_FILE_OPTION=,--script="application.gld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/659850784/timer.o: ../src/api/timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/timer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/timer.c  -o ${OBJECTDIR}/_ext/659850784/timer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/timer.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/timer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/api.o: ../src/api/api.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/api.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/api.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/api.c  -o ${OBJECTDIR}/_ext/659850784/api.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/api.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/api.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/api_can.o: ../src/api/api_can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/api_can.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/api_can.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/api_can.c  -o ${OBJECTDIR}/_ext/659850784/api_can.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/api_can.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/api_can.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/api_canmotor.o: ../src/api/api_canmotor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/api_canmotor.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/api_canmotor.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/api_canmotor.c  -o ${OBJECTDIR}/_ext/659850784/api_canmotor.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/api_canmotor.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/api_canmotor.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/adc.o: ../src/api/adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/adc.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/adc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/adc.c  -o ${OBJECTDIR}/_ext/659850784/adc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/adc.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/adc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/api_actuator.o: ../src/api/api_actuator.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/api_actuator.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/api_actuator.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/api_actuator.c  -o ${OBJECTDIR}/_ext/659850784/api_actuator.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/api_actuator.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/api_actuator.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/drv_spi16.o: ../src/api/drv_spi16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/drv_spi16.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/drv_spi16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/drv_spi16.c  -o ${OBJECTDIR}/_ext/659850784/drv_spi16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/drv_spi16.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/drv_spi16.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/ee_mc_25xx.o: ../src/api/ee_mc_25xx.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/ee_mc_25xx.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/ee_mc_25xx.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/ee_mc_25xx.c  -o ${OBJECTDIR}/_ext/659850784/ee_mc_25xx.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/ee_mc_25xx.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/ee_mc_25xx.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/api_dinput.o: ../src/api/api_dinput.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/api_dinput.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/api_dinput.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/api_dinput.c  -o ${OBJECTDIR}/_ext/659850784/api_dinput.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/api_dinput.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/api_dinput.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/api_imu.o: ../src/api/api_imu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/api_imu.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/api_imu.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/api_imu.c  -o ${OBJECTDIR}/_ext/659850784/api_imu.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/api_imu.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/api_imu.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/lsm6ds3.o: ../src/api/lsm6ds3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/lsm6ds3.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/lsm6ds3.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/lsm6ds3.c  -o ${OBJECTDIR}/_ext/659850784/lsm6ds3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/lsm6ds3.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/lsm6ds3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/mcp2515_mb.o: ../src/api/mcp2515_mb.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/mcp2515_mb.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/mcp2515_mb.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/mcp2515_mb.c  -o ${OBJECTDIR}/_ext/659850784/mcp2515_mb.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/mcp2515_mb.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/mcp2515_mb.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/can2.o: ../src/api/can2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/can2.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/can2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/can2.c  -o ${OBJECTDIR}/_ext/659850784/can2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/can2.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/can2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/ainputs.o: ../src/KNA/ainputs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/ainputs.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/ainputs.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/ainputs.c  -o ${OBJECTDIR}/_ext/659828548/ainputs.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/ainputs.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/ainputs.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/battery_charger.o: ../src/KNA/battery_charger.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/battery_charger.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/battery_charger.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/battery_charger.c  -o ${OBJECTDIR}/_ext/659828548/battery_charger.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/battery_charger.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/battery_charger.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/brake.o: ../src/KNA/brake.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/brake.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/brake.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/brake.c  -o ${OBJECTDIR}/_ext/659828548/brake.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/brake.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/brake.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/can2.o: ../src/KNA/can2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/can2.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/can2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/can2.c  -o ${OBJECTDIR}/_ext/659828548/can2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/can2.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/can2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/deck_actuator.o: ../src/KNA/deck_actuator.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/deck_actuator.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/deck_actuator.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/deck_actuator.c  -o ${OBJECTDIR}/_ext/659828548/deck_actuator.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/deck_actuator.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/deck_actuator.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/dinputs.o: ../src/KNA/dinputs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/dinputs.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/dinputs.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/dinputs.c  -o ${OBJECTDIR}/_ext/659828548/dinputs.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/dinputs.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/dinputs.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/DMC.o: ../src/KNA/DMC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/DMC.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/DMC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/DMC.c  -o ${OBJECTDIR}/_ext/659828548/DMC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/DMC.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/DMC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/fault.o: ../src/KNA/fault.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/fault.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/fault.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/fault.c  -o ${OBJECTDIR}/_ext/659828548/fault.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/fault.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/fault.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/HMI_interface.o: ../src/KNA/HMI_interface.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/HMI_interface.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/HMI_interface.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/HMI_interface.c  -o ${OBJECTDIR}/_ext/659828548/HMI_interface.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/HMI_interface.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/HMI_interface.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/imu.o: ../src/KNA/imu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/imu.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/imu.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/imu.c  -o ${OBJECTDIR}/_ext/659828548/imu.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/imu.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/imu.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/lockout.o: ../src/KNA/lockout.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/lockout.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/lockout.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/lockout.c  -o ${OBJECTDIR}/_ext/659828548/lockout.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/lockout.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/lockout.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/nvram.o: ../src/KNA/nvram.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/nvram.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/nvram.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/nvram.c  -o ${OBJECTDIR}/_ext/659828548/nvram.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/nvram.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/nvram.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/outputs.o: ../src/KNA/outputs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/outputs.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/outputs.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/outputs.c  -o ${OBJECTDIR}/_ext/659828548/outputs.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/outputs.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/outputs.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/Qualitiy_testing.o: ../src/KNA/Qualitiy_testing.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/Qualitiy_testing.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/Qualitiy_testing.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/Qualitiy_testing.c  -o ${OBJECTDIR}/_ext/659828548/Qualitiy_testing.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/Qualitiy_testing.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/Qualitiy_testing.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/Telemetry.o: ../src/KNA/Telemetry.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/Telemetry.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/Telemetry.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/Telemetry.c  -o ${OBJECTDIR}/_ext/659828548/Telemetry.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/Telemetry.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/Telemetry.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/test.o: ../src/KNA/test.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/test.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/test.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/test.c  -o ${OBJECTDIR}/_ext/659828548/test.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/test.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/test.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/test_spi.o: ../src/KNA/test_spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/test_spi.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/test_spi.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/test_spi.c  -o ${OBJECTDIR}/_ext/659828548/test_spi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/test_spi.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/test_spi.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/t_rex_operation.o: ../src/KNA/t_rex_operation.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/t_rex_operation.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/t_rex_operation.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/t_rex_operation.c  -o ${OBJECTDIR}/_ext/659828548/t_rex_operation.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/t_rex_operation.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/t_rex_operation.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/micro_actuator.o: ../src/KNA/micro_actuator.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/micro_actuator.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/micro_actuator.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/micro_actuator.c  -o ${OBJECTDIR}/_ext/659828548/micro_actuator.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/micro_actuator.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/micro_actuator.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/main_power_contactor.o: ../src/KNA/main_power_contactor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/main_power_contactor.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/main_power_contactor.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/main_power_contactor.c  -o ${OBJECTDIR}/_ext/659828548/main_power_contactor.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/main_power_contactor.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/main_power_contactor.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/main.o: ../src/KNA/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/main.c  -o ${OBJECTDIR}/_ext/659828548/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/EEPROM.o: ../src/KNA/EEPROM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/EEPROM.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/EEPROM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/EEPROM.c  -o ${OBJECTDIR}/_ext/659828548/EEPROM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/EEPROM.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/EEPROM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/hour_meters.o: ../src/KNA/hour_meters.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/hour_meters.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/hour_meters.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/hour_meters.c  -o ${OBJECTDIR}/_ext/659828548/hour_meters.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/hour_meters.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/hour_meters.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/utility.o: ../src/KNA/utility.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/utility.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/utility.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/utility.c  -o ${OBJECTDIR}/_ext/659828548/utility.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/utility.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/utility.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/reset.o: mcc_generated_files/reset.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/reset.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/reset.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/reset.c  -o ${OBJECTDIR}/mcc_generated_files/reset.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/reset.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/reset.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/system.o: mcc_generated_files/system.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/system.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/system.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/system.c  -o ${OBJECTDIR}/mcc_generated_files/system.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/system.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/system.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/can1.o: mcc_generated_files/can1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/can1.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/can1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/can1.c  -o ${OBJECTDIR}/mcc_generated_files/can1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/can1.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/can1.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/clock.o: mcc_generated_files/clock.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/clock.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/clock.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/clock.c  -o ${OBJECTDIR}/mcc_generated_files/clock.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/clock.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/clock.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/interrupt_manager.o: mcc_generated_files/interrupt_manager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/interrupt_manager.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/interrupt_manager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/interrupt_manager.c  -o ${OBJECTDIR}/mcc_generated_files/interrupt_manager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/interrupt_manager.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/interrupt_manager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/mcc.o: mcc_generated_files/mcc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/mcc.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/mcc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/mcc.c  -o ${OBJECTDIR}/mcc_generated_files/mcc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/mcc.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/mcc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/pin_manager.o: mcc_generated_files/pin_manager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/pin_manager.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/pin_manager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/pin_manager.c  -o ${OBJECTDIR}/mcc_generated_files/pin_manager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/pin_manager.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/pin_manager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/traps.o: mcc_generated_files/traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/traps.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/traps.c  -o ${OBJECTDIR}/mcc_generated_files/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/traps.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/traps.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/dma.o: mcc_generated_files/dma.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/dma.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/dma.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/dma.c  -o ${OBJECTDIR}/mcc_generated_files/dma.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/dma.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/dma.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/tmr3.o: mcc_generated_files/tmr3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/tmr3.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/tmr3.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/tmr3.c  -o ${OBJECTDIR}/mcc_generated_files/tmr3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/tmr3.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/tmr3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/i2c1.o: mcc_generated_files/i2c1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/i2c1.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/i2c1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/i2c1.c  -o ${OBJECTDIR}/mcc_generated_files/i2c1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/i2c1.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/i2c1.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/pwm.o: mcc_generated_files/pwm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/pwm.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/pwm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/pwm.c  -o ${OBJECTDIR}/mcc_generated_files/pwm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/pwm.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/pwm.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/ext_int.o: mcc_generated_files/ext_int.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/ext_int.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/ext_int.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/ext_int.c  -o ${OBJECTDIR}/mcc_generated_files/ext_int.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/ext_int.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/ext_int.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/uart2.o: mcc_generated_files/uart2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/uart2.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/uart2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/uart2.c  -o ${OBJECTDIR}/mcc_generated_files/uart2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/uart2.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/uart2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/659850784/timer.o: ../src/api/timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/timer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/timer.c  -o ${OBJECTDIR}/_ext/659850784/timer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/timer.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/timer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/api.o: ../src/api/api.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/api.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/api.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/api.c  -o ${OBJECTDIR}/_ext/659850784/api.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/api.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/api.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/api_can.o: ../src/api/api_can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/api_can.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/api_can.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/api_can.c  -o ${OBJECTDIR}/_ext/659850784/api_can.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/api_can.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/api_can.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/api_canmotor.o: ../src/api/api_canmotor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/api_canmotor.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/api_canmotor.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/api_canmotor.c  -o ${OBJECTDIR}/_ext/659850784/api_canmotor.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/api_canmotor.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/api_canmotor.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/adc.o: ../src/api/adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/adc.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/adc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/adc.c  -o ${OBJECTDIR}/_ext/659850784/adc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/adc.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/adc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/api_actuator.o: ../src/api/api_actuator.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/api_actuator.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/api_actuator.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/api_actuator.c  -o ${OBJECTDIR}/_ext/659850784/api_actuator.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/api_actuator.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/api_actuator.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/drv_spi16.o: ../src/api/drv_spi16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/drv_spi16.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/drv_spi16.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/drv_spi16.c  -o ${OBJECTDIR}/_ext/659850784/drv_spi16.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/drv_spi16.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/drv_spi16.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/ee_mc_25xx.o: ../src/api/ee_mc_25xx.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/ee_mc_25xx.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/ee_mc_25xx.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/ee_mc_25xx.c  -o ${OBJECTDIR}/_ext/659850784/ee_mc_25xx.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/ee_mc_25xx.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/ee_mc_25xx.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/api_dinput.o: ../src/api/api_dinput.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/api_dinput.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/api_dinput.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/api_dinput.c  -o ${OBJECTDIR}/_ext/659850784/api_dinput.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/api_dinput.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/api_dinput.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/api_imu.o: ../src/api/api_imu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/api_imu.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/api_imu.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/api_imu.c  -o ${OBJECTDIR}/_ext/659850784/api_imu.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/api_imu.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/api_imu.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/lsm6ds3.o: ../src/api/lsm6ds3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/lsm6ds3.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/lsm6ds3.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/lsm6ds3.c  -o ${OBJECTDIR}/_ext/659850784/lsm6ds3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/lsm6ds3.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/lsm6ds3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/mcp2515_mb.o: ../src/api/mcp2515_mb.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/mcp2515_mb.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/mcp2515_mb.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/mcp2515_mb.c  -o ${OBJECTDIR}/_ext/659850784/mcp2515_mb.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/mcp2515_mb.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/mcp2515_mb.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659850784/can2.o: ../src/api/can2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659850784" 
	@${RM} ${OBJECTDIR}/_ext/659850784/can2.o.d 
	@${RM} ${OBJECTDIR}/_ext/659850784/can2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/api/can2.c  -o ${OBJECTDIR}/_ext/659850784/can2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659850784/can2.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659850784/can2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/ainputs.o: ../src/KNA/ainputs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/ainputs.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/ainputs.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/ainputs.c  -o ${OBJECTDIR}/_ext/659828548/ainputs.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/ainputs.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/ainputs.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/battery_charger.o: ../src/KNA/battery_charger.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/battery_charger.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/battery_charger.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/battery_charger.c  -o ${OBJECTDIR}/_ext/659828548/battery_charger.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/battery_charger.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/battery_charger.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/brake.o: ../src/KNA/brake.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/brake.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/brake.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/brake.c  -o ${OBJECTDIR}/_ext/659828548/brake.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/brake.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/brake.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/can2.o: ../src/KNA/can2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/can2.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/can2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/can2.c  -o ${OBJECTDIR}/_ext/659828548/can2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/can2.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/can2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/deck_actuator.o: ../src/KNA/deck_actuator.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/deck_actuator.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/deck_actuator.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/deck_actuator.c  -o ${OBJECTDIR}/_ext/659828548/deck_actuator.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/deck_actuator.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/deck_actuator.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/dinputs.o: ../src/KNA/dinputs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/dinputs.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/dinputs.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/dinputs.c  -o ${OBJECTDIR}/_ext/659828548/dinputs.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/dinputs.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/dinputs.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/DMC.o: ../src/KNA/DMC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/DMC.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/DMC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/DMC.c  -o ${OBJECTDIR}/_ext/659828548/DMC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/DMC.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/DMC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/fault.o: ../src/KNA/fault.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/fault.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/fault.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/fault.c  -o ${OBJECTDIR}/_ext/659828548/fault.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/fault.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/fault.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/HMI_interface.o: ../src/KNA/HMI_interface.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/HMI_interface.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/HMI_interface.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/HMI_interface.c  -o ${OBJECTDIR}/_ext/659828548/HMI_interface.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/HMI_interface.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/HMI_interface.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/imu.o: ../src/KNA/imu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/imu.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/imu.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/imu.c  -o ${OBJECTDIR}/_ext/659828548/imu.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/imu.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/imu.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/lockout.o: ../src/KNA/lockout.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/lockout.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/lockout.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/lockout.c  -o ${OBJECTDIR}/_ext/659828548/lockout.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/lockout.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/lockout.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/nvram.o: ../src/KNA/nvram.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/nvram.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/nvram.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/nvram.c  -o ${OBJECTDIR}/_ext/659828548/nvram.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/nvram.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/nvram.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/outputs.o: ../src/KNA/outputs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/outputs.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/outputs.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/outputs.c  -o ${OBJECTDIR}/_ext/659828548/outputs.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/outputs.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/outputs.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/Qualitiy_testing.o: ../src/KNA/Qualitiy_testing.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/Qualitiy_testing.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/Qualitiy_testing.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/Qualitiy_testing.c  -o ${OBJECTDIR}/_ext/659828548/Qualitiy_testing.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/Qualitiy_testing.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/Qualitiy_testing.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/Telemetry.o: ../src/KNA/Telemetry.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/Telemetry.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/Telemetry.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/Telemetry.c  -o ${OBJECTDIR}/_ext/659828548/Telemetry.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/Telemetry.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/Telemetry.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/test.o: ../src/KNA/test.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/test.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/test.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/test.c  -o ${OBJECTDIR}/_ext/659828548/test.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/test.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/test.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/test_spi.o: ../src/KNA/test_spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/test_spi.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/test_spi.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/test_spi.c  -o ${OBJECTDIR}/_ext/659828548/test_spi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/test_spi.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/test_spi.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/t_rex_operation.o: ../src/KNA/t_rex_operation.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/t_rex_operation.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/t_rex_operation.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/t_rex_operation.c  -o ${OBJECTDIR}/_ext/659828548/t_rex_operation.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/t_rex_operation.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/t_rex_operation.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/micro_actuator.o: ../src/KNA/micro_actuator.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/micro_actuator.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/micro_actuator.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/micro_actuator.c  -o ${OBJECTDIR}/_ext/659828548/micro_actuator.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/micro_actuator.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/micro_actuator.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/main_power_contactor.o: ../src/KNA/main_power_contactor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/main_power_contactor.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/main_power_contactor.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/main_power_contactor.c  -o ${OBJECTDIR}/_ext/659828548/main_power_contactor.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/main_power_contactor.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/main_power_contactor.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/main.o: ../src/KNA/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/main.c  -o ${OBJECTDIR}/_ext/659828548/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/main.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/EEPROM.o: ../src/KNA/EEPROM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/EEPROM.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/EEPROM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/EEPROM.c  -o ${OBJECTDIR}/_ext/659828548/EEPROM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/EEPROM.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/EEPROM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/hour_meters.o: ../src/KNA/hour_meters.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/hour_meters.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/hour_meters.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/hour_meters.c  -o ${OBJECTDIR}/_ext/659828548/hour_meters.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/hour_meters.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/hour_meters.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659828548/utility.o: ../src/KNA/utility.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659828548" 
	@${RM} ${OBJECTDIR}/_ext/659828548/utility.o.d 
	@${RM} ${OBJECTDIR}/_ext/659828548/utility.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/KNA/utility.c  -o ${OBJECTDIR}/_ext/659828548/utility.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659828548/utility.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/_ext/659828548/utility.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/reset.o: mcc_generated_files/reset.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/reset.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/reset.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/reset.c  -o ${OBJECTDIR}/mcc_generated_files/reset.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/reset.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/reset.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/system.o: mcc_generated_files/system.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/system.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/system.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/system.c  -o ${OBJECTDIR}/mcc_generated_files/system.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/system.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/system.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/can1.o: mcc_generated_files/can1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/can1.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/can1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/can1.c  -o ${OBJECTDIR}/mcc_generated_files/can1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/can1.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/can1.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/clock.o: mcc_generated_files/clock.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/clock.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/clock.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/clock.c  -o ${OBJECTDIR}/mcc_generated_files/clock.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/clock.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/clock.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/interrupt_manager.o: mcc_generated_files/interrupt_manager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/interrupt_manager.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/interrupt_manager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/interrupt_manager.c  -o ${OBJECTDIR}/mcc_generated_files/interrupt_manager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/interrupt_manager.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/interrupt_manager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/mcc.o: mcc_generated_files/mcc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/mcc.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/mcc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/mcc.c  -o ${OBJECTDIR}/mcc_generated_files/mcc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/mcc.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/mcc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/pin_manager.o: mcc_generated_files/pin_manager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/pin_manager.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/pin_manager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/pin_manager.c  -o ${OBJECTDIR}/mcc_generated_files/pin_manager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/pin_manager.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/pin_manager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/traps.o: mcc_generated_files/traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/traps.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/traps.c  -o ${OBJECTDIR}/mcc_generated_files/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/traps.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/traps.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/dma.o: mcc_generated_files/dma.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/dma.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/dma.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/dma.c  -o ${OBJECTDIR}/mcc_generated_files/dma.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/dma.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/dma.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/tmr3.o: mcc_generated_files/tmr3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/tmr3.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/tmr3.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/tmr3.c  -o ${OBJECTDIR}/mcc_generated_files/tmr3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/tmr3.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/tmr3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/i2c1.o: mcc_generated_files/i2c1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/i2c1.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/i2c1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/i2c1.c  -o ${OBJECTDIR}/mcc_generated_files/i2c1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/i2c1.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/i2c1.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/pwm.o: mcc_generated_files/pwm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/pwm.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/pwm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/pwm.c  -o ${OBJECTDIR}/mcc_generated_files/pwm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/pwm.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/pwm.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/ext_int.o: mcc_generated_files/ext_int.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/ext_int.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/ext_int.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/ext_int.c  -o ${OBJECTDIR}/mcc_generated_files/ext_int.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/ext_int.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/ext_int.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mcc_generated_files/uart2.o: mcc_generated_files/uart2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/mcc_generated_files" 
	@${RM} ${OBJECTDIR}/mcc_generated_files/uart2.o.d 
	@${RM} ${OBJECTDIR}/mcc_generated_files/uart2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mcc_generated_files/uart2.c  -o ${OBJECTDIR}/mcc_generated_files/uart2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mcc_generated_files/uart2.o.d"      -mno-eds-warn  -g -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -O0 -I"../src/api" -I"../src/apitest" -I"../src" -I"." -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/mcc_generated_files/uart2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/TREX_VCM.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    application.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/TREX_VCM.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG=__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)   -mreserve=data@0x1000:0x101B -mreserve=data@0x101C:0x101D -mreserve=data@0x101E:0x101F -mreserve=data@0x1020:0x1021 -mreserve=data@0x1022:0x1023 -mreserve=data@0x1024:0x1027 -mreserve=data@0x1028:0x104F   -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D__DEBUG=__DEBUG,--defsym=__MPLAB_DEBUGGER_ICD4=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp=${DFP_DIR}/xc16 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/TREX_VCM.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   application.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/TREX_VCM.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DRUN_TESTS -DXPRJ_Standalone_test=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp=${DFP_DIR}/xc16 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/TREX_VCM.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf   -mdfp=${DFP_DIR}/xc16 
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Standalone_test
	${RM} -r dist/Standalone_test

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
