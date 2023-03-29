#platform linux
HELP_INFO="usage: make PF=PLATFORM		where PLATFORM : {LINUX IMX5 IMX6}"
GOT_PLATFORM=0
ifndef PF
	PF=LINUX
	GOT_PLATFORM=1
endif

#platform LINUX
ifeq ($(PF),LINUX)
	LIB_ROOT=/usr/local/linux_lib
	GOT_PLATFORM=1
	PF_DIR=linux
  CXXFLAGS +=
  LINKFLAGS +=
endif

#platform freescale iMX5
ifeq ($(PF),IMX5)
	CROSS_COMPILE=arm-linux-
	LIB_ROOT=/usr/local/freescale_lib
	GOT_PLATFORM=1
	PF_DIR=freescale
  CXXFLAGS += -O2
  LINKFLAGS +=
endif

#platform freescale imx6
ifeq ($(PF),IMX6)
  CROSS_COMPILE=arm-poky-linux-gnueabi-
  LIB_ROOT=/usr/local/imx6_lib
	GOT_PLATFORM=1
	PF_DIR=
  CXXFLAGS += -O2 -march=armv7-a -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9 --sysroot=/opt/fsl-imx-x11/3.14.52-1.1.1/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/
  LINKFLAGS += -march=armv7-a -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9 --sysroot=/opt/fsl-imx-x11/3.14.52-1.1.1/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/
endif

#没找到对应的平台
ifeq ($(GOT_PLATFORM),0)
	CXX=@echo [error platform $(PF)] g++
	AR=@echo [error platform $(PF)] ar
	STRIP=@echo [error platform $(PF)] strip
endif

CURL_ROOT=$(LIB_ROOT)/curl-7.56.1
CURL_INCLUDE := -I$(CURL_ROOT)/include
CURL_LIB_PATH := $(CURL_ROOT)/lib
CURL_LIBS := $(CURL_LIB_PATH)/libcurl.a

#OTHER_LIB########################################
DYNAMIC_LIBS = -lpthread -lrt -lm

#-Wl,-Bstatic和-Wl,-Bdynamic。这两个选项是gcc的特殊选项，它会将选项的参数传递给链接器，作为链接器的选项。
#-Wl,-Bstatic告诉链接器使用-Bstatic选项，该选项是告诉链接器，对接下来的-l选项使用静态链接；
#-Wl,-Bdynamic就是告诉链接器对接下来的-l选项使用动态链接
COMMON_LIBS = -Wl,-Bstatic $(CURL_LIBS) -Wl,-Bdynamic $(DYNAMIC_LIBS)

COMMON_INCLUDE := 

CFLAGS := 

CXXFLAGS += $(COMMON_INCLUDE) $(CURL_INCLUDE)

CXXFLAGS += -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS

CC = gcc
CXX = g++

SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

TARGET = media_update

$(TARGET):$(OBJS)
	$(CROSS_COMPILE)$(CXX) -o $(TARGET) $(LINKFLAGS) $(OBJS) $(LIBS) $(COMMON_LIBS)
#$(CROSS_COMPILE)strip $(TARGET)

$(OBJS):%.o:%.cpp
	$(CROSS_COMPILE)$(CXX) -c -g $(CXXFLAGS) $< -o $@
	
clean:
	rm -rf  $(TARGET) ./*.o *~
	
	
