set remoteaddresssize 64
set remotetimeout 999999
target remote localhost:2000
set download-write-size 2000
set remote memory-write-packet-size 2000
set remote memory-write-packet-size fixed
set remote memory-read-packet-size 2000
set remote memory-read-packet-size fixed
erase
prog /home/timothee/solarclimb/src/Debug/solarclimb.elf
run

