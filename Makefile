CXX = g++
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -pthread -I./include
SRC = src/browser_core/AdBlocker.cpp \
      src/browser_core/NetworkInterceptor.cpp \
      src/browser_core/SchemeHandler.cpp \
      src/browser_core/PrivacyShield.cpp \
      src/browser_core/JsBridge.cpp \
      src/browser_core/BookmarkHistoryStore.cpp \
      src/browser_core/TabManager.cpp \
      src/browser_core/BrowserEngine.cpp \
      src/browser_core/main.cpp

OBJ = $(SRC:.cpp=.o)
TARGET = bin/atlas_browser_core

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)
	@echo "Build successful: $(TARGET)"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(TARGET)
	./$(TARGET) --test

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f src/browser_core/*.o bin/*

.PHONY: all test run clean
