add_executable(uid_static)

target_link_libraries(uid_static PRIVATE iuic_test iuic_core)

target_sources(uid_static
	PUBLIC
	"uid-static.cpp"
)

add_test(NAME "iuic-scheme-builder[uid::static]" COMMAND uid_static LABELS "unit;core")



add_executable(uid_ordered)

target_link_libraries(uid_ordered PRIVATE iuic_test iuic_core)

target_sources(uid_ordered
	PUBLIC
	"uid-ordered.cpp"
)

add_test(NAME "iuic-scheme-builder[uid::ordered]" COMMAND uid_ordered LABELS "unit;core")



add_executable(uid_stable)

target_link_libraries(uid_stable PRIVATE iuic_test iuic_core)

target_sources(uid_stable
	PUBLIC
	"uid-stable.cpp"
)

add_test(NAME "iuic-scheme-builder[uid::stable]" COMMAND uid_stable LABELS "unit;core")


## [builder.memory]

add_executable(memory_persist)

target_link_libraries(memory_persist PRIVATE iuic_test iuic_core)

target_sources(memory_persist
	PUBLIC
	"memory-persist.cpp"
)

add_test(NAME "iuic-scheme-builder[memory::persist]" COMMAND memory_persist LABELS "unit;core")

add_executable(memory_tmp)

target_link_libraries(memory_tmp PRIVATE iuic_test iuic_core)

target_sources(memory_tmp
	PUBLIC
	"memory-tmp.cpp"
)

add_test(NAME "iuic-scheme-builder[memory::tmp]" COMMAND memory_tmp LABELS "unit;core")

