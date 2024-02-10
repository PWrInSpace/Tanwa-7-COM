idf_component_register( SRCS "lora_task.c" "memory_task.c" "user_interface.c" "slave_com.c" "app_init_task.c" "devices_config.c"
                        INCLUDE_DIRS "."
                        REQUIRES cmock driver lora console app utility mcu_config flash_memory sd_card)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format -Wall -W -Werror)