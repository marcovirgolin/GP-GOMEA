/*
 


 */

/* 
 * File:   main.cpp
 * Author: virgolin
 *
 * Created on June 27, 2018, 11:56 AM
 */

#include <cstdlib>

#include <iostream>
#include <armadillo>

#include "GPGOMEA/Evolution/EvolutionState.h"
#include "GPGOMEA/Evolution/EvolutionRun.h"
#include "GPGOMEA/RunHandling/IMSHandler.h"


using namespace std;

int main(int argc, char** argv) {

    EvolutionState * st = new EvolutionState();

    st->SetOptions(argc, argv);

    IMSHandler * imsh = new IMSHandler(st);
    imsh->Start();
    delete imsh;

    delete st;

    return 0;
}

