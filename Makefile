CXX=g++
CXXFLAGS=-ggdb -Wall -Wextra -Ilib
LDFLAGS=-L/usr/local/lib -L.
LIBS=-lzmq -ljson -lm2pp

AR=ar
RANLIB=ranlib

LIBRARY=libm2pp.a

TEST=m2pp-test
CGI=m2pp-cgi

LIBOBJS=$(patsubst %.cpp,%.o,$(wildcard lib/*.cpp))
CGIOBJS=$(patsubst %.cpp,%.o,$(wildcard cgi/*.cpp))

all: $(LIBRARY) $(TEST) $(CGI)

$(TEST): $(TEST).o $(LIBRARY)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(TEST).o $(LIBS)

$(CGI): $(CGIOBJS) $(LIBRARY)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(CGIOBJS) $(LIBS)

$(LIBRARY): $(LIBOBJS)
	$(RM) $@
	$(AR) qc $@ $^
	$(RANLIB) $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	$(RM) $(LIBOBJS) $(LIBRARY) $(TEST) $(TEST).o $(CGI) $(CGIOBJS)

.PHONY: clean
