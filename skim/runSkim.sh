# Usage:
# bash runSkim.sh

#--------------------------------------------------------
# Check if we used bash
#--------------------------------------------------------
if [[ "$0" != "$BASH_SOURCE" ]]; then
    echo ">>> ${BASH_SOURCE[0]}: Error: Script must be run with bash"
    return
fi


#--------------------------------------------------------
# Compile
#--------------------------------------------------------
COMPILER=$(root-config --cxx)
FLAGS=$(root-config --cflags --libs)
time $COMPILER -g -O3 -Wall -Wextra -Wpedantic -o skim skim.cxx $FLAGS 

if [[ $? -ne 0 ]]; then
    echo ">>> Compile failed, exit"
    exit 1
fi


#--------------------------------------------------------
# Execute
#--------------------------------------------------------
./skim root://cmsxrootd.fnal.gov///store/mc/RunIISummer20UL18NanoAODv9/VBFHToTauTau_M125_TuneCP5_13TeV-powheg-pythia8/NANOAODSIM/106X_upgrade2018_realistic_v16_L1v1-v1/130000/5BD57963-BF8F-9A48-854D-E344A5F555BB.root out.root 1.55 138
