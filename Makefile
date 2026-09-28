CXX = g++
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -pthread -I./include

CORE_SRC = src/browser_core/AdBlocker.cpp \
           src/browser_core/NetworkInterceptor.cpp \
           src/browser_core/SchemeHandler.cpp \
           src/browser_core/PrivacyShield.cpp \
           src/browser_core/JsBridge.cpp \
           src/browser_core/BookmarkHistoryStore.cpp \
           src/browser_core/TabManager.cpp \
           src/browser_core/ContainerManager.cpp \
           src/browser_core/AutoContainerRouter.cpp \
           src/browser_core/WorkspaceManager.cpp \
           src/browser_core/AiAssistantEngine.cpp \
           src/browser_core/DownloadManager.cpp \
           src/browser_core/PerformanceMonitor.cpp \
           src/browser_core/ReaderModeEngine.cpp \
           src/browser_core/ExtensionRuntime.cpp \
           src/browser_core/BrowserEngine.cpp

CLI_SRC = src/browser_core/main.cpp
SERVER_SRC = src/browser_server/server.cpp

CORE_OBJ = $(CORE_SRC:.cpp=.o)
CLI_OBJ = $(CLI_SRC:.cpp=.o)
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)

TARGET_CLI = bin/atlas_browser_core
TARGET_SERVER = bin/atlas_browser_server

all: $(TARGET_CLI) $(TARGET_SERVER)

$(TARGET_CLI): $(CORE_OBJ) $(CLI_OBJ)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $@ $(CORE_OBJ) $(CLI_OBJ)
	@echo "Build successful: $(TARGET_CLI)"

$(TARGET_SERVER): $(CORE_OBJ) $(SERVER_OBJ)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $@ $(CORE_OBJ) $(SERVER_OBJ)
	@echo "Build successful: $(TARGET_SERVER)"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(TARGET_CLI)
	./$(TARGET_CLI) --test

run: $(TARGET_CLI)
	./$(TARGET_CLI)

server: $(TARGET_SERVER)
	./$(TARGET_SERVER)

clean:
	rm -f src/browser_core/*.o src/browser_server/*.o bin/*

.PHONY: all test run server clean
