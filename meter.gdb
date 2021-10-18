target extended-remote | openocd -f openocd.cfg -c "gdb_port pipe; log_output ./build/openocd.log"
monitor reset halt
monitor arm semihosting enable
file ./build/meter/src/meter.elf
