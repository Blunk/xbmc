SRCS=DriverPrimitive.cpp \
     JoystickMonitor.cpp \
     JoystickTranslator.cpp \
     KeymapHandler.cpp \

LIB=input_joysticks.a

include ../../../Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
