add_executable(uid_str)

target_link_libraries(uid_str PRIVATE iuic_core)

target_sources(uid_str
	PUBLIC
	"uid-str.cpp"
)

add_test(NAME "iuic-scheme-builder[uid::static]" COMMAND uid_str LABELS "unit;core")



add_executable(uid_unique)

target_link_libraries(uid_unique PRIVATE iuic_core)

target_sources(uid_unique
	PUBLIC
	"uid-unique.cpp"
)

add_test(NAME "iuic-scheme-builder[uid::unique]" COMMAND uid_unique LABELS "unit;core")
