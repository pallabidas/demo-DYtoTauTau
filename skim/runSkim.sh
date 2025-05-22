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
./skim /hdfs/store/data/Run2022G/Muon/NANOAOD/22Sep2023-v1/30000/90390f8e-cb55-4507-8106-6e134d7fa259.root Muon_Skim.root 1.0 3274.8 
