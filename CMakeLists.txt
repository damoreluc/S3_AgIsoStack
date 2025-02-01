# https://github.com/espressif/esp-idf/issues/10568
cmake_minimum_required(VERSION 3.16.0)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(EXCLUDE_COMPONENTS esp_lcd)
project(S3_AgIsoStack)
