#Add sources from ui directory
file(GLOB_RECURSE SRC_SQUARELINE_UI ${CMAKE_SOURCE_DIR} "squareline-ui/*.c")
file(GLOB_RECURSE SRC_SENSORS ${CMAKE_SOURCE_DIR} "sensors/*.c")
file(GLOB_RECURSE SRC_UI ${CMAKE_SOURCE_DIR} "ui/*.c")
file(GLOB_RECURSE SRC_NTP ${CMAKE_SOURCE_DIR} "ntp/*.c")
file(GLOB_RECURSE SRC_RTC ${CMAKE_SOURCE_DIR} "rtc/*.c")
file(GLOB_RECURSE SRC_WIFI ${CMAKE_SOURCE_DIR} "wifi/*.c")
file(GLOB_RECURSE SRC_BLE ${CMAKE_SOURCE_DIR} "ble/*.c")

idf_component_register(SRCS "main.c" ${SRC_SQUARELINE_UI} ${SRC_SENSORS}  ${SRC_UI} ${SRC_NTP} ${SRC_RTC} ${SRC_WIFI} ${SRC_BLE}
                    INCLUDE_DIRS "." "squareline-ui" "sensors" "ui" "ntp" "rtc" "wifi" "ble")
