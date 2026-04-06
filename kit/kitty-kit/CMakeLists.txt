



project(kitty_kit)

add_library(kitty_kit STATIC)

add_library(iuic::kit::kitty ALIAS kitty_kit)

if(IUIC_TMP_CXX_STD)
	target_link_libraries(kitty_kit PRIVATE tmp_cxx_std)
endif()

target_sources(kitty_kit PUBLIC
    FILE_SET kitty_kit_modules
    TYPE CXX_MODULES
    FILES
    "src/kitty-kit-decl.cppm"
    "src/kitty-kit.cppm"
    "src/kitty-kit-containers.cppm"
    "src/kitty-kit-buttons.cppm"
    "src/kitty-kit-color.cppm"
    "src/kitty-kit-base.cppm"
)

target_sources(kitty_kit PUBLIC
    "src/kitty-kit.cpp"
    "src/impl/containers/cc.cpp"
)

target_link_libraries(kitty_kit PUBLIC 
    iuic_core
    iuic_base_key_map)
