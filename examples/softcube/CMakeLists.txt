project(softcube)

add_executable(softcube8 main8.c)
target_compile_definitions(softcube8 PUBLIC)
target_link_libraries(softcube8 BRender::Full BRender::DDI)
set_property(TARGET softcube8 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../dat")
set_property(TARGET softcube8 PROPERTY FOLDER "BRender/Examples")

add_executable(softcube16 main.c)
target_compile_definitions(softcube16 PUBLIC -DSDL_MAIN_HANDLED -DSOFTCUBE_16BIT)
target_link_libraries(softcube16 BRender::Full BRender::DDI)
set_property(TARGET softcube16 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../dat")
set_property(TARGET softcube16 PROPERTY FOLDER "BRender/Examples")

add_executable(softcube24 main.c)
target_compile_definitions(softcube24 PUBLIC -DSDL_MAIN_HANDLED)
target_link_libraries(softcube24 BRender::Full BRender::DDI)
set_property(TARGET softcube24 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../dat")
set_property(TARGET softcube24 PROPERTY FOLDER "BRender/Examples")