idf_component_register( SRC_DIRS "."
                        INCLUDE_DIRS "."
                        REQUIRES cmock hardware mcu_config utility state_machine esp_now lora )

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format" "-Wall" "-Werror")