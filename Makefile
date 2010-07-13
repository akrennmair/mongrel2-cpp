CXX=g++
CXXFLAGS=-ggdb -Wall -Wextra
LDFLAGS=-L/usr/local/lib
LIBS=-lzmq -ljson

TARGET=m2pp-test
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	$(RM) $(TARGET) $(OBJS)

.PHONY: clean
