



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
    "src/kitty-kit-form.cppm"
    "src/kitty-kit-base.cppm"
)

target_sources(kitty_kit PUBLIC
    ## containers
    "src/impl/containers/tl.cpp"
    "src/impl/containers/tc.cpp"
    "src/impl/containers/tr.cpp"
    "src/impl/containers/cl.cpp"
    "src/impl/containers/cc.cpp"
    "src/impl/containers/cr.cpp"
    "src/impl/containers/bl.cpp"
    "src/impl/containers/bc.cpp"
    "src/impl/containers/br.cpp"
    #-
    "src/impl/containers/lrt.cpp"
    "src/impl/containers/rlt.cpp"
    "src/impl/containers/lrc.cpp"
    "src/impl/containers/rlc.cpp"
    "src/impl/containers/lrb.cpp"
    "src/impl/containers/rlb.cpp"
    "src/impl/containers/tbl.cpp"
    "src/impl/containers/btl.cpp"
    "src/impl/containers/tbc.cpp"
    "src/impl/containers/btc.cpp"
    "src/impl/containers/tbr.cpp"
    "src/impl/containers/btr.cpp"
    ## buttons
    "src/impl/buttons/sb.cpp"
    ## form
    "src/impl/form/none.cpp"
    "src/impl/form/frame.cpp"
)

target_link_libraries(kitty_kit PUBLIC 
    iuic_core
    iuic_base_key_map)
