project(sheet)

add_executable(sheet
        sheet.c
)
target_compile_definitions(sheet PUBLIC -DSDL_MAIN_HANDLED)
target_link_libraries(sheet BRender::Full dosbase)
set_property(TARGET sheet PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dat")
set_property(TARGET sheet PROPERTY FOLDER "BRender/Examples/Samples")
