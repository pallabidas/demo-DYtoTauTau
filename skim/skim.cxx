/*
 * Implementation of the skimming step of the analysis
 *
 * The skimming step reduces the inital generic samples to a dataset optimized
 * for this specific analysis. Most important, the skimming removes all events
 * from the initial dataset, which are not of interest for our study and builds
 * from the reconstructed muons and taus a valid pair, which may originate from
 * the decay of a Higgs boson.
 */


#include "ROOT/RDataFrame.hxx"
#include "ROOT/RSnapshotOptions.hxx"
#include "ROOT/RVec.hxx"

#include "Math/Vector4D.h"
#include "TChain.h"
#include "TROOT.h"
#include "TStopwatch.h"

#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>

#include "helpers/helpers.h"
#include "helpers/leadingtautau.h"

/*
 * Main function of the skimming step of the analysis.
 */
 
int main(int argc, char **argv) {

    // Can add command-line arguments for bells and whistles, for now ignore them 
    (void) argc;
    (void) argv;

    ROOT::EnableImplicitMT(); // Tell ROOT we want to go parallel
   
    TStopwatch time;  // track how long it took our script to run
    time.Start();

    // Declare inputs
    // Use an example file from this dataset: /VBFHToTauTau_M125_TuneCP5_13TeV-powheg-pythia8/RunIISummer20UL18NanoAODv9-106X_upgrade2018_realistic_v16_L1v1-v1/NANOAODSIM
    std::string line = "root://cmsxrootd.fnal.gov///store/mc/RunIISummer20UL18NanoAODv9/VBFHToTauTau_M125_TuneCP5_13TeV-powheg-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/130000/5BD57963-BF8F-9A48-854D-E344A5F555BB.root";
	const char *filename = line.c_str();

    // Declare output file name
    const std::string output = "out.root";
    std::cout << ">>> skim.cxx: Output name: " << output << std::endl;

    // Get the inputs 
    TChain *ch = new TChain("Events");  // NanoAOD contains a TTree called "Events"
	int result = ch->Add(filename);
	if (result == 0) {
	    std::cout << ">>> [ERROR:] skim.cxx: Failed to find file Events TTree in the input NanoAOD file!" << std::endl;
        return 1;
	}
	else {
        std::cout << ">>> skim.cxx: Added " << filename << " Events TTree" << std::endl;
    }

    // Create the RDataFrame object 
    ROOT::RDataFrame df(*ch);

    ////////// Event selection
    auto df2 = ApplyLooseSelection(df); 
    auto df3 = SelectLeadingPair(df2);
    auto df4 = GetLeadingPairInfo(df3);
    auto dfFinal = df4;

    // Save output n-tuple
    std::vector<std::string> finalVariables = {"pt_1", "eta_1", "phi_1", "m_1", "pt_2", "eta_2", "phi_2", "m_2", "m_vis", "pt_vis"};
    dfFinal.Snapshot("event_tree", output, finalVariables);

    // Print the cutflow report
    auto report = dfFinal.Report();
    report->Print();

    time.Stop();
    time.Print();

    return 0;
}
