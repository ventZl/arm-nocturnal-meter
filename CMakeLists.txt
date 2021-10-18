cmake_minimum_required(VERSION 3.3)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmrx/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/FirmwareUtils/cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Automatically ignore this build dir in git status view
file(WRITE ${CMAKE_BINARY_DIR}/.gitignore "*")

# Defines list of variables, which if exist, are propagated
# down to any existing subproject added using add_firmware()
set(FIRMWARE_GLOBALS SEMIHOSTING)

include(FirmwareUtils)

if (TESTING)
	# Do not propagate SEMIHOSTING if testing is enabled
	unset(SEMIHOSTING)
endif()

project(nocturnal LANGUAGES NONE)

if (NOT SUBPROJECT)
	add_firmware(meter
		ARCH arm
		TOOLCHAIN gcc
		VARIANT meter
		)
else()
	if (TESTING)
		include(CTest)
		add_definitions(
			-DTESTING=1
			)
	endif()

	enable_language(C)
	enable_language(ASM)

	include(${SUBPROJECT}.cmake)

	# Specific handling of subprojects, which compile bare metal
	# Means embedded device, without operating system
	if (BARE_METAL EQUAL 1)

		if (NOT TESTING)
			include(LinkerConfig)

			include_directories(
				${CMAKE_SOURCE_DIR}/libopencm3/include
			)
		endif()

		include(CMRX)

		include_directories(
			${CMAKE_SOURCE_DIR}/cmrx/include
			${CMAKE_BINARY_DIR}
			)

		if (NOT TESTING)
			add_link_options(-Wl,-Map=helloworld.map --static)
			if (NOT SEMIHOSTING)
				add_link_options(-nostartfiles)
				add_definitions(-fno-common)
				add_link_options(--specs=nano.specs)
				#			add_link_options(--specs=nosys.spec)
			else()
				add_definitions(-DSEMIHOSTING=1)
				add_link_options(--specs=rdimon.specs)
				add_link_options(--specs=nano.specs)
			endif()
			add_link_options(-Wl,--gc-sections)
		endif()

	endif()

	# Specific handling of subprojects, which compile to Linux metal
	# Means embedded device running Linux
	if (HOSTED_METAL EQUAL 1)

	endif()

	# Specific handling of subprojects, which compile to native architecture
	# Stuff like tools
	if (HOSTED_NATIVE EQUAL 1)
	endif()

	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/conf)
	configure_file(${CMAKE_SOURCE_DIR}/cmrx/conf/kernel.h ${CMAKE_BINARY_DIR}/conf/kernel.h)
	link_directories(${CMAKE_BINARY_DIR})

	# Some toolchains apparently require this
	#add_link_options(--specs=nosys.spec)

	#add_definitions(-mcpu=cortex-m0plus)
	if (NOT TESTING)
		add_definitions(
			-fdata-sections
			-ffunction-sections
			)
	endif()

	set(CMAKE_C_FLAGS "--std=gnu99")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ggdb3 -Wall -pedantic -Os")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -ggdb3 -Wall")

	if (TESTING)
		include(tests)
		add_group_target(test)
	endif()
	add_subdirectory(cmrx/src)
	add_subdirectory(src)
	run_event_hooks(CMAKE_DONE)
endif()

