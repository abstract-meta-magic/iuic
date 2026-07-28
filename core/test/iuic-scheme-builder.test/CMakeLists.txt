add_executable(uid_static)

target_link_libraries(uid_static PRIVATE iuic_core)

target_sources(uid_static
	PUBLIC
	"uid-static.cpp"
)

add_test(NAME "iuic-scheme-builder[uid::static]" COMMAND uid_static LABELS "unit;core")



add_executable(uid_ordered)

target_link_libraries(uid_ordered PRIVATE iuic_core)

target_sources(uid_ordered
	PUBLIC
	"uid-ordered.cpp"
)

add_test(NAME "iuic-scheme-builder[uid::ordered]" COMMAND uid_ordered LABELS "unit;core")



add_executable(uid_stable)

target_link_libraries(uid_stable PRIVATE iuic_core)

target_sources(uid_stable
	PUBLIC
	"uid-stable.cpp"
)

add_test(NAME "iuic-scheme-builder[uid::stabel]" COMMAND uid_stable LABELS "unit;core")

