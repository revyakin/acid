#target remote | openocd -f board/stm32vldiscovery.cfg --pipe
#monitor reset init
#load
#s

shell st-util -p 3333 -1 >/dev/null 2>&1 &
target remote :3333 
monitor reset halt

define reload
    monitor reset
    load
    monitor reset
end
