.PHONY: all

all: imagehash

README.md: imagehash README.md.in
	cat README.md.in | regexec -e "HELP_OUTPUT" -c "./imagehash -h" -n 1 > README.md

imagehash: main.cc
	g++ -g -std=c++11 -Wall -fPIC -o imagehash main.cc -I. -lboost_program_options -lpHash
