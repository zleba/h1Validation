all: pythiaHEPMC  RivetH1_2016_I1496981.so

pythiaHEPMC: pythiaHEPMC.cc
	g++  -I${CMAKE_PREFIX_PATH}/include  $< -o $@  -O2  -pedantic -W -Wall -Wshadow -fPIC  -ldl -L${CMAKE_PREFIX_PATH}/lib  -lpythia8 -lHepMC 

RivetH1_2016_I1496981.so: H1_2016_I1496981.cc
	rivet-buildplugin $@ $<
