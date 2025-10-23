



project(iuic_base_key_map)

add_library(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PUBLIC
    FILE_SET iuic_base_key_map_modules
    TYPE CXX_MODULES
    FILES
    "src/base-key-map.cppm"
  )

target_link_libraries(${PROJECT_NAME} PUBLIC iuic_core)
