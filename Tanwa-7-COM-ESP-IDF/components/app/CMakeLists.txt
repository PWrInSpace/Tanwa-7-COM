idf_component_register( SRC_DIRS "."
                        INCLUDE_DIRS "."
                        REQUIRES cmock device_config cli data esp_now proto commands timers utility)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format" "-Wall" "-Werror")