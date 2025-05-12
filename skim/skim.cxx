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
template <typename T>
auto AddEventWeight(T &df, const float numEvents, const float xsec, const float lumi) {
    return df.Define("weight", [=](){ return xsec / numEvents * lumi; });
}
 
int main(int argc, char **argv) {
    if(argc != 5) {
        std::cout << "Use executable with following arguments: ./skim input output cross_section integrated_luminosity" << std::endl;
        return -1;
    }

    std::string input = argv[1];
    const char *filename = input.c_str();
    std::cout << ">>> skim.cxx: Process input: " << input << std::endl;

    ROOT::EnableImplicitMT(); // Tell ROOT we want to go parallel
   
    TStopwatch time;  // track how long it took our script to run
    time.Start();

    std::string output = argv[2];
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
    const auto numEvents = *df.Count();
    std::cout << ">>> skim.cxx: Number of events: " << numEvents << std::endl;

    const auto xsec = atof(argv[3]);
    std::cout << ">>> skim.cxx: Cross-section: " << xsec << std::endl;

    const auto lumi = atof(argv[4]);
    std::cout << ">>> skim.cxx: Integrated luminosity: " << lumi << std::endl;

    ////////// Event selection
    auto df2 = AddEventWeight(df, numEvents, xsec, lumi);
    auto df3 = ApplyLooseSelection(df2); 
    auto df4 = SelectLeadingPair(df3);
    auto df5 = GetLeadingPairInfo(df4);
    auto df6 = ApplyTrigger(df5);
    auto dfFinal = df6;

    // Save output n-tuple
    std::vector<std::string> finalVariables = {"weight", "pt_1", "eta_1", "phi_1", "m_1", "q_1", "pt_2", "eta_2", "phi_2", "m_2", "q_2", "m_vis", "pt_vis", "mt_1", "mt_2"};
    dfFinal.Snapshot("event_tree", output, finalVariables);

    // Print the cutflow report
    auto report = dfFinal.Report();
    report->Print();

    time.Stop();
    time.Print();

    return 0;
}
