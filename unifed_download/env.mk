HISI3521d=1
__CHR380=1  
#标动与CHR380 FM 设置一到。
ifdef FS
	CXX = arm-linux-g++
	STRIP=arm-linux-strip
else 
	ifdef HISI
		CXX = arm-hismall-linux-g++
		STRIP=arm-hismall-linux-strip
	else ifdef HISI353X
		CXX = arm-hisiv100nptl-linux-g++
		STRIP=arm-hisiv100nptl-linux-strip
	else ifdef HISI3521d
		CXX = arm-hisiv600-linux-g++
		STRIP=arm-hisiv600-linux-strip
	else
		CXX = g++
		CXXFLAGS := -g
		STRIP=echo no strip
	endif
endif

FS_LIB_ROOT=/usr/local/freescale_lib
HISI_LIB_ROOT=/usr/local/hisi_lib
LINUX_LIB_ROOT=/usr/local/linux_lib
HISI353X_LIB_ROOT=/usr/local/hisi3531_mm_s_lib
HISI3521d_LIB_ROOT=/usr/local/hisi3521d_lib


FS_BIN_DIR=./freescale
HISI_BIN_DIR=./hisi
LINUX_BIN_DIR=./linux
HISI353X_BIN_DIR=./hisi353x
HISI3521d_BIN_DIR=./hisi3521d
ifdef FS
	LIB_ROOT=$(FS_LIB_ROOT)
	BIN_DIR=$(FS_BIN_DIR)
else
	ifdef HISI
		LIB_ROOT=$(HISI_LIB_ROOT)
		BIN_DIR=$(HISI_BIN_DIR)
	else ifdef HISI353X
		LIB_ROOT=$(HISI353X_LIB_ROOT)
		BIN_DIR=$(HISI353X_BIN_DIR)
	else ifdef HISI3521d
		LIB_ROOT=$(HISI3521d_LIB_ROOT)
		BIN_DIR=$(HISI3521d_BIN_DIR)
	else
		LIB_ROOT=$(LINUX_LIB_ROOT)
		BIN_DIR=$(LINUX_BIN_DIR)
	endif
endif
	

BOOST_ROOT=$(LIB_ROOT)/boost_1_50_0
FFMPEG_ROOT=$(LIB_ROOT)/ffmpeg-2.6.1-fdkaac
LIVE_ROOT=$(LIB_ROOT)/live555_jx
CURL_ROOT=$(LIB_ROOT)/curl-7.56.1
ICONV_ROOT=$(LIB_ROOT)/libiconv-1.15

BOOST_INCLUDE := $(BOOST_ROOT)/include
LIVE_INCLUDE := -I$(LIVE_ROOT)/include/BasicUsageEnvironment \
				-I$(LIVE_ROOT)/include/groupsock \
				-I$(LIVE_ROOT)/include/liveMedia \
				-I$(LIVE_ROOT)/include/UsageEnvironment

CURL_INCLUDE := -I$(CURL_ROOT)/include
ICONV_INCLUDE := -I$(ICONV_ROOT)/include

#########################################HISI MPP#############################################
#####HISI_SDK_DIR:=/home/haibao/Hi3531_SDK_V1.0.B.0
HISI_SDK_DIR:=/usr/local/hq_sdk/Hi3521D/Hi3521DV100_SDK_V2.0.5.0
HISI_PUB_PATH:=$(HISI_SDK_DIR)/pub
HISI_EXTDRV_PATH:=$(HISI_SDK_DIR)/extdrv/
HISI_MPP_PATH:=$(HISI_SDK_DIR)/mpp
HISI_LIB_PATH:=$(HISI_MPP_PATH)/lib
HISI_COMPONENT_PATH:=$(HISI_MPP_PATH)/lib

HISI_INC_PATH:=-I$(HISI_PUB_PATH)/include \
	-I$(HISI_PUB_PATH)/kbuild-FULL_REL/include \
	-I$(HISI_MPP_PATH)/include \
	-I$(HISI_MPP_PATH)/sample/common \
	-I$(HISI_MPP_PATH)/extdrv/tw2865 \
	-I$(HISI_MPP_PATH)/extdrv/tw2960 \
	-I$(HISI_MPP_PATH)/extdrv/tlv320aic31 \
	-I$(HISI_EXTDRV_PATH)


#头文件目录
INCLUDE_DIR := -I$(MAKEROOT) \
	-I$(BOOST_INCLUDE) \
	-I$(FFMPEG_ROOT)/include \
	$(LIVE_INCLUDE) \
	$(CURL_INCLUDE) \
	$(ICONV_INCLUDE)

ifdef HISI3521d
	INCLUDE_DIR += $(HISI_INC_PATH)
endif

CXXFLAGS := $(CXXFLAGS) $(INCLUDE_DIR) -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS

ifdef HISI3521d
	CXXFLAGS := $(CXXFLAGS) -DHISI
	CFLAGS :=  $(CFLAGS) -DHISI
endif


ifdef __CHR380
	CXXFLAGS := $(CXXFLAGS)  -D__CHR380
        CFLAGS :=  $(CFLAGS) -D__CHR380
endif
#对所有的.o文件以.c文件创建它
%.o : %.cpp
	${CXX} ${CXXFLAGS} -c $< -o $@
