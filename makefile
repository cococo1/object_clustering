CC = g++
SRCDIR = src
BUILDDIR = build
TARGET = cluster
LDIR = /usr/local/Cellar/opencv/2.4.9/lib
IDIR1 = /usr/local/Cellar/opencv/2.4.9/include
IDIR2 = /Users/Max/Desktop/object_clustering/include
SRCEXT = cc
SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
LIBS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video

OBJ = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
TEST_OBJ = build/image.o build/object.o build/object_detector.o build/gui_functions.o build/abstract_cluster_algorithm.o build/k_means_clustering_algorithm.o build/test.o
CFLAGS = -Wall -std=c++11

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT) 
#	@echo "$(CC) $(CFLAGS) -I$(IDIR1) -I$(IDIR2) -c -o $@ $^";
	$(CC) $(CFLAGS) -I$(IDIR1) -I$(IDIR2) -c -o $@ $^

$(TARGET): $(OBJ)
#	@echo "$(CC) $(CFLAGS) -L$(LDIR) $(LIBS) -o $@ $^";
	$(CC) $(CFLAGS) -L$(LDIR) $(LIBS) -o bin/$@ $^

build/test.o: test/test.cc test/*.h 
	$(CC) $(CFLAGS) -I$(IDIR1) -I$(IDIR2) -c -o $@ $<

test: $(TEST_OBJ)
#	@echo "$(CC) $(CFLAGS) -L$(LDIR) $(LIBS) -o $@ $^";
	$(CC) $(CFLAGS) -L$(LDIR) $(LIBS) -o bin/$@ $^ 

clean:
#	@echo "rm -f $(BUILD)/*.o $(TARGET)";
	rm -f $(BUILDDIR)/*.o $(TARGET) bin/test

.PHONY: clean

