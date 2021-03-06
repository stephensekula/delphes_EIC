// main44.cc is a part of the PYTHIA event generator.
// Copyright (C) 2020 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL v2 or later, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Author: Stefan Prestel <stefan.prestel@thep.lu.se>.

// Keywords: LHE file; hepmc;

// This program (main44.cc) illustrates how a file with HepMC2 events can be
// generated by Pythia8. See main45.cc for how to ouput HepMC3 events instead.
// Note: both main44.cc and main45.cc can use the same main44.cmnd input card.

#include "Pythia8/Pythia.h"
//#include "HepMCInterface.h" //Interface HepMC to Pythia8
//#include "HepMC/GenEvent.h" //Generated HepMC event
//#include "HepMC/IO_GenEvent.h" //IO stream for HepMC event
#include "Pythia8Plugins/HepMC2.h"
#include <unistd.h>

using namespace Pythia8;

//==========================================================================
// Example main programm to illustrate merging.
int main( int argc, char* argv[] ){
  // Check that correct number of command-line arguments
  if (argc != 3) {
    cerr << " Unexpected number of command-line arguments ("<<argc<<"). \n"
         << " You are expected to provide the arguments" << endl
         << " 1. Input file for settings" << endl
         << " 2. Output file for HepMC events" << endl
         << " Program stopped. " << endl;
    return 1;
  }
  Pythia pythia;
  // Input parameters:
  pythia.readFile(argv[1],0);

  // Interface for conversion from Pythia8::Event to HepMC one.
  HepMC::Pythia8ToHepMC ToHepMC;
  ToHepMC.set_store_xsec(false);
  //HepMC::I_Pythia8 ToHepMC;
  // Specify file where HepMC events will be stored.
  HepMC::IO_GenEvent ascii_io(argv[2], std::ios::out);

  // Allow abort of run if many errors.
  int  nAbort  = pythia.mode("Main:timesAllowErrors");
  int  iAbort  = 0;
  bool doAbort = false;

  cout << endl << endl << endl;
  cout << "Start generating events" << endl;

  long nEvent = pythia.settings.mode("Main:numberOfEvents");
  int nRuns = pythia.mode("Main:numberOfSubruns");

  double sigmaTotal(0.), errorTotal(0.);
  // Loop over subruns with varying number of jets.
  // Initialise.
  pythia.init();
  std::cout << " CROSS SECTION " <<  pythia.info.sigmaGen()*1e9 << std::endl;
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    if (!pythia.next()) continue;
    double evtweight         = pythia.info.weight();
    float xs = pythia.info.sigmaGen();
    // Do not print zero-weight events.                                                                                                                                                                   
    if ( evtweight == 0. ) continue;

    // Find number of all final charged particles and fill histogram.
    // Construct new empty HepMC event and fill it.
    // Default units are ( HepMC3::Units::GEV, HepMC3::Units::MM)
    // but can be changed in the GenEvent constructor.
    
    HepMC::GenEvent* hepmcevt = new HepMC::GenEvent();
   

    //normhepmc = xs / double(1e9*nEvent);
    hepmcevt->weights().push_back(1.0);
    ToHepMC.fill_next_event( pythia, hepmcevt );


    HepMC::GenCrossSection xsec;
    //xsec.set_cross_section( pythia.info.sigmaGen()*1e9, pythia.info.sigmaErr()*1e9 );
    xsec.set_cross_section( 1.0, 0.01 );    
    hepmcevt->set_cross_section( xsec );
    
    // Write the HepMC event to file. Done with it.                                                                                                                                                       
    ascii_io << hepmcevt;
    delete hepmcevt;

 


   

  // End of event loop. Statistics. Histogram.
     
  }

  std::cout << "THIS IS THE CROSS SECTION " <<  pythia.info.sigmaGen() <<std::endl;   
    // print cross section, errors
    pythia.stat();


  // Done
  return 0;

}
