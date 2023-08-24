##
# Add all core targets to the "Core" export, and
# configure their "include" and "ddi" filesets to be copied.
##

include(GNUInstallDirs)

install(TARGETS
        brender brender-ddi
        EXPORT Core
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/brender
        )

install(TARGETS
        brender-inc brender-inc-ddi
        inc fmt fw host math nulldev pixelmap std v1db
        EXPORT Core
        FILE_SET include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/brender
        FILE_SET ddi DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/brender/ddi
        )

if (BRENDER_BUILD_DRIVERS)
    if (TARGET glrend)
        install(TARGETS glrend-headers
                EXPORT Core
                FILE_SET include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/brender/glrend
                )

        install(TARGETS glrend
                EXPORT Core
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/brender
                )
    endif()

    if (TARGET sdl2dev)
        install(TARGETS sdl2dev-headers
                EXPORT Core
                FILE_SET include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/brender/sdl2dev
                )

        install(TARGETS sdl2dev
                EXPORT Core
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/brender
                )
    endif()

    if (TARGET mcga)
        install(TARGETS mcga-headers
                EXPORT Core
                FILE_SET include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/brender/mcga
                )

        install(TARGETS mcga
                EXPORT Core
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/brender
                )
    endif()

    if (TARGET vesa)
        install(TARGETS vesa-headers
                EXPORT Core
                FILE_SET include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/brender/vesa
                )

        install(TARGETS vesa
                EXPORT Core
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/brender
                )
    endif()

    if (TARGET softrend)
        install(TARGETS softrend
                EXPORT Core
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/brender
                )
    endif()

    if (TARGET pentprim)
        install(TARGETS pentprim
                EXPORT Core
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/brender
                )
    endif()

endif ()


install(EXPORT Core
        FILE BRenderTargets.cmake
        NAMESPACE BRender::
        DESTINATION lib/cmake/brender
        )

include(CMakePackageConfigHelpers)

configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/BRenderConfig.cmake"
        INSTALL_DESTINATION lib/cmake/brender
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/BRenderConfigVersion.cmake"
        VERSION "1.3.2"
        COMPATIBILITY AnyNewerVersion
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/BRenderConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/BRenderConfigVersion.cmake
        DESTINATION lib/cmake/brender
        )

# pkg-config
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/brender.pc.in ${CMAKE_CURRENT_BINARY_DIR}/brender.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/brender.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
