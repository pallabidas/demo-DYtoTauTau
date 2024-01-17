#ifndef LEADING_TAU_TAU_H_INCL
#define LEADING_TAU_TAU_H_INCL

#include "helpers.h"

/**********************************************************/

/* 
 * Perform a basic selection to get events that have >0 reco muons and >0 reco taus.
 */
template <typename T>
auto ApplyLooseSelection(T &df) {

  // Say an event has 5 muons in it, so the NanoAOD branches like Muon_pt, Muon_eta etc. are vectors of length 5 for this event. 
  // This new branch that we define called "goodMuons", is going to be a vector of integers of length 5 for this event (0 if that muon didn't pass these cuts, 1 if it did).

  return df.Define("goodMuons", "(Muon_pt > 19) && (abs(Muon_eta) < 2.4) && (Muon_mediumId == true) && (abs(Muon_dz) < 0.2) && (abs(Muon_dxy) < 0.045) && (Muon_pfRelIso04_all < 0.15)")
           .Define("goodTaus", "(Tau_pt > 18) && (abs(Tau_eta) < 2.3) && (abs(Tau_dz) < 0.2) && (Tau_idDeepTau2017v2p1VSe & 0x04) && (Tau_idDeepTau2017v2p1VSmu & 0x01) && (Tau_idDeepTau2017v2p1VSjet & 0x01) \
                                                && (Tau_decayMode != 5) && (Tau_decayMode != 6)")
            // Per the above comment, if we just take the sum of this vector then we'll know if the event had muons and taus in this event.
           .Filter("Sum(goodMuons) > 0", "looseSelection.h: ApplyLooseSelection: Has muons passing loose selection")
           .Filter("Sum(goodTaus) > 0", "looseSelection.h: ApplyLooseSelection: Has taus passing loose selection");

}

/**********************************************************/

/*
 * Perform selection of the leading pairs.
 */

template <typename T>
auto SelectLeadingPair(T &df) {
    
    return df.Define("pairIdx", Helper::build_pair, {"goodMuons", "goodTaus", 
                                                     "Muon_pt", "Muon_eta", "Muon_phi",
                                                     "Tau_pt", "Tau_eta", "Tau_phi"}) 
             .Define("idx_1", "pairIdx[0]")
             .Define("idx_2", "pairIdx[1]")
             .Filter("idx_1 != -1", "looseSelection.h: SelectLeadingPair: Valid first leg in selected pair")
             .Filter("idx_2 != -1", "looseSelection.h: SelectLeadingPair: Valid second leg in selected pair");


}

/**********************************************************/


/* 
 * Get the kinematics of the leading legs.
 */
template <typename T>
auto GetLeadingPairInfo(T &df) {

    // New branches can be defined as string expressions (these are compiled in-time)
    return df.Define("pt_1",  "Muon_pt[idx_1]")
             .Define("eta_1", "Muon_eta[idx_1]")
             .Define("phi_1", "Muon_phi[idx_1]")
             .Define("m_1",   "Muon_mass[idx_1]")

             .Define("pt_2", "Tau_pt[idx_2]")
             .Define("eta_2", "Tau_eta[idx_2]")
             .Define("phi_2", "Tau_phi[idx_2]")
             .Define("m_2",   "Tau_mass[idx_2]")

    // Or as return values of functions
             .Define("p4_1", Helper::add_p4, {"pt_1", "eta_1", "phi_1", "m_1"})
             .Define("p4_2", Helper::add_p4, {"pt_2", "eta_2", "phi_2", "m_2"})
             
    // Or as lambda functions
             .Define("m_vis",  [](ROOT::Math::PtEtaPhiMVector p4_1, ROOT::Math::PtEtaPhiMVector p4_2) { return (float) (p4_1+p4_2).M();  }, {"p4_1", "p4_2"})
             .Define("pt_vis", [](ROOT::Math::PtEtaPhiMVector p4_1, ROOT::Math::PtEtaPhiMVector p4_2) { return (float) (p4_1+p4_2).Pt(); }, {"p4_1", "p4_2"});


}


/**********************************************************/

#endif