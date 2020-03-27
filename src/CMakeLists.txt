# add sources here
set(src_list 
    main.cc
    hw/can.cc
    hw/nmt.cc
    hw/maxon.cc
    hw/robot.cc
    freemodbus_tcp/freemodbus_tcp.c
    openamp/echo.c
    delay/delay.cc
    )

set(elf_file ${project_name})

set(INC_DIR hw
            delay
            openamp
            freemodbus_tcp
            )

# add library
include_directories(${INC_DIR})

#add sources to executable
add_executable(${elf_file} ${src_list})

# link freemodbus library to project
target_link_libraries(${elf_file} PUBLIC freemodbus_lib)

# add pthread support
find_package(Threads)
target_link_libraries(${elf_file} PUBLIC ${CMAKE_THREAD_LIBS_INIT})

