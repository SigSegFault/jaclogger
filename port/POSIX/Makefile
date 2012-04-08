CXX = g++
LD = gcc
AR = ar
RM = rm -rf
MKDIR = mkdir -p

CXXFLAGS = -g -Wall
LDFLAS =

OBJDIR = ../../obj
SRCDIR = .

INCLUDES = -I/usr/include -I/usr/local/include

SOURCES = posixmutex.cpp
OBJECTS = $(SOURCES:.cpp=.o)

RMLIST = $(addprefix $(OBJDIR)/, $(OBJECTS))

all : $(addprefix $(OBJDIR)/, $(OBJECTS))

$(OBJDIR)/%.o:$(SRCDIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@

clean :
	$(RM) $(RMLIST)
