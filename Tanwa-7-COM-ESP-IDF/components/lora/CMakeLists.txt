idf_component_register(SRCS "lora.c"
                    INCLUDE_DIRS "."
                    REQUIRES cmock driver soc esp_rom lora)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)