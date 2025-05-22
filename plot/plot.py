# Implementation of the plotting step of the analysis
#
# The plotting combines the histograms to plots which allow us to study the
# inital dataset based on observables motivated through physics.


import argparse
import ROOT
ROOT.gROOT.SetBatch(True)


# Declare a human-readable label for each variable
labels = {
        "pt_1": "Muon p_{T} / GeV",
        "pt_2": "Tau p_{T} / GeV",
        "eta_1": "Muon #eta",
        "eta_2": "Tau #eta",
        "phi_1": "Muon #phi",
        "phi_2": "Tau #phi",
        "q_1": "Muon charge",
        "q_2": "Tau charge",
        "m_1": "Muon mass / GeV",
        "m_2": "Tau mass / GeV",
        "mt_1": "Muon transverse mass / GeV",
        "mt_2": "Tau transverse mass / GeV",
        "m_vis": "Visible di-tau mass / GeV",
        "pt_vis": "Visible di-tau p_{T} / GeV",
        }


# Specify the color for each process
colors = {
        "TT": ROOT.TColor.GetColor(155, 152, 204),
        "W": ROOT.TColor.GetColor(222, 90, 106),
        "QCD":  ROOT.TColor.GetColor(250, 202, 255),
        "ZLL": ROOT.TColor.GetColor(100, 192, 232),
        "ZTT": ROOT.TColor.GetColor(248, 206, 104),
        }


# Retrieve a histogram from the input file based on the process and the variable
# name
def getHistogram(tfile, name, variable, tag=""):
    name = "{}_{}{}".format(name, variable, tag)
    h = tfile.Get(name)
    if not h:
        raise Exception("Failed to load histogram {}.".format(name))
    return h


# Main function of the plotting step
#
# The major part of the code below is dedicated to define a nice-looking layout.
# The interesting part is the combination of the histograms to the QCD estimation.
# There, we take the data histogram from the control region and subtract all known
# processes defined in simulation and define the remaining part as QCD. Then,
# this shape is extrapolated into the signal region with a scale factor.
def main(path, output, variable):
    tfile = ROOT.TFile(path, "READ")

    # Styles
    ROOT.gStyle.SetOptStat(0)

    ROOT.gStyle.SetCanvasBorderMode(0)
    ROOT.gStyle.SetCanvasColor(ROOT.kWhite)
    ROOT.gStyle.SetCanvasDefH(600)
    ROOT.gStyle.SetCanvasDefW(600)
    ROOT.gStyle.SetCanvasDefX(0)
    ROOT.gStyle.SetCanvasDefY(0)

    ROOT.gStyle.SetPadTopMargin(0.08)
    ROOT.gStyle.SetPadBottomMargin(0.13)
    ROOT.gStyle.SetPadLeftMargin(0.16)
    ROOT.gStyle.SetPadRightMargin(0.05)

    ROOT.gStyle.SetHistLineColor(1)
    ROOT.gStyle.SetHistLineStyle(0)
    ROOT.gStyle.SetHistLineWidth(1)
    ROOT.gStyle.SetEndErrorSize(2)
    ROOT.gStyle.SetMarkerStyle(20)

    ROOT.gStyle.SetOptTitle(0)
    ROOT.gStyle.SetTitleFont(42)
    ROOT.gStyle.SetTitleColor(1)
    ROOT.gStyle.SetTitleTextColor(1)
    ROOT.gStyle.SetTitleFillColor(10)
    ROOT.gStyle.SetTitleFontSize(0.05)

    ROOT.gStyle.SetTitleColor(1, "XYZ")
    ROOT.gStyle.SetTitleFont(42, "XYZ")
    ROOT.gStyle.SetTitleSize(0.05, "XYZ")
    ROOT.gStyle.SetTitleXOffset(1.00)
    ROOT.gStyle.SetTitleYOffset(1.60)

    ROOT.gStyle.SetLabelColor(1, "XYZ")
    ROOT.gStyle.SetLabelFont(42, "XYZ")
    ROOT.gStyle.SetLabelOffset(0.007, "XYZ")
    ROOT.gStyle.SetLabelSize(0.04, "XYZ")

    ROOT.gStyle.SetAxisColor(1, "XYZ")
    ROOT.gStyle.SetStripDecimals(True)
    ROOT.gStyle.SetTickLength(0.03, "XYZ")
    ROOT.gStyle.SetNdivisions(510, "XYZ")
    ROOT.gStyle.SetPadTickX(1)
    ROOT.gStyle.SetPadTickY(1)

    ROOT.gStyle.SetPaperSize(20., 20.)
    ROOT.gStyle.SetHatchesLineWidth(5)
    ROOT.gStyle.SetHatchesSpacing(0.05)

    ROOT.TGaxis.SetExponentOffset(-0.08, 0.01, "Y")

    # Simulation

    W = getHistogram(tfile, "W", variable)
    TT = getHistogram(tfile, "TT", variable)
    ZLL = getHistogram(tfile, "ZLL", variable)
    ZTT = getHistogram(tfile, "ZTT", variable)

    # Data
    data = getHistogram(tfile, "2022G", variable)

    # Data-driven QCD estimation
    QCD = getHistogram(tfile, "2022G", variable, "_cr")

    for name in ["ZLL", "ZTT", "W", "TT"]:
        ss = getHistogram(tfile, name, variable, "_cr")
        QCD.Add(ss, -1.0)
    for i in range(1, QCD.GetNbinsX() + 1):
        if QCD.GetBinContent(i) < 0.0:
            QCD.SetBinContent(i, 0.0)
    QCDScaleFactor = 0.80
    QCD.Scale(QCDScaleFactor)

    # Draw histograms
    data.SetMarkerStyle(20)
    data.SetLineColor(ROOT.kBlack)

    for x, l in [(QCD, "QCD"), (ZLL, "ZLL"), (ZTT, "ZTT"), (W, "W"), (TT, "TT")]:
        x.SetLineWidth(0)
        x.SetFillColor(colors[l])

    stack = ROOT.THStack("", "")
    for x in [QCD, TT, W, ZLL, ZTT]:
        stack.Add(x)

    c = ROOT.TCanvas("", "", 600, 600)
    stack.Draw("hist")
    name = data.GetTitle()
    if name in labels:
        title = labels[name]
    else:
        title = name
    stack.GetXaxis().SetTitle(title)
    stack.GetYaxis().SetTitle("N_{Events}")
    stack.SetMaximum(max(stack.GetMaximum(), data.GetMaximum()) * 1.4)
    stack.SetMinimum(1.0)

    data.Draw("E1P SAME")

    # Add legend
    legend = ROOT.TLegend(0.4, 0.73, 0.90, 0.88)
    legend.SetNColumns(2)
    legend.AddEntry(ZTT, "Z#rightarrow#tau#tau", "f")
    legend.AddEntry(ZLL, "Z#rightarrowll", "f")
    legend.AddEntry(W, "W+jets", "f")
    legend.AddEntry(TT, "t#bar{t}", "f")
    legend.AddEntry(QCD, "QCD multijet", "f")
    legend.AddEntry(data, "Data", "lep")
    legend.SetBorderSize(0)
    legend.Draw()

    # Add title
    latex = ROOT.TLatex()
    latex.SetNDC()
    latex.SetTextSize(0.04)
    latex.SetTextFont(42)
    lumi = 3.2748
    latex.DrawLatex(0.55, 0.935, "{:.1f} fb^{{-1}} (2022G, 13.6 TeV)".format(lumi))
    latex.DrawLatex(0.16, 0.935, "#bf{CMS Preliminary}")

    # Save
    c.SaveAs("{}/{}.pdf".format(output, variable))
    c.SaveAs("{}/{}.png".format(output, variable))


# Loop over all variable names and make a plot for each
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("path", type=str, help="Full path to ROOT file with all histograms")
    parser.add_argument("output", type=str, help="Output directory for plots")
    args = parser.parse_args()
    for variable in labels.keys():
        main(args.path, args.output, variable)
