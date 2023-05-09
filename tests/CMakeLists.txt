enable_testing()

include(GoogleTest)
include(CompileOptions)

set(hashmap_target hashmap_test)
add_executable(${hashmap_target})

set_compile_options(${hashmap_target})

target_sources(
	${hashmap_target}
	PRIVATE
		hashmap.test.cpp
)
target_include_directories(
	${hashmap_target}
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(
	${hashmap_target}
	PRIVATE
        GTest::gtest_main
)

gtest_discover_tests(${hashmap_target})



