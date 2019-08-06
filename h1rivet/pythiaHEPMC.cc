// main41.cc is a part of the PYTHIA event generator.
// Copyright (C) 2017 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Author: Mikhail Kirsanov, Mikhail.Kirsanov@cern.ch, based on main01.cc.
// This program illustrates how HepMC can be interfaced to Pythia8.
// It studies the charged multiplicity distribution at the LHC.
// HepMC events are output to the hepmcout41.dat file.

// WARNING: typically one needs 25 MB/100 events at the LHC.
// Therefore large event samples may be impractical.

#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC2.h"

#include <cassert>
using namespace std;

using namespace Pythia8;

int main(int argc, char **argv) {
    if(argc != 3) {
        cout << "please provide output HepMC file name + seed as arguments" << endl;
        assert(0);
    }

  // Interface for conversion from Pythia8::Event to HepMC event.
  HepMC::Pythia8ToHepMC ToHepMC;

  // Specify file where HepMC events will be stored.
  HepMC::IO_GenEvent ascii_io(argv[1], std::ios::out);

  // Generator. Process selection. LHC initialization. Histogram.
  Pythia pythia;
  pythia.readFile("pythiaHEPMC.cmnd");
  int nEvent = pythia.settings.mode("Main:numberOfEvents");
  pythia.readString("Random:setSeed = on");
  pythia.readString("Random:seed = 1"+string(argv[2]));

  pythia.init();

  // Begin event loop. Generate event. Skip if error.
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    if (!pythia.next()) continue;

    HepMC::GenEvent* hepmcevt = new HepMC::GenEvent();
    ToHepMC.fill_next_event( pythia, hepmcevt );

    //pythia.event.list();
    // Write the HepMC event to file. Done with it.
    ascii_io << hepmcevt;
    delete hepmcevt;

  // End of event loop. Statistics. Histogram.
  }
  pythia.stat();

  // Done.
  return 0;
}
