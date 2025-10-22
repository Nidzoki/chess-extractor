# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Iinclude -Wall -Wextra
SRCDIR = src
INCDIR = include
EXAMPLEDIR = examples

# Source files for the library
SOURCES = $(SRCDIR)/parser.cpp
OBJECTS = $(SOURCES:.cpp=.o)
LIBRARY = libpgn.a

# Example programs
EXAMPLES = basic_usage.exe advanced_test.exe performance_test.exe

# Default target
all: $(LIBRARY) examples

# Build the library
$(LIBRARY): $(OBJECTS)
	@echo Creating library $@...
	ar rcs $@ $^
	@echo Library $@ built successfully!

# Compile source files to object files
$(SRCDIR)/%.o: $(SRCDIR)/%.cpp $(INCDIR)/pgn/parser.hpp $(INCDIR)/pgn/types.hpp
	@echo Compiling $<...
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build all examples
examples: $(LIBRARY) $(addprefix $(EXAMPLEDIR)/, $(EXAMPLES))

# Individual example targets
$(EXAMPLEDIR)/basic_usage.exe: $(EXAMPLEDIR)/basic_usage.cpp $(LIBRARY)
	@echo Building $@...
	$(CXX) $(CXXFLAGS) $< $(LIBRARY) -o $@

$(EXAMPLEDIR)/advanced_test.exe: $(EXAMPLEDIR)/advanced_test.cpp $(LIBRARY)
	@echo Building $@...
	$(CXX) $(CXXFLAGS) $< $(LIBRARY) -o $@

$(EXAMPLEDIR)/performance_test.exe: $(EXAMPLEDIR)/performance_test.cpp $(LIBRARY)
	@echo Building $@...
	$(CXX) $(CXXFLAGS) $< $(LIBRARY) -o $@

# Clean build files
clean:
	@echo Cleaning build files...
	rm -f $(OBJECTS) $(LIBRARY)
	rm -f $(addprefix $(EXAMPLEDIR)/, $(EXAMPLES))
	@echo Clean complete!

# Run tests - Windows compatible
test: examples
	@echo === RUNNING ALL TESTS ===
	@echo === Basic Test ===
	@$(EXAMPLEDIR)/basic_usage.exe || echo Basic test failed!
	@echo.
	@echo === Advanced Test ===
	@$(EXAMPLEDIR)/advanced_test.exe || echo Advanced test failed!
	@echo.
	@echo === Performance Test ===
	@$(EXAMPLEDIR)/performance_test.exe || echo Performance test failed!
	@echo.
	@echo === ALL TESTS COMPLETED ===

# Run specific tests
test-basic: $(EXAMPLEDIR)/basic_usage.exe
	@$(EXAMPLEDIR)/basic_usage.exe

test-advanced: $(EXAMPLEDIR)/advanced_test.exe
	@$(EXAMPLEDIR)/advanced_test.exe

test-performance: $(EXAMPLEDIR)/performance_test.exe
	@$(EXAMPLEDIR)/performance_test.exe

# Development targets
debug: CXXFLAGS += -g -DDEBUG
debug: clean all

release: CXXFLAGS += -DNDEBUG
release: clean all

# Install (placeholder for future package management)
install: $(LIBRARY)
	@echo Install would copy headers and library to system directories
	@echo Not implemented yet - use locally for now

# Show library info
info:
	@echo === libpgn Library Information ===
	@echo Library: $(LIBRARY)
	@echo Sources: $(SOURCES)
	@echo Objects: $(OBJECTS)
	@echo Examples: $(EXAMPLES)
	@echo Compiler: $(CXX)
	@echo Flags: $(CXXFLAGS)
	@echo ================================

# Help target
help:
	@echo Available targets:
	@echo   all       - Build library and all examples (default)
	@echo   clean     - Remove all build files
	@echo   test      - Run all tests
	@echo   test-basic - Run basic test only
	@echo   test-advanced - Run advanced test only
	@echo   test-performance - Run performance test only
	@echo   debug     - Build with debug symbols
	@echo   release   - Build with release optimizations
	@echo   info      - Show build information
	@echo   help      - Show this help message

.PHONY: all clean examples test test-basic test-advanced test-performance debug release install info help