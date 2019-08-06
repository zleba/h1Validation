#To setup CERN software and set path to all LHAPDF data sets
source /cvmfs/sft.cern.ch/lcg/views/LCG_96/x86_64-slc6-gcc8-opt/setup.sh
#To downgrade rivet from rivet 3 to rivet 2
source /cvmfs/sft.cern.ch/lcg/releases/MCGenerators/rivet/2.6.2-239af/x86_64-slc6-gcc8-opt/rivetenv.sh

export LHAPDF_DATA_PATH=/cvmfs/sft.cern.ch/lcg/external/lhapdfsets/current/:`lhapdf-config --datadir`
