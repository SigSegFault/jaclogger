# Yeah, currently only unices supported
CXX = g++
LD = gcc
AR = ar
RM = rm -rf
MKDIR = mkdir -p
RANLIB = ranlib

CXXFLAGS = -g -Wall
LDFLAS =

BINDIR = lib
OBJDIR = obj
SRCDIR = src
INCLUDEDIR = include
PORTDIR = port
DIRS = $(BINDIR) $(OBJDIR)
CONFIG = include/config.h

LIBNAME = jaclogger
LIBPREFIX = lib
LIBSTATIC = $(BINDIR)/$(LIBPREFIX)$(LIBNAME).a
LIBSHARED = $(BINDIR)/$(LIBPREFIX)$(LIBNAME).so

INCLUDES = -I/usr/include -I/usr/local/include -I./include
LIBS = -L/usr/lib -L/usr/local/lib -lc -lpthread

SOURCES = logger.cpp logdispatcher.cpp logtostd.cpp logtostdcoloured.cpp \
	  logtofile.cpp
OBJECTS = $(SOURCES:.cpp=.o)
PORTOBJS = posixmutex.o

OBJECTS_IN_PLACE = $(addprefix $(OBJDIR)/, $(OBJECTS))
PORTOBJS_IN_PLACE = $(addprefix $(OBJDIR)/, $(PORTOBJS))

RMLIST = $(addprefix $(OBJDIR)/, $(OBJECTS))
RMLIST += $(LIBSTATIC) $(LIBSHARED)
RMLIST += $(CONFIG)

all : $(DIRS) $(LIBSHARED) $(LIBSTATIC)

$(LIBSHARED) : $(OBJECTS_IN_PLACE)  port
	$(LD) -shared $(LDFLAS) -o $@ $(filter %.o,$^) $(PORTOBJS_IN_PLACE) $(LIBS)

$(LIBSTATIC) : $(OBJECTS_IN_PLACE) port
	$(AR) cr $@ $(filter %.o,$^) $(PORTOBJS_IN_PLACE)
	$(RANLIB) $@

$(OBJDIR)/%.o:$(SRCDIR)/%.cpp configure
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@

configure : $(CONFIG)

 $(CONFIG) :
	echo "#ifndef LOGGER_CONFIG_H" > $(CONFIG)
	echo "#define LOGGER_CONFIG_H" >> $(CONFIG)
	echo "" >> $(CONFIG)
	echo "#define THREAD_SAFE_LOGGER" >> $(CONFIG)
	echo "" >> $(CONFIG)
	echo "#endif // LOGGER_CONFIG_H" >> $(CONFIG)

$(DIRS):
	$(MKDIR) $@

port : always_true
	$(MAKE) -C $(PORTDIR)

always_true :
	true

clean :
	$(RM) $(RMLIST)
	$(MAKE) -C $(PORTDIR) clean
