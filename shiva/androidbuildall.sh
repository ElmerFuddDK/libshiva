#!/bin/bash


# setupenv32/64 must contain the following vars:
#  * ANDROID_NDK_ROOT
#  * ANDROID_SDK_ROOT
#  * Path env var with correct qmake
# eg:
#  export ANDROID_SDK_ROOT=/data/local/android
#  export ANDROID_NDK_ROOT=/data/local/android/ndk-r21d
#  export PATH="$HOME/Qt/5.12.6/android_arm64_v8a/bin:$PATH"

echo -e "\nBuilding 32 bit\n-------------------------"
(
. $HOME/android/setup-env32.sh
exec bash qmakebuildall.sh buildprefix android "$@"
)

echo -e "\nBuilding 64 bit\n-------------------------"
(
. $HOME/android/setup-env64.sh
exec bash qmakebuildall.sh buildprefix android64 "$@"
)

