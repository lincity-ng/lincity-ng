all: autocrop

autocrop : autocrop.cxx
	$(CXX) -Wall -O2 -g -o $@ $< `pkg-config --cflags --libs libpng`


# EOF #
