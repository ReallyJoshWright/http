TARGET := app
CXX := g++
STD := -std=c++23
DEBUG := -g -Wall -Wextra -pedantic
DEP := -MP -MD
INC := -I./include
SRC := src/http_server.cpp src/logger.cpp main.cpp
OBJ := src/http_server.o src/logger.o main.o
DEPFILES := src/http_server.d src/logger.d main.d

all: $(TARGET)

$(TARGET): $(OBJ)
	@$(CXX) $(STD) $(DEBUG) $(INC) $(DEP) $^ -o $@
%.o: %.cpp
	@$(CXX) $(STD) $(DEBUG) $(INC) $(DEP) -c $< -o $@

.PHONY: clean run

clean:
	@rm -rf $(TARGET)
	@rm -rf $(OBJ)
	@rm -rf $(DEPFILES)

run: $(TARGET)
	@./$(TARGET)

-include $(DEPFILES)
