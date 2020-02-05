#!/bin/bash

g++ efficiency.C -rpath ${ROOTSYS}/lib `root-config --libs --cflags` -o efficiency