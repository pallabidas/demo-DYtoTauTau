# Demo LUNA framework

## Introduction

This is meant to be a minimal working example of the C++ RDataFrame framework developed for the `h->aa->bbtautau` analysis. 

Current features:
- Instructions to run in a virtual environment inside a CMSSW environment. There is no `cmsRun` command here, we just use CMSSW for the environment. 



## Setup (do only once)

On `lxplus` (`lxplus9`, the default as of December 2023), any recent `CMSSW` should work, we do not have any `cmsRun` configs actually.
```bash
cmsrel CMSSW_13_1_0_pre4
cd src

cd demoLunaFramework
cmsenv
```

Next, we make a virtual environment: 

```bash
cmsenv
virtualenv venv-LUNA-demo
source venv-LUNA-demo/bin/activate
(venv-LUNA-demo) $ pip install -r environment-LUNA-requirements.txt
```

## At the start of each session
```bash
cmsenv
source venv-LUNA-demo/bin/activate
# and get voms-proxy-init certificate too
```