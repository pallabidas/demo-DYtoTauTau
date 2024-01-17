# Demo LUNA framework

## Introduction

This is meant to be a minimal working example of the C++ RDataFrame framework developed for the `h->aa->bbtautau` analysis.

Current features:
- Instructions to run in a CMSSW environment, tested on lxplus9. There is no `cmsRun` command here, we just use CMSSW for the environment.



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