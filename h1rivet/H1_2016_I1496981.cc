// -*- C++ -*-
#include <cmath>
#include "Rivet/Analysis.hh"
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/FastJets.hh"
#include "Rivet/Projections/DISKinematics.hh"
#include "Rivet/Projections/DISFinalState.hh"
#include "Rivet/Projections/ChargedFinalState.hh"
#include "Rivet/Tools/BinnedHistogram.hh"

namespace Rivet {


/// @brief Add a short analysis description here
class H1_2016_I1496981 : public Analysis {
public:

/// Constructor
   DEFAULT_RIVET_ANALYSIS_CTOR(H1_2016_I1496981);


   /// @name Analysis methods
   //@{
      
   /// Book histograms and initialise projections before the run
   void init() {

      // Initialise and register projections
      // Projections
      const DISKinematics& diskin = declare(DISKinematics(), "Kinematics");
      const DISFinalState& fsbreit =
         declare(DISFinalState(diskin, DISFinalState::BREIT), "FSB");
      declare(FastJets(fsbreit,
                       fastjet::kt_algorithm,
                       fastjet::RecombinationScheme::pt_scheme,1.0),
              "JetsBreit"); 

      // HIstograms
      _h_inclusiveDIS=Histo1DPtr
         (new YODA::Histo1D
          ({5.5,8.,11.,16.,22.,30.,42.,60.,80.},"inclusiveDIS",
           ";Q^2 [GeV];sigma [pb]"));
      for(int k=0;k<8;k++) {
         _h_inclusive_jets.push_back(bookHisto1D(1+k, 1, 1));
         _h_dijets.push_back(bookHisto1D(9+k, 1, 1));
         _h_trijets.push_back(bookHisto1D(17+k, 1, 1));
         _hnorm_inclusive_jets.push_back(bookHisto1D(25+k, 1, 1));
         _hnorm_dijets.push_back(bookHisto1D(33+k, 1, 1));
         _hnorm_trijets.push_back(bookHisto1D(41+k, 1, 1));
      }
      _h_inclusive_jetsHighQ2=bookHisto1D(51,1,1);
      _snorm_inclusive_jetsHighQ2=bookScatter2D(52,1,1);
      _h_inclusiveDIShighQ2=Histo1DPtr(_h_inclusive_jetsHighQ2->newclone());
   }


   /// Perform the per-event analysis
   void analyze(const Event& event) {
      // DIS kinematics

      const DISKinematics& kin = apply<DISKinematics>(event, "Kinematics");
      const int orientation = kin.orientation();
      const double weight = event.weight();

      double q2  = kin.Q2();
      double y   = kin.y();

      if (!inRange(y, 0.2, 0.6)) return;

      ssize_t ibinLowQ2=_h_inclusiveDIS->binIndexAt(q2/GeV2);
      ssize_t ibinHighQ2=_h_inclusiveDIShighQ2->binIndexAt(q2/GeV2);

      bool isLowQ2=(ibinLowQ2>=0)&&
         (ibinLowQ2<(ssize_t)_h_inclusive_jets.size());
      bool isHighQ2=(ibinHighQ2>=0)&&
         (ibinHighQ2<(ssize_t)_h_inclusiveDIShighQ2->numBins());

      double binWidthHighQ2=0.;
      if(isLowQ2) {
         _h_inclusiveDIS->fill(q2,weight);
      } else if(isHighQ2) {
         binWidthHighQ2=_h_inclusiveDIShighQ2->bin(ibinHighQ2).xWidth();
         _h_inclusiveDIShighQ2->fill(q2/GeV2,weight*binWidthHighQ2);
      }

      // Boost to Breit and back
      const LorentzTransform breitboost = kin.boostBreit() ;
      const LorentzTransform breitboostInv = breitboost.inverse();

      // jets in Breit frame
      const Jets jets = apply<FastJets>(event, "JetsBreit").
         jets(Cuts::Et > 4*GeV, cmpMomByEt); 

      // Jet selection
      int njet = 0  ;
      double pT_dijet = 0. ;  
      double pT_trijet = 0. ;  

      foreach (const Jet& j, jets) {
         double pT = j.Et() ;

         //if (pT< 4*GeV) continue;

         const FourMomentum LabMom = breitboostInv.transform(j.momentum()); 

         if (!inRange(LabMom.eta()*orientation,-1, 2.5)) continue;

         if(isHighQ2 && ((pT>=5.*GeV)&&(pT<7.*GeV))) {
            _h_inclusive_jetsHighQ2->fill(q2/GeV2,weight*binWidthHighQ2);
         }

         if(isLowQ2) {
            // inclusive jets
            double binWidth=0.;
            size_t ibinPT=_h_inclusive_jets[ibinLowQ2]->binIndexAt(pT/GeV);
            if((ibinPT>=0)&&(ibinPT<_h_inclusive_jets[ibinLowQ2]->numBins())) {
               binWidth=_h_inclusive_jets[ibinLowQ2]->bin(ibinPT).xWidth();
            }

            _h_inclusive_jets[ibinLowQ2]->fill(pT/GeV,weight*binWidth);
            _hnorm_inclusive_jets[ibinLowQ2]->fill(pT/GeV,weight*binWidth);
         }
         // determine scales and jet multiplicity
         // this works only if the jets are ordered in pt
         njet = njet + 1;
         if (njet <= 2 ) pT_dijet = pT_dijet + j.Et();
         if (njet <= 3 ) pT_trijet = pT_trijet + j.Et();
      }
      // count dijet and trijet events
      if(isLowQ2 && (njet>=2)) {
         double pTavg=pT_dijet/2./GeV;
         double binWidth=0.;
         size_t ibinPT=_h_dijets[ibinLowQ2]->binIndexAt(pTavg);
         if((ibinPT>=0)&&(ibinPT<_h_dijets[ibinLowQ2]->numBins())) {
            binWidth=_h_dijets[ibinLowQ2]->bin(ibinPT).xWidth();
         }
         _h_dijets[ibinLowQ2]->fill(pTavg,weight*binWidth);
         _hnorm_dijets[ibinLowQ2]->fill(pTavg,weight*binWidth);
      }

      if(isLowQ2 && (njet>=3)) {
         double pTavg=pT_trijet/3./GeV;
         double binWidth=0.;
         size_t ibinPT=_h_trijets[ibinLowQ2]->binIndexAt(pTavg);
         if((ibinPT>=0)&&(ibinPT<_h_trijets[ibinLowQ2]->numBins())) {
            binWidth=_h_trijets[ibinLowQ2]->bin(ibinPT).xWidth();
         }
         _h_trijets[ibinLowQ2]->fill(pTavg,weight*binWidth);
         _hnorm_trijets[ibinLowQ2]->fill(pTavg,weight*binWidth);
      }
   }

   /// Normalise histograms etc., after the run
   void finalize() {
      // normalized high Q2
      divide(_h_inclusive_jetsHighQ2,_h_inclusiveDIShighQ2,
             _snorm_inclusive_jetsHighQ2);
      // histograms with cross sections
      double to_crosec=crossSection()/sumOfWeights();
      for(size_t k=0;k<_h_inclusive_jets.size();k++) {
         scale(_h_inclusive_jets[k],to_crosec);
         scale(_h_dijets[k],to_crosec);
         scale(_h_trijets[k],to_crosec);
         // normalized low Q2
         double normToDis=1./_h_inclusiveDIS->bin(k).sumW();
         scale(_hnorm_inclusive_jets[k],normToDis);
         scale(_hnorm_dijets[k],normToDis);
         scale(_hnorm_trijets[k],normToDis);
      }
      scale(_h_inclusiveDIS,to_crosec); 
      scale(_h_inclusive_jetsHighQ2,to_crosec);
   }

   //@}

   /// @name Histograms
   //@{
   Histo1DPtr _h_inclusiveDIS;
   Histo1DPtr _h_inclusiveDIShighQ2;
   Histo1DPtr _h_inclusive_jetsHighQ2;
   Scatter2DPtr _snorm_inclusive_jetsHighQ2;
   vector<Histo1DPtr> _h_inclusive_jets, _h_dijets, _h_trijets;
   //BinnedHistogram<double> _h_inclusive_jets, _h_dijets, _h_trijets;
   vector<Histo1DPtr> _hnorm_inclusive_jets, _hnorm_dijets, _hnorm_trijets;

   //@}
};


// The hook for the plugin system
DECLARE_RIVET_PLUGIN(H1_2016_I1496981);

// end namespace Rivet
}
