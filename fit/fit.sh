#!/bin/bash

HISTOGRAMS=$1
OUTPUT=$2

python3 fit.py $HISTOGRAMS $OUTPUT
