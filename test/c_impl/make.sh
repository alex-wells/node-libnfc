#!/bin/bash

set -e
g++ -std=c++11 -Wall -Os -l nfc -o nfc_test *.cc
