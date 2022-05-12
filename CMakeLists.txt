file(GLOB_RECURSE SRC_DIR_LIST "Src/*.*")

#添加源文件到工程, 将DIR_LIB_SRCS中文件列表以静态库的方式加入工程
target_sources(${PROJECT_NAME} PUBLIC
        ${SRC_DIR_LIST}										#将DIR_LIB_SRCS中文件列表以静态库的方式加入工程
        )

#添加头文件到工程
target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Inc
        )

# 编译后打印占用 ROM, RAM, 仅RP2040
#target_link_options(pico_standard_link INTERFACE "LINKER:--print-memory-usage")

# 使能外部库
SET(ds18b20_en 0)       # ds18b20
SET(easyflash_en 0)     # easyflash

if (ds18b20_en)
    #添加子目录
    add_subdirectory(ds18b20)
    #使能该宏启用 ds18b20
    target_compile_definitions(${PROJECT_NAME} PUBLIC
            -DICE_DS18B20 # ds18b20
            )
endif (ds18b20_en)

if (easyflash_en)
    #添加子目录
    add_subdirectory(easyflash)
    #使能该宏启用easyflash
    target_compile_definitions(${PROJECT_NAME} PUBLIC
            -DICE_EASYFLASH # easyflash
            )
endif (easyflash_en)








