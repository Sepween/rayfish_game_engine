CXX = clang++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -flto
INCLUDES = -I./ThirdParty \
			-I./ThirdParty/glm-0.9.9.8 \
			-I./ThirdParty/lua \
			-I./ThirdParty/box2d \
			-I./ThirdParty/SDL \
			-I./ThirdParty/SDL/SDL2 \
			-I./ThirdParty/SDL/SDL2_image \
			-I./ThirdParty/SDL/SDL2_mixer \
			-I./ThirdParty/SDL/SDL2_ttf
LINKERS = -lSDL2 \
			-lSDL2_image \
			-lSDL2_mixer \
			-lSDL2_ttf

SRC_DIRS = src \
			ThirdParty/box2d/collision \
			ThirdParty/box2d/common \
			ThirdParty/box2d/dynamics \
			ThirdParty/box2d/rope

CPP_SOURCES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
C_SOURCES = $(wildcard ThirdParty/lua/*.c)
CPP_OBJECTS = $(CPP_SOURCES:.cpp=.o)
C_OBJECTS = $(C_SOURCES:.c=.o)
NAME = game_engine_linux

# Main build rule
all: $(NAME)

# Link the object files into the executable
$(NAME): $(CPP_OBJECTS) $(C_OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(LINKERS) -o $@

# Compile C++ source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile C source files - using C compiler not C++
ThirdParty/lua/%.o: ThirdParty/lua/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean rule
clean:
	rm -f $(CPP_OBJECTS) $(C_OBJECTS) $(NAME)
