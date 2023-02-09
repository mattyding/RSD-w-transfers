/* File: engine.h
 * -------------
 * This file defines the Engine class, which is responsible for
 * running the simulation and coordinating the interactions between
 * the agents and the environment.
 */
#ifndef _engine_h
#define _engine_h

#include <cstdlib>
#include <iostream>
#include <random>
#include <set>
#include <tuple>
#include "agent.h"
#include "matching.h"

int main();

std::vector<int> gen_ordering(int numAgents);

#endif

