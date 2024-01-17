# Demo LUNA framework

## Introduction

This is meant to be a minimal working example of the C++ RDataFrame framework developed for the `h->aa->bbtautau` analysis which can be found [here](https://gitlab.cern.ch/skkwan/lunaFramework). 

Current features:
- Instructions to run in a CMSSW environment, tested on lxplus9. There is no `cmsRun` command here, we just use CMSSW for the environment.
- Basic skimming code, which takes NanoAOD as input, performs an example basic event selection, computes a few kinematic variables, and saves the result as a TTree in a local output file.

Other features in the [full "LUNA" framework](https://gitlab.cern.ch/skkwan/lunaFramework) - please reach out if you'd like to discuss any of these:
- More complex functions, e.g. ones that access external ROOT files (RooWorkspaces, TH1Fs).
- Wrappers to run on many NanoAOD datasets at once.
- C++ RDataFrame code to produce histograms.
- Systematics handling.
- Data/MC plotting scripts in Python.
- Condor jobs and monitoring.

## Install

On `lxplus` (`lxplus9`, the default as of December 2023): 
```bash
cmsrel CMSSW_13_1_0_pre4
cd src
git clone ssh://git@gitlab.cern.ch:7999/skkwan/demo-luna-framework.git
cd demo-luna-framework
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