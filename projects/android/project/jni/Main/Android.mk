LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

V_SRC_PATH     := ../../../../../source
V_GPG_PATH     := ../../../../../../GooglePlayGamesSDK/android/include
V_GPG_LIB_PATH := ../../../../../../GooglePlayGamesSDK/android/lib/gnustl
V_ENG_PATH     := ../../../../../../CoreEngine/source
V_ENG_LIB_PATH := ../../../../../../CoreEngine/libraries
V_GJ_PATH      := ../../../../../../GameJoltAPI/source
V_GJ_LIB_PATH  := ../../../../../../GameJoltAPI/libraries

##########################################################################

LOCAL_MODULE := SDL2
LOCAL_SRC_FILES := $(V_ENG_LIB_PATH)/bin/android/$(TARGET_ARCH_ABI)/libSDL2.so
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)

LOCAL_MODULE := SDL2_ttf
LOCAL_SRC_FILES := $(V_ENG_LIB_PATH)/bin/android/$(TARGET_ARCH_ABI)/libSDL2_ttf.so
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)

LOCAL_MODULE := SDL2_image
LOCAL_SRC_FILES := $(V_ENG_LIB_PATH)/bin/android/$(TARGET_ARCH_ABI)/libSDL2_image.so
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)

LOCAL_MODULE := OpenAL
LOCAL_SRC_FILES := $(V_ENG_LIB_PATH)/bin/android/$(TARGET_ARCH_ABI)/libOpenAL.so
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)

LOCAL_MODULE := Ogg
LOCAL_SRC_FILES := $(V_ENG_LIB_PATH)/bin/android/$(TARGET_ARCH_ABI)/libOgg.so
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)

LOCAL_MODULE := Vorbis
LOCAL_SRC_FILES := $(V_ENG_LIB_PATH)/bin/android/$(TARGET_ARCH_ABI)/libVorbis.so
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)

LOCAL_MODULE := VorbisFile
LOCAL_SRC_FILES := $(V_ENG_LIB_PATH)/bin/android/$(TARGET_ARCH_ABI)/libVorbisFile.so
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)

LOCAL_MODULE := libcurl
LOCAL_SRC_FILES := $(V_GJ_LIB_PATH)/bin/android/$(TARGET_ARCH_ABI)/libcurl.so
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)

LOCAL_MODULE := libgpg
LOCAL_SRC_FILES := $(V_GPG_LIB_PATH)/$(TARGET_ARCH_ABI)/libgpg.a
include $(PREBUILT_STATIC_LIBRARY)
include $(CLEAR_VARS)

##########################################################################

LOCAL_MODULE := Main

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(V_ENG_LIB_PATH)/include \
                    $(LOCAL_PATH)/$(V_ENG_PATH)             \
                    $(LOCAL_PATH)/$(V_GJ_LIB_PATH)/include  \
                    $(LOCAL_PATH)/$(V_GJ_PATH)              \
                    $(LOCAL_PATH)/$(V_GPG_PATH)             \
                    $(LOCAL_PATH)/$(V_SRC_PATH)

LOCAL_SRC_FILES := $(V_SRC_PATH)/main.cpp                                \
                   $(V_SRC_PATH)/application.cpp                         \
                   $(V_SRC_PATH)/cBackground.cpp                         \
                   $(V_SRC_PATH)/cBeverage.cpp                           \
                   $(V_SRC_PATH)/cCombatText.cpp                         \
                   $(V_SRC_PATH)/cFirst.cpp                              \
                   $(V_SRC_PATH)/cGame.cpp                               \
                   $(V_SRC_PATH)/cInterface.cpp                          \
                   $(V_SRC_PATH)/cMenu.cpp                               \
                   $(V_SRC_PATH)/cOnline.cpp                             \
                   $(V_SRC_PATH)/cPlate.cpp                              \
                   $(V_SRC_PATH)/cRay.cpp                                \
                   $(V_SRC_PATH)/cRock.cpp                               \
                   $(V_SRC_PATH)/cStage.cpp                              \
                   $(V_SRC_PATH)/cTrap.cpp                               \
                   $(V_ENG_PATH)/Core.cpp                                \
                   $(V_ENG_PATH)/additional/android/main.c               \
                   $(V_ENG_PATH)/additional/coreCPUID.cpp                \
                   $(V_ENG_PATH)/additional/coreGLES.cpp                 \
                   $(V_ENG_PATH)/components/audio/CoreAudio.cpp          \
                   $(V_ENG_PATH)/components/audio/coreMusic.cpp          \
                   $(V_ENG_PATH)/components/audio/coreSound.cpp          \
                   $(V_ENG_PATH)/components/debug/CoreDebug.cpp          \
                   $(V_ENG_PATH)/components/graphics/coreDataBuffer.cpp  \
                   $(V_ENG_PATH)/components/graphics/coreFont.cpp        \
                   $(V_ENG_PATH)/components/graphics/coreFrameBuffer.cpp \
                   $(V_ENG_PATH)/components/graphics/CoreGraphics.cpp    \
                   $(V_ENG_PATH)/components/graphics/coreModel.cpp       \
                   $(V_ENG_PATH)/components/graphics/coreShader.cpp      \
                   $(V_ENG_PATH)/components/graphics/coreSync.cpp        \
                   $(V_ENG_PATH)/components/graphics/coreTexture.cpp     \
                   $(V_ENG_PATH)/components/input/CoreInput.cpp          \
                   $(V_ENG_PATH)/components/system/CoreSystem.cpp        \
                   $(V_ENG_PATH)/components/system/coreThread.cpp        \
                   $(V_ENG_PATH)/components/system/coreTimer.cpp         \
                   $(V_ENG_PATH)/manager/coreMemory.cpp                  \
                   $(V_ENG_PATH)/manager/coreObject.cpp                  \
                   $(V_ENG_PATH)/manager/coreResource.cpp                \
                   $(V_ENG_PATH)/objects/game/coreObject2D.cpp           \
                   $(V_ENG_PATH)/objects/game/coreObject3D.cpp           \
                   $(V_ENG_PATH)/objects/game/coreParticle.cpp           \
                   $(V_ENG_PATH)/objects/menu/coreButton.cpp             \
                   $(V_ENG_PATH)/objects/menu/coreCheckBox.cpp           \
                   $(V_ENG_PATH)/objects/menu/coreLabel.cpp              \
                   $(V_ENG_PATH)/objects/menu/coreMenu.cpp               \
                   $(V_ENG_PATH)/objects/menu/coreTextBox.cpp            \
                   $(V_ENG_PATH)/utilities/data/coreData.cpp             \
                   $(V_ENG_PATH)/utilities/data/coreRand.cpp             \
                   $(V_ENG_PATH)/utilities/file/coreArchive.cpp          \
                   $(V_ENG_PATH)/utilities/file/coreConfig.cpp           \
                   $(V_ENG_PATH)/utilities/file/coreLanguage.cpp         \
                   $(V_ENG_PATH)/utilities/file/coreLog.cpp              \
                   $(V_GJ_PATH)/Base64.cpp                               \
                   $(V_GJ_PATH)/MD5.cpp                                  \
                   ../../src/gjAPI.cpp                                   \
                   $(V_GJ_PATH)/gjDataItem.cpp                           \
                   $(V_GJ_PATH)/gjNetwork.cpp                            \
                   $(V_GJ_PATH)/gjScore.cpp                              \
                   $(V_GJ_PATH)/gjTrophy.cpp                             \
                   $(V_GJ_PATH)/gjUser.cpp                               \
                   $(V_ENG_LIB_PATH)/include/SI/ConvertUTF.c

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_ttf SDL2_image OpenAL Ogg Vorbis VorbisFile libcurl
LOCAL_STATIC_LIBRARIES := libgpg
LOCAL_LDLIBS           := -ldl -llog -latomic -lz -landroid -lGLESv2 -lEGL
LOCAL_CPPFLAGS         := -std=c++14 -O3 -fno-exceptions -frtti

include $(BUILD_SHARED_LIBRARY)