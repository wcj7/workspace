test.bin:main.o
	arm-poky-linux-gnueabi-g++ -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9 --sysroot=/opt/fsl-imx-x11/3.14.52-1.1.1/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi -o test.bin main.o -lm -ldl -lpthread -lstdc++
main.o: main.cpp
	arm-poky-linux-gnueabi-g++ -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9 --sysroot=/opt/fsl-imx-x11/3.14.52-1.1.1/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi  -O2 -pipe -g -feliminate-unused-debug-types -I. -Iinc -D__LINUX -D__USE_DBG  -Wall -Wno-strict-aliasing -Werror --std=c++11   -c -o main.o main.cpp

clean:
	\rm *.o *.bin
