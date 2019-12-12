CXX = `root-config --cxx` 
CFLAGS = `root-config --cflags`\
	-O3 -W -Wall -Wextra -Wno-long-long \
	-fno-common -g \

LDFLAGS = `root-config --glibs --auxlibs` -lGeom -lgfortran -lm

energy: energy.C Reader.o
	$(CXX) $(CFLAGS) -o energy energy.C Reader.o $(LDFLAGS)

Reader.o: Reader.h Reader.C
	$(CXX) $(CFLAGS) -c Reader.C	
