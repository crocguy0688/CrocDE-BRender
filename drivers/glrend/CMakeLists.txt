project(glrend)

add_custom_command(
        OUTPUT brender.vert.glsl.h
        COMMAND "${PERL_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/embedshader.pl" "g_DefaultVertexShader" "${CMAKE_CURRENT_SOURCE_DIR}/brender.vert.glsl" brender.vert.glsl.h
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/brender.vert.glsl"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/embedshader.pl"
)

add_custom_command(
        OUTPUT brender.frag.glsl.h
        COMMAND "${PERL_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/embedshader.pl" "g_DefaultFragmentShader" "${CMAKE_CURRENT_SOURCE_DIR}/brender.frag.glsl" brender.frag.glsl.h
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/brender.frag.glsl"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/embedshader.pl"
)

set(GLSTATE_FILES
        glcache.c
        glstate.c
        glstate.h
        glstate_cull.c
        glstate_hidden.c
        glstate_ip.h
        glstate_light.c
        glstate_matrix.c
        glstate_output.c
        glstate_primitive.c
        glstate_surface.c
        )
source_group("STATE" FILES ${GLSTATE_FILES})

set(VIDEO_FILES
        checkboard64.c
        shader_brender.c
        shader_default.c
        shader_text.c
        video.c
        video.h
        )
source_group("VIDEO" FILES ${VIDEO_FILES})

set(RENDERER_FILES
        gv1buckt.c
        gv1buckt.h

        gv1model.c
        gv1model.h

        gstored.c
        gstored.h
        v1model.c

        onscreen.c

        renderer.c
        renderer.h

        sbuffer.h
        sbuffer.c

        sstate.c
        sstate.h
        )
source_group("Renderer" FILES ${RENDERER_FILES})

set(GLREND_FILES
        drv.h
        drv_ip.h

        driver.c

        outfcty.c
        outfcty.h

        ext_procs.c

        rendfcty.c
        rendfcty.h

        device.c
        device.h

        devpixmp.c
        devpixmp.h
        devpmglf.c

        font.c
        font.h

        template.h

        quad.c
        util.c

        glad.c
        )

set(INC_FILES
        include/brglrend.h
        include/brglrend_p.h

        include/glad/glad.h
        include/KHR/khrplatform.h
        )

add_library(glrend-headers INTERFACE)
target_sources(glrend-headers INTERFACE
        FILE_SET include
        TYPE HEADERS
        BASE_DIRS include
        FILES ${INC_FILES}
        )

target_include_directories(glrend-headers INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/brender/glrend>
        )

set_target_properties(glrend-headers PROPERTIES
        EXPORT_NAME "Drivers::GLRend::Headers"
        FOLDER "BRender/Drivers"
        )

add_library(glrend
        ${GLREND_FILES}
        ${RENDERER_FILES}
        ${VIDEO_FILES}
        ${GLSTATE_FILES}

        "${CMAKE_CURRENT_BINARY_DIR}/brender.vert.glsl.h"
        "${CMAKE_CURRENT_BINARY_DIR}/brender.frag.glsl.h"
        )

get_target_property(target_type glrend TYPE)
if (target_type STREQUAL SHARED_LIBRARY)
    target_compile_definitions(glrend PRIVATE -DDEFINE_BR_ENTRY_POINT)
endif ()

target_include_directories(glrend PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(glrend PRIVATE OpenGL::GL BRender::DDI)
target_link_libraries(glrend PUBLIC glrend-headers)

set_target_properties(glrend PROPERTIES
        C_STANDARD 11
        C_STANDARD_REQUIRED TRUE
        EXPORT_NAME "Drivers::GLRend"
        FOLDER "BRender/Drivers"
        )

add_library(BRender::Drivers::GLRend::Headers ALIAS glrend-headers)
add_library(BRender::Drivers::GLRend ALIAS glrend)
