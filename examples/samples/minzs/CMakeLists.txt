project(minzs)

add_executable(minzs
        minzs.c
)
target_compile_definitions(minzs PUBLIC -DSDL_MAIN_HANDLED)
target_link_libraries(minzs BRender::Full dosbase)
set_property(TARGET minzs PROPERTY FOLDER "BRender/Examples/Samples")
