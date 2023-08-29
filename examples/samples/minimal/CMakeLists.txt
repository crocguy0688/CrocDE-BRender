project(minimal)

add_executable(minimal
        minimal.c
)
target_compile_definitions(minimal PUBLIC -DSDL_MAIN_HANDLED)
target_link_libraries(minimal BRender::Full dosbase)
set_property(TARGET minimal PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../dat")
set_property(TARGET minimal PROPERTY FOLDER "BRender/Examples/Samples")
