#! /bin/bash
gcc SEND-ARTIK5.c readfile.c -o SEND -I/usr/include/arm-linux-gnueabihf/artik/base -I/usr/include/arm-linux-gnueabihf/artik/bluetooth -lartik-sdk-base
