CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra
LDFLAGS  ?=

SRCS := main.cpp tgaimage.cpp model.cpp sgl.cpp
OBJS := $(SRCS:.cpp=.o)
HDRS := geometry.h model.h sgl.h tgaimage.h

TARGET := renderer

# Model/image paths passed to the renderer. Override on the command line,
# e.g.:
#   make run MODEL="obj/african_head/african_head.obj"
#   make run MODEL="obj/diablo3_pose/diablo3_pose.obj obj/boggie/head.obj"
MODEL ?= obj/african_head/african_head.obj

ifeq ($(OS),Windows_NT)
RUN := $(TARGET)
else
RUN := ./$(TARGET)
endif

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	$(RUN) $(MODEL)

clean:
	$(RM) $(OBJS) $(TARGET)
