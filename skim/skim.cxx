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
double SumGenEventSumw(T &df) {
  double genEventSumw = 1.0;
  auto sumw = df.Sum("genWeight");
  genEventSumw = (double) *sumw;
  return genEventSumw;
}

template <typename T>
auto AddEventWeight(T &df, const float xsec) {
    if (xsec == 1.0) {
       return df.Define("genWeight", [](){ return (float) 1.0; }, {}); // For data, create branch
    }
    return df.Redefine("genWeight", "genWeight"); // For MC, copy branch
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
    TChain *chRuns = new TChain("Runs");
    int result = ch->Add(filename);
    int resultRuns = chRuns->Add(filename);
    if (result == 0 || resultRuns == 0) {
	std::cout << ">>> [ERROR:] skim.cxx: Failed to find file Events/Runs TTree in the input NanoAOD file!" << std::endl;
        return 1;
    }
    else {
        std::cout << ">>> skim.cxx: Added " << filename << " Events and Runs TTree" << std::endl;
    }

    // Create the RDataFrame object 
    ROOT::RDataFrame df(*ch);
    const auto numEvents = *df.Count();
    std::cout << ">>> skim.cxx: Number of events: " << numEvents << std::endl;

    const auto xsec = atof(argv[3]);
    std::cout << ">>> skim.cxx: Cross-section: " << xsec << std::endl;
    bool isData = false;
    if (xsec == 1.0) isData = true;

    const auto lumi = atof(argv[4]);
    std::cout << ">>> skim.cxx: Integrated luminosity: " << lumi << std::endl;

    ROOT::RDataFrame dfRuns(*chRuns);
    float genEventSumw  = 1.0;
    if (!isData) genEventSumw = SumGenEventSumw(df);
    std::cout << ">>> skim.cxx: genEventSumw " << genEventSumw << std::endl;

    ROOT::RDF::RSnapshotOptions opts;
    opts.fMode = "RECREATE";

    ROOT::RDataFrame df_sumw(1);  // 1-row dummy dataframe
    auto df_sumw2 = df_sumw.Define("genWeightSumw", [=](){ return (float) genEventSumw; })
	                   .Define("xsec", [=](){ return (float) xsec; })
			   .Define("lumi", [=](){ return (float) lumi; });
    df_sumw2.Snapshot("sumw_tree", output, {"genWeightSumw", "xsec", "lumi"}, opts);

    ////////// Event selection
    auto df2 = AddEventWeight(df, xsec);
    auto df3 = ApplyLooseSelection(df2); 
    auto df4 = SelectLeadingPair(df3);
    auto df5 = GetLeadingPairInfo(df4);
    auto df6 = CheckGeneratorTaus(df5, isData);
    auto df7 = ApplyTrigger(df6);
    auto dfFinal = df7;

    // Save output n-tuple
    std::vector<std::string> finalVariables = {"genWeight", "pt_1", "eta_1", "phi_1", "m_1", "q_1", "pt_2", "eta_2", "phi_2", "m_2", "q_2", "gen_match", "m_vis", "pt_vis", "mt_1", "mt_2"};
    opts.fMode = "UPDATE";
    dfFinal.Snapshot("event_tree", output, finalVariables, opts);

    // Print the cutflow report
    auto report = dfFinal.Report();
    report->Print();

    time.Stop();
    time.Print();

    return 0;
}
