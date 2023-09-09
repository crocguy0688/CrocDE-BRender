foreach(target 3ds2br dquery dripmem mkblend mkfog mkranges mkshades paljoin texconv matconv)
	add_subdirectory(${target})
	set_property(TARGET ${target} PROPERTY FOLDER "BRender/Tools")
endforeach()

if (TARGET SDL2::SDL2)
    add_subdirectory(texview)
    set_property(TARGET texview PROPERTY FOLDER "BRender/Tools")
endif ()
