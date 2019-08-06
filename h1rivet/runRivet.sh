#!/bin/bash
seed=10

mkdir -p /tmp/$USER/
fifo="/tmp/$USER/hepmc.fifo"
rm -f  $fifo
mkfifo $fifo

./pythiaHEPMC $fifo $seed &
rivet --pwd -a  H1_2016_I1496981 $fifo
