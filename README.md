# Demo DY to TauTau analysis

## Introduction

This is a minimal working example of a SM DY to tautau cross section measurement analysis using the C++ RDataFrame framework. The structure of the code follows [demo-luna-framework](https://gitlab.cern.ch/skkwan/demo-luna-framework) and [Awesome-Workshop](https://awesome-workshop.github.io/awesome-htautau-analysis/05-plot/index.html). 

## Install

On `lxplus` (`lxplus9`, the default as of December 2023): 
```bash
cmsrel CMSSW_13_1_0_pre4
cd src
git clone git@github.com:pallabidas/demo-DYtoTauTau.git
cd demo-DYtoTauTau
```

## To run the example
The current skimming example is in the `skim/` directory:
```bash
cmsenv
voms-proxy-init --voms cms 
cd skim/
bash runSkim.sh
```

This compiles `skim.cxx` into an executable `skim` and calls it on a remotely-located input NanoAOD file, and outputs a file to the current directory.
The `run_interactive.sh` script can be used to run interactively with MC sample list and cross section in pb. Right now the integrated luminosity is hard-coded to be 3274.8 pb-1 in this script. After running over multiple files in a list, they should be added together using `hadd` command, per sample.

## Getting histograms
```
cd ../plot/
sh histograms.sh ../skim/output .
sh plot.sh histograms.root .
```

## Finding the DY signal strength
Using Roofit package:
```
cd ../fit/
sh fit.sh ../plot/histograms.root .
```
Using Combine tool:
```
combine -M FitDiagnostics MuTau.txt --justFit
text2workspace.py MuTau.txt 
combine MuTau.root -M MultiDimFit --saveWorkspace -n Postfit
combine higgsCombinePostfit.MultiDimFit.mH120.root -M MultiDimFit -n Total --algo grid --snapshotName MultiDimFit --setParameterRanges r=0,4
mv higgsCombinepostfit.MultiDimFit.mH120.root higgsCombinePostfit.MultiDimFit.mH120.root
plot1DScan.py  higgsCombineTotal.MultiDimFit.mH120.root --main-label "Total Uncert."
```
