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
#include "rooms.h"

int main();

Matching run_random_serial_dictatorship(AgentArray &agents, RoomArray &rooms, std::vector<int> &order, bool verbose);
Matching run_transfers(AgentArray &agents, Matching &m, std::vector<int> &order, int friction, bool verbose);
std::vector<int> gen_ordering(int numAgents);

#endif

