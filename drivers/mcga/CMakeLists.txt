project(mcga)

set(INC_FILES
        include/mcga.h
        include/mcga_p.h
        )

add_library(mcga-headers INTERFACE)
target_sources(mcga-headers INTERFACE
        FILE_SET include
        TYPE HEADERS
        BASE_DIRS include
        FILES ${INC_FILES}
        )
target_include_directories(mcga-headers INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/brender/mcga>
        )

set_target_properties(mcga-headers PROPERTIES
        EXPORT_NAME "Drivers::MCGA::Headers"
        FOLDER "BRender/Drivers"
        )


add_library(mcga
        driver.c
        )

get_target_property(target_type mcga TYPE)
if (target_type STREQUAL SHARED_LIBRARY)
    target_compile_definitions(mcga PRIVATE -DDEFINE_BR_ENTRY_POINT)
endif ()

target_compile_definitions(mcga PRIVATE -DNO_MEMORY_FALLBACK=1)

target_link_libraries(mcga PRIVATE BRender::DDI)
target_link_libraries(mcga PUBLIC mcga-headers)

set_target_properties(mcga PROPERTIES
        EXPORT_NAME "Drivers::MCGA"
        FOLDER "BRender/Drivers"
        )

add_library(BRender::Drivers::MCGA::Headers ALIAS mcga-headers)
add_library(BRender::Drivers::MCGA ALIAS mcga)
