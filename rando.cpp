//
// Created by matt on 4/3/17.
//
#include "rando.hpp"

std::random_device rando::rd;
std::mt19937 rando::gen;
std::weibull_distribution<float> rando::d;
std::uniform_real_distribution<float> rando::u;
std::uniform_real_distribution<float> rando::out;