# Implementation of the histogramming step of the analysis
#
# The histogramming step produces histograms for each variable in the dataset
# and for each physics process resulting into the final state with a muon and a
# tau. Then, the resulting histograms are passed to the plotting step, which
# combines the histograms so that we can study the physics of the decay.


import argparse
import ROOT
ROOT.gROOT.SetBatch(True)


# Declare the range of the histogram for each variable
#
# Each entry in the dictionary contains of the variable name as key and a tuple
# specifying the histogram layout as value. The tuple sets the number of bins,
# the lower edge and the upper edge of the histogram.
default_nbins = 15
ranges = {
        "pt_1": (default_nbins, 20, 70),
        "pt_2": (default_nbins, 20, 70),
        "eta_1": (default_nbins, -2.4, 2.4),
        "eta_2": (default_nbins, -2.3, 2.3),
        "phi_1": (default_nbins, -3.14, 3.14),
        "phi_2": (default_nbins, -3.14, 3.14),
        "q_1": (2, -2, 2),
        "q_2": (2, -2, 2),
        "m_1": (default_nbins, 0, 0.2),
        "m_2": (default_nbins, 0, 2),
        "mt_1": (default_nbins, 0, 50),
        "mt_2": (default_nbins, 0, 100),
        "m_vis": (default_nbins, 20, 140),
        "pt_vis": (default_nbins, 0, 60),
        }


# Book a histogram for a specific variable each entry multiplied by genWeight
def bookHistogram(df, variable, range_):
    return df.Histo1D(ROOT.ROOT.RDF.TH1DModel(variable, variable, range_[0], range_[1], range_[2]),\
                      variable, "genWeight")


# Write a histogram with a given name to the output ROOT file
def writeHistogram(h, name, scale):
    h.Scale(scale)
    h.SetName(name)
    h.Write()


# Apply a selection based on generator information about the tau
#
# See the skimming step for further details about this variable.
def filterGenMatch(df, label):
    if label == "ZTT":
        return df.Filter("gen_match", "Select genuine taus")
    elif label == "ZLL":
        return df.Filter("!gen_match", "Select fake taus")
    else:
        return df


# Main function of the histogramming step
#
# The function loops over the outputs from the skimming step and produces the
# required histograms for the final plotting.
# Note that we perform a set of secondary selections on the skimmed dataset. First,
# we perform a second reduction with the baseline selection to a signal-enriched
# part of the dataset. Second, we select besides the signal region a control region
# which is used to estimate the contribution of QCD events producing the muon-tau
# pair in the final state.
def main(sample, process, output):
    # Create output file
    tfile = ROOT.TFile(output, "RECREATE")
    variables = ranges.keys()

    # Process skimmed datasets and produce histograms of variables
    print(">>> Process skimmed sample {} for process {}".format(sample, process))

    # Load skimmed dataset and apply baseline selection
    df = ROOT.ROOT.RDataFrame("event_tree", sample)\
                  .Filter("mt_1<30", "Muon transverse mass cut for W+jets suppression")
    df = filterGenMatch(df, process)

    dfSum = ROOT.ROOT.RDataFrame("sumw_tree", sample)
    sumweight = dfSum.Sum("genWeightSumw").GetValue()
    branch_lumi = dfSum.Take["float"]("lumi").GetValue()
    branch_xsec = dfSum.Take["float"]("xsec").GetValue()

    # Scale MC histograms to luminosity using sun of genWeight
    hist_scale = 1.0
    if (process != "2022G"):
        hist_scale *= (branch_lumi[0] * branch_xsec[0])/sumweight 

    # Book histograms for the signal region
    df1 = df.Filter("q_1*q_2<0", "Require opposited charge for signal region")
    hists = {}
    for variable in variables:
        hists[variable] = bookHistogram(df1, variable, ranges[variable])
    report1 = df1.Report()

    # Book histograms for the control region used to estimate the QCD contribution
    df2 = df.Filter("q_1*q_2>0", "Control region for QCD estimation")
    hists_cr = {}
    for variable in variables:
        hists_cr[variable] = bookHistogram(df2, variable, ranges[variable])
    report2 = df2.Report()

    hists = {}
    for variable in variables:
        hists[variable] = bookHistogram(df, variable, ranges[variable])
    report1 = df.Report()

    # Write histograms to output file
    for variable in variables:
        writeHistogram(hists[variable], "{}_{}".format(process, variable), hist_scale)
    for variable in variables:
        writeHistogram(hists_cr[variable], "{}_{}_cr".format(process, variable), hist_scale)

    # Print cut-flow report
    print("Cut-flow report (signal region):")
    report1.Print()
    print("Cut-flow report (control region):")
    report2.Print()

    tfile.Close()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("sample", type=str, help="Full path to skimmed sample")
    parser.add_argument("process", type=str, help="Process name")
    parser.add_argument("output", type=str, help="Output file with histograms")
    args = parser.parse_args()
    main(args.sample, args.process, args.output)
