# Copyright (c) 2020 Gerson Fernando Budke
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

zephyr_include_directories_ifdef(CONFIG_SIGFOX inc)

if(CONFIG_SIGFOX)

if(CONFIG_SIGFOX_STACK_SIGFOX_ALLIANCE)
zephyr_sources(
	src/mcu_zephyr.c
	src/radio_zephyr.c
	src/sigfox_alliance.c
	)
zephyr_sources_ifdef(CONFIG_SIGFOX_MONARCH_FEATURE_API
	src/monarch_zephyr.c
	)

if(CONFIG_CPU_CORTEX_M0PLUS)
zephyr_link_libraries(
	${CMAKE_CURRENT_SOURCE_DIR}/libs/SFX_LIB_MON_CM0+_GCC.a)
elseif(CONFIG_CPU_CORTEX_M3)
zephyr_link_libraries(
	${CMAKE_CURRENT_SOURCE_DIR}/libs/SFX_LIB_MON_CM3_GCC.a)
elseif(CONFIG_CPU_CORTEX_M4 OR CONFIG_CPU_CORTEX_M7)
zephyr_link_libraries(
	${CMAKE_CURRENT_SOURCE_DIR}/libs/SFX_LIB_MON_CM4_GCC.a)
else()
message(FATAL_ERROR "This project only works with ARM Cortex-M MCUs")
endif()

if(CONFIG_SIGFOX_SE_FEATURE_API)
if(CONFIG_CPU_CORTEX_M0PLUS)
zephyr_link_libraries(
	${CMAKE_CURRENT_SOURCE_DIR}/libs/ST_RETRIEVER_LIB_CM0+_GCC.a)
elseif(CONFIG_CPU_CORTEX_M3)
zephyr_link_libraries(
	${CMAKE_CURRENT_SOURCE_DIR}/libs/ST_RETRIEVER_LIB_CM3_GCC.a)
elseif(CONFIG_CPU_CORTEX_M4 OR CONFIG_CPU_CORTEX_M7)
zephyr_link_libraries(
	${CMAKE_CURRENT_SOURCE_DIR}/libs/ST_RETRIEVER_LIB_CM4_GCC.a)
endif()
endif()

zephyr_linker_sources_ifdef(CONFIG_SIGFOX RODATA iar.rtmodel.ld)
zephyr_linker_sources_ifdef(CONFIG_SIGFOX RODATA iar.vfe.ld)

endif() # CONFIG_SIGFOX_STACK_SIGFOX_ALLIANCE

if(CONFIG_SIGFOX_STACK_RENARD)
message(FATAL_ERROR "This project doesn't support Sigfox Renard yet!")
endif() # CONFIG_SIGFOX_STACK_RENARD

endif() # CONFIG_SIGFOX