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

Matching run_random_serial_dictatorship(const std::vector<std::vector<int>> &preferences, const std::vector<int> &order);
Matching run_transfers(AgentArray &agents, Matching &m, const std::vector<int> &order, int friction, bool verbose);
std::vector<int> gen_ordering(int numAgents);

void print_title();
void print_line();

#endif

