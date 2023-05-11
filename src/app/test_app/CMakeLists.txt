set(target test_app)
add_executable(${target})

include(CompileOptions)

set_compile_options(${target})

target_sources(
	${target}
	PRIVATE
		main.cpp
)
target_include_directories(
	${target}
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(
	${target}
	PRIVATE
)
   