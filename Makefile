
OBJS = main.o htmlwriter.o lzw.o stream.o util.o
CPPFLAGS = -g -Wall


gif2html: $(OBJS)
	g++ $(CPPFLAGS) -o gif2html $+


clean:
	rm gif2html $(OBJS)


%.o: %.cpp gif2html.h
	g++ $(CPPFLAGS) -c $<




