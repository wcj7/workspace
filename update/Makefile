TARGET = update


USE_BOARD:=1


#compile
ifeq ($(USE_BOARD), 1)
SDKTARGETSYSROOT=/opt/fsl-imx-x11/3.14.52-1.1.1/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi
CROSS_COMPILE=arm-poky-linux-gnueabi-
CC=$(CROSS_COMPILE)g++ -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9 --sysroot=$(SDKTARGETSYSROOT)
CXX=$(CROSS_COMPILE)g++ -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9 --sysroot=$(SDKTARGETSYSROOT)
LINK=$(CROSS_COMPILE)g++ -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9 --sysroot=$(SDKTARGETSYSROOT)
else
CROSS_COMPILE=
CC=$(CROSS_COMPILE)g++ 
CXX=$(CROSS_COMPILE)g++ 
LINK=$(CROSS_COMPILE)g++
endif


#lib
ARM_SHARED_LIB = -lm -ldl -lpthread -lstdc++
INC_PATH = -I. -Iinc
ifeq ($(USE_BOARD), 1)
LIB_PATH = -L../libs/imx6
else
LIB_PATH = -L../libs/linux
endif
#ARM_STATIC_LIB = -lSockets -lxml -liconv -lcsvparser

PRE_DEF = -D__LINUX -D__USE_DBG 
#-Wno-error=unused-result -Werror=return-type
CFLAGS += $(INC_PATH) $(PRE_DEF) -Wall -Wno-strict-aliasing -Werror --std=c++11
CXXFLAGS += $(INC_PATH) $(PRE_DEF) -Wall -Wno-strict-aliasing -Werror --std=c++11


C_SOURCES = $(wildcard *.c)
CPP_SOURCES = $(wildcard *.cpp)
C_OBJS = $(patsubst %.c,%.o,$(C_SOURCES))
CPP_OBJS = $(patsubst %.cpp,%.o,$(CPP_SOURCES))
OBJ = $(C_OBJS) $(CPP_OBJS)

all: $(TARGET)
$(TARGET): $(OBJ)
	@echo $(wildcard *.c)
	@echo $(C_SOURCES)
	$(LINK) -o $(TARGET) $(OBJ) -lrt $(INC_PATH) $(LIB_PATH) $(ARM_SHARED_LIB)
	$(CROSS_COMPILE)strip $(TARGET)

clean:
	rm -rf $(OBJ) $(TARGET)
	rm -rf *.o
	rm -rf *.pre
