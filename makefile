.PHONY: build format test clean
build:
	cmake --preset Debug
	cmake --build build/Debug

format:
	git ls-files *.cpp | xargs clang-format -i -style=file --verbose
	git ls-files *.hpp | xargs clang-format -i -style=file --verbose

test:
	ctest --preset default

clean:
	cmake --build build/Release --target clean
	cmake --build build/Debug --target clean

all: format build test

