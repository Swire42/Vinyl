CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)
COMP = $(CC) $(CFLAGS)

vinyl: main.o in_out.o effects.o bobine.o filter.o engraving.o percent.o diskinfo.o needle.o trackingangle.o
	$(CC) $(LFLAGS) $^ -o $@

%.o : %.cpp audio.hpp filter.hpp percent.hpp diskinfo.hpp needle.hpp
	$(COMP) $< -o $@

clean :
	rm -f vinyl *.o
