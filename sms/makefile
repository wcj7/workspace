VPATH := ./tinyxml-2.5.3


INC=-I./tinyxml-2.5.3 -I.
CC=
CPP= $(wildcard ./*.cpp)
CPP +=  tinystr.cpp tinyxml.cpp tinyxmlerror.cpp tinyxmlparser.cpp
#CPP += main.cpp commonLocal.cpp xmlServer.cpp netService.cpp log.cpp
USE_BOARD:=1
#OBJ= $(subst cpp,o ,$(notdir $(foreach n,$(CPP), $(n))))
ifeq ($(USE_BOARD), 1)
SDKTARGETSYSROOT=/opt/fsl-imx-x11/3.14.52-1.1.1/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi
CROSS_COMPILE=arm-poky-linux-gnueabi-
CC=$(CROSS_COMPILE)g++ -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9 --sysroot=$(SDKTARGETSYSROOT)
else
CC=g++
endif

OBJ= $(subst cpp,o, $(foreach n,$(CPP), $(n)))
sms:$(OBJ)
	CC -o sms   $^ 
	strip sms
$(OBJ):$(CPP)
	CC -c  $^  $(INC)  -std=c++11    #-g -DDEBUG 

clean:
	rm -f *.o  sms