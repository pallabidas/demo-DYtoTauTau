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
./skim