# Implementation of the fitting step of the analysis
#
# In the fit we treat ZTT as signal and fit the signal strength given statistical
# uncertainties of the observation and normalization uncertainties for each process.

import argparse
import ROOT
ROOT.gROOT.SetBatch(True)
import os

def main(path, output):
    # Read and preprocess histograms for fitting
    variable = "m_vis"
    h = ROOT.TFile(path, "READ")

    # Data
    data = h.Get("2022G_{}".format(variable))

    # Backgrounds
    ztt = h.Get("ZTT_{}".format(variable))
    zll = h.Get("ZLL_{}".format(variable))
    tt = h.Get("TT_{}".format(variable))
    w = h.Get("W_{}".format(variable))

    # QCD estimate from same-sign region
    data_cr = h.Get("2022G_{}_cr".format(variable))

    for label in ["ZTT", "ZLL", "TT", "W"]:
        hist = h.Get("{}_{}_cr".format(label, variable))
        data_cr.Add(hist, -1.0)
    qcd = data_cr
    QCDScaleFactor = 0.80
    qcd.Scale(QCDScaleFactor)

    # Write all histograms used for fitting in a new file
    inputfile = os.path.join(output, "fit_inputs.root")
    f = ROOT.TFile(inputfile, "RECREATE")
    for hist, label in [(ztt, "ZTT"), (zll, "ZLL"), (tt, "TT"), (w, "W"), (qcd, "QCD"), (data, "2022G")]:
        hist.SetName(label)
        hist.Write(label, ROOT.TObject.kOverwrite)
    f.Close()
    h.Close()

    # Set up the uncertainty model using the HistFactory
    meas = ROOT.RooStats.HistFactory.Measurement("measurement", "measurement")
    meas.SetOutputFilePrefix("fit/ZTT_mu")
    meas.SetPOI("ZTT_mu")

    meas.SetLumi(1.0)
    meas.SetLumiRelErr(0.01)
    meas.SetExportOnly(False)

    channel = ROOT.RooStats.HistFactory.Channel("channel")
    channel.SetData("2022G", inputfile)
    channel.SetStatErrorConfig(0.1, "Poisson")

    sample = ROOT.RooStats.HistFactory.Sample("ZTT", "ZTT", inputfile)
    sample.AddOverallSys("ZTT_xsec_sys",  0.9, 1.1)
    sample.AddNormFactor("ZTT_mu", 1, 0, 2)
    channel.AddSample(sample)

    for process in ["ZLL", "W", "TT", "QCD"]:
        sample = ROOT.RooStats.HistFactory.Sample(process, process, inputfile)
        sample.AddOverallSys(process + "_xsec_sys",  0.7, 1.3)
        channel.AddSample(sample)

    meas.AddChannel(channel)
    meas.CollectHistograms()
    meas.PrintTree()

    # Create a RooFit workspace
    hist2workspace = ROOT.RooStats.HistFactory.HistoToWorkspaceFactoryFast(meas)
    workspace = hist2workspace.MakeCombinedModel(meas)

    # Make a profile likelihood with plot for the ZTT signal strength
    model = workspace.obj("ModelConfig")
    data = workspace.data("obsData")
    poi = workspace.var("ZTT_mu")
    pl = ROOT.RooStats.ProfileLikelihoodCalculator(data, model)
    pl.SetConfidenceLevel(0.68)
    interval = pl.GetInterval()

    bestfit_params = interval.GetBestFitParameters()
    lower_limit = interval.LowerLimit(poi)
    upper_limit = interval.UpperLimit(poi)
    bestfit_ZTT_mu = bestfit_params["ZTT_mu"].getValV()
    title = "ZTT_mu: {:.3f} +{:.3f} -{:.3f}".format(
        bestfit_ZTT_mu, upper_limit - bestfit_ZTT_mu, bestfit_ZTT_mu - lower_limit)

    plot = ROOT.RooStats.LikelihoodIntervalPlot(interval)
    plot.SetNPoints(50)
    plot.SetMaximum(1.0)
    plot.SetRange(0.75, 1.10)
    plot.SetTitle(title)
    c = ROOT.TCanvas("", "", 600, 600)
    plot.Draw()
    c.SaveAs(os.path.join(output, "fit.png"))
    c.SaveAs(os.path.join(output, "fit.pdf"))

    # Workaround for a PyROOT bug
    del pl


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("path", type=str, help="Full path to ROOT file with all histograms")
    parser.add_argument("output", type=str, help="Output directory for plots")
    args = parser.parse_args()
    main(args.path, args.output)
