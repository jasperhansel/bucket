.PHONY: all build clean

CLANG = /usr/local/bin/clang++
CLANGFLAGS = -Ofast -flto -fstrict-vtable-pointers -ffast-math \
	-fwhole-program-vtables -march=native -mtune=native -Weverything \
	-Wno-c++98-compat -std=c++1z -Wno-c++98-c++11-compat-binary-literal \
	-Wno-padded -pedantic -I code -Wno-shadow-field-in-constructor \
	-Wno-c++98-compat-pedantic -Wno-weak-vtables -DNDEBUG -fno-rtti
CLANGSOURCES = build/support/unicodefilereader-clang.o \
							 build/support/unicodecharacter-clang.o \
							 build/main-clang.o \
							 build/frontend/sourcefile-clang.o \
							 build/frontend/token-clang.o \
							 build/frontend/lexer-clang.o \
							 build/frontend/parser-clang.o \
							 build/abstract_syntax_tree/printer-clang.o \
							 build/code_generator/code_generator-clang.o \
							 build/compiler_objects/class-clang.o \
							 build/compiler_objects/field-clang.o \
							 build/compiler_objects/method-clang.o \
							 build/compiler_objects/module-clang.o \
							 build/compiler_objects/object-clang.o \
							 build/compiler_objects/scope-clang.o

GCC = /usr/local/bin/g++-7
GCCFLAGS = -Ofast -flto -fipa-pta -ffast-math  -march=native \
	-mtune=native -Wall -Wextra -Wpedantic -pedantic -std=c++1z -DNDEBUG \
	-fno-rtti -I code
GCCSOURCES = build/support/unicodefilereader-gcc.o \
							build/support/unicodecharacter-gcc.o \
							build/main-gcc.o \
							build/frontend/sourcefile-gcc.o \
							build/frontend/token-gcc.o \
							build/frontend/lexer-gcc.o \
							build/frontend/parser-gcc.o \
							build/abstract_syntax_tree/printer-gcc.o \
							build/code_generator/code_generator-gcc.o \
							build/compiler_objects/class-gcc.o \
							build/compiler_objects/field-gcc.o \
							build/compiler_objects/method-gcc.o \
							build/compiler_objects/module-gcc.o \
							build/compiler_objects/object-gcc.o \
							build/compiler_objects/scope-gcc.o

LIBS = -lutf8proc

all: build bucket-clang bucket-gcc

build:
	[ -d build ] || mkdir build build/support build/frontend \
	build/abstract_syntax_tree build/code_generator build/compiler_objects

bucket-clang: $(CLANGSOURCES)
	$(CLANG) $(CLANGFLAGS) $(LIBS) $(CLANGSOURCES) -o bucket-clang

bucket-gcc: $(GCCSOURCES)
	$(GCC) $(GCCFLAGS) $(LIBS) $(GCCSOURCES) -o bucket-gcc

build/%-clang.o: code/%.cxx
	$(CLANG) $(CLANGFLAGS) -c $< -o $@

build/%-gcc.o: code/%.cxx
	$(GCC) $(GCCFLAGS) -c $< -o $@

clean:
	rm -rf build
	rm bucket-clang bucket-gcc
