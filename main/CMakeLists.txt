#Add sources from ui directory
file(GLOB_RECURSE SRC_UI ${CMAKE_SOURCE_DIR} "ui/*.c")
file(GLOB_RECURSE SRC_SENSORS ${CMAKE_SOURCE_DIR} "sensors/*.c")

idf_component_register(SRCS "main.c" ${SRC_SENSORS} ${SRC_UI}
                    INCLUDE_DIRS "." "ui" "sensors")
