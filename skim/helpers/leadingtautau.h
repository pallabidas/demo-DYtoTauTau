#ifndef LEADING_TAU_TAU_H_INCL
#define LEADING_TAU_TAU_H_INCL

#include "helpers.h"

/**********************************************************/

template <typename T>
auto ApplyTrigger(T &df) {
  return df.Define("pass_HLT_IsoMu24", [](bool b) { return (int) b; }, {"HLT_IsoMu24"})
	  .Define("pass_HLT_IsoMu27", [](bool b) { return (int) b; }, {"HLT_IsoMu27"})
	  .Define("pass_HLT_Mu20Tau27",    [](bool b) { return (int) b; }, {"HLT_IsoMu20_eta2p1_LooseDeepTauPFTauHPS27_eta2p1_CrossL1"})
          .Define("trigger_mt_Mu24", [](int passHLTPath, float pt_1, float pt_2) {return (int) (passHLTPath && (pt_1 > 25) && (pt_2 > 20)); }, {"pass_HLT_IsoMu24", "pt_1", "pt_2"})
	  .Define("trigger_mt_Mu27", [](int passHLTPath, float pt_1, float pt_2) {return (int) (passHLTPath && (pt_1 > 28) && (pt_2 > 20)); }, {"pass_HLT_IsoMu24", "pt_1", "pt_2"})
	  .Define("trigger_mt_Mu20Tau27", [](int passHLTPath, float pt_1, float pt_2) {return (int) (passHLTPath && (pt_1 > 21) && (pt_2 > 32)); }, {"pass_HLT_Mu20Tau27", "pt_1", "pt_2"})
	  .Filter("trigger_mt_Mu24 || trigger_mt_Mu27 || trigger_mt_Mu20Tau27", "looseSelection.h: ApplyTrigger: Pass IsoMu24/IsoMu27/Mu20Tau27");
}

/* 
 * Perform a basic selection to get events that have >0 reco muons and >0 reco taus.
 */
template <typename T>
auto ApplyLooseSelection(T &df) {

  // Say an event has 5 muons in it, so the NanoAOD branches like Muon_pt, Muon_eta etc. are vectors of length 5 for this event. 
  // This new branch that we define called "goodMuons", is going to be a vector of integers of length 5 for this event (0 if that muon didn't pass these cuts, 1 if it did).

  return df.Define("goodMuons", "(Muon_pt > 19) && (abs(Muon_eta) < 2.4) && (Muon_mediumId == true) && (abs(Muon_dz) < 0.2) && (abs(Muon_dxy) < 0.045) && (Muon_pfRelIso04_all < 0.15)")
	   .Define("goodTaus", "(Tau_pt > 18) && (abs(Tau_eta) < 2.3) && (abs(Tau_dz) < 0.2) && (Tau_idDeepTau2017v2p1VSe > 3) && (Tau_idDeepTau2017v2p1VSjet > 3) && (Tau_idDeepTau2017v2p1VSmu > 1) && Tau_idDecayModeNewDMs && (Tau_decayMode != 5) && (Tau_decayMode != 6)")
	   .Define("goodElectrons", "(Electron_pt > 10) && (abs(Electron_eta) < 2.5) && (abs(Electron_dz) < 0.2) && (abs(Electron_dxy) < 0.045) && (Electron_mvaTTH > 0.8) && (Electron_lostHits <= 1) && (Electron_convVeto) && (Electron_pfRelIso03_all < 0.15)")
            // Per the above comment, if we just take the sum of this vector then we'll know if the event had muons and taus in this event.
           .Filter("Sum(goodMuons) > 0", "looseSelection.h: ApplyLooseSelection: Has muons passing loose selection")
           .Filter("Sum(goodTaus) > 0", "looseSelection.h: ApplyLooseSelection: Has taus passing loose selection")
	   .Filter("Sum(goodElectrons) == 0", "looseSelection.h: ApplyLooseSelection: Has no extra leptons");

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

/*
 * Check that the generator particles matched to the identified taus are
 * actually taus and add this information to the dataset.
 *
 * This information is used to estimate the fraction of events that are falsely
 * identified as taus, e.g., electrons or jets that could fake such a particle.
 */
template <typename T>
auto CheckGeneratorTaus(T &df, const bool isData) {
    if (isData) {
        return df.Define("gen_match", "false");
    } else {
        return df.Define("gen_match",
                         "Muon_genPartFlav[idx_1] == 15 && \
                          Tau_genPartFlav[idx_2] == 5");
    }
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
	     .Define("q_1",   "Muon_charge[idx_1]")

             .Define("pt_2",  "Tau_pt[idx_2]")
             .Define("eta_2", "Tau_eta[idx_2]")
             .Define("phi_2", "Tau_phi[idx_2]")
             .Define("m_2",   "Tau_mass[idx_2]")
	     .Define("q_2",   "Tau_charge[idx_2]")

    // Or as return values of functions
             .Define("p4_1", Helper::add_p4, {"pt_1", "eta_1", "phi_1", "m_1"})
             .Define("p4_2", Helper::add_p4, {"pt_2", "eta_2", "phi_2", "m_2"})
	     .Define("mt_1", Helper::compute_mt, {"pt_1", "phi_1", "PuppiMET_pt", "PuppiMET_phi"})
	     .Define("mt_2", Helper::compute_mt, {"pt_2", "phi_2", "PuppiMET_pt", "PuppiMET_phi"})
             
    // Or as lambda functions
             .Define("m_vis",  [](ROOT::Math::PtEtaPhiMVector p4_1, ROOT::Math::PtEtaPhiMVector p4_2) { return (float) (p4_1+p4_2).M();  }, {"p4_1", "p4_2"})
             .Define("pt_vis", [](ROOT::Math::PtEtaPhiMVector p4_1, ROOT::Math::PtEtaPhiMVector p4_2) { return (float) (p4_1+p4_2).Pt(); }, {"p4_1", "p4_2"});


}


/**********************************************************/

#endif
