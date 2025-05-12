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
