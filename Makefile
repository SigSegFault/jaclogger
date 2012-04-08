CXX = g++
LD = gcc
AR = ar
RM = rm -rf
MKDIR = mkdir -p

CXXFLAGS = -g -Wall
LDFLAS =

BINDIR = bin
OBJDIR = obj
SRCDIR = src
INCLUDEDIR = include
DIRS = $(BINDIR) $(OBJDIR)

LIBNAME = jaclogger
LIBPREFIX = lib
LIBSTATIC = $(BINDIR)/$(LIBPREFIX)$(LIBNAME).a
LIBSHARED = $(BINDIR)/$(LIBPREFIX)$(LIBNAME).so

INCLUDES = -I/usr/include -I/usr/local/include -I./include
LIBS = -L/usr/lib -L/usr/local/lib -lc

SOURCES = logger.cpp logdispatcher.cpp
OBJECTS = $(SOURCES:.cpp=.o)

RMLIST = $(addprefix $(OBJDIR)/, $(OBJECTS))
RMLIST += $(BINDIR)/$(LIBSTATIC) $(BINDIR)/$(LIBSHARED)

all : $(DIRS) $(LIBSHARED) $(LIBSTATIC)

$(LIBSHARED) : $(addprefix $(OBJDIR)/, $(OBJECTS))
	$(LD) -shared $(LDFLAS) -o $@ $^ $(LIBS)

$(LIBSTATIC) : $(addprefix $(OBJDIR)/, $(OBJECTS))
	$(AR) cr $@ $^

$(OBJDIR)/%.o:$(SRCDIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@

$(DIRS):
	$(MKDIR) $@

clean:
	$(RM) $(RMLIST)
