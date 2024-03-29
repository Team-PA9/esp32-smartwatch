#Add sources from ui directory
file(GLOB_RECURSE SRC_SQUARELINE_UI ${CMAKE_SOURCE_DIR} "squareline-ui/*.c")
file(GLOB_RECURSE SRC_SENSORS ${CMAKE_SOURCE_DIR} "sensors/*.c")
file(GLOB_RECURSE SRC_UI ${CMAKE_SOURCE_DIR} "ui/*.c")
file(GLOB_RECURSE SRC_BLE ${CMAKE_SOURCE_DIR} "ble/*.c")

idf_component_register(SRCS "main.c" ${SRC_SENSORS} ${SRC_SQUARELINE_UI} ${SRC_UI} ${SRC_BLE}
                    INCLUDE_DIRS "." "squareline-ui" "sensors" "ui" "ble")

