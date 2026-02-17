#!/bin/sh
ls *.cpp | entr sh -c 'clear && make debug && ./yote'