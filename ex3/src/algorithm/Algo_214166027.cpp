/**
 * @file Algo_214166027.cpp
 * @brief Implementation file for the Algo_214166027 class.
 */
#include "Algo_214166027.h"

REGISTER_ALGORITHM(Algo_214166027);

//Algo_214166027 is a non deterministic occurrence of CommonAlgorithm, hence is_deterministic field is always false
Algo_214166027::Algo_214166027() : CommonAlgorithm(false){
}

Step Algo_214166027::nextStep(){
    return CommonAlgorithm::nextStep(is_deterministic);
}