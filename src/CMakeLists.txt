set(owlib_SRCS onewire.c common.c ds18x20.c uart_wrapper.c)
add_library(owlib OBJECT EXCLUDE_FROM_ALL ${owlib_SRCS})
target_link_libraries(owlib ${LIBOPENCM3})

set(logger_SRCS logger.c)
add_application(logger ${logger_SRCS})
target_link_libraries(logger owlib ${LIBOPENCM3})

set(meter_SRCS main.c)
add_firmware(meter ${meter_SRCS})
target_link_libraries(meter os nosys logger ipc pthread ${LIBOPENCM3})

if (SEMIHOSTING)
	message("Semihosting linking activated")
	target_link_libraries(meter c rdimon)
endif()

