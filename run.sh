#!/bin/bash

NPTS=$1
./genpoints/genpoints ${NPTS:=100} || return 1
. ./gnuplot/visualizacao.sh 