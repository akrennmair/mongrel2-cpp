CXX=g++
CXXFLAGS=-ggdb -Wall -Wextra -Ilib
LDFLAGS=-L/usr/local/lib -L.
LIBS=-lzmq -ljson -lm2pp

AR=ar
RANLIB=ranlib

TARGET=m2pp-test
LIBRARY=libm2pp.a
LIBOBJS=$(patsubst %.cpp,%.o,$(wildcard lib/*.cpp))

all: $(LIBRARY) $(TARGET)

$(TARGET): $(TARGET).o $(LIBRARY)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(TARGET).o $(LIBS)

$(LIBRARY): $(LIBOBJS)
	$(RM) $@
	$(AR) qc $@ $^
	$(RANLIB) $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	$(RM) $(TARGET) $(LIBOBJS) $(LIBRARY) $(TARGET).o

.PHONY: clean
