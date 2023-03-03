/* File: agent.h
* -------------
* This file defines the Agent class, which is the base class for
* all agents in the simulation.  It is responsible for maintaining
* the agent's state and providing a common interface for all agents.
*/

#ifndef _agent_h
#define _agent_h

#include <algorithm>
#include <functional>
#include <random>
#include <iostream>
#include <numeric>
#include <vector>
#include "rooms.h"

class Agent {
    // CONSTANTS CONTROLLING RANDOM VALUES
    int kBudgetParam = 20;
    int kValuationParam = 100;

    public:
        int id;
        // 50% chance 0; 50% chance in [0, 1000]
        int budget;


        // valuations[k] is the valuation of room k (in dollars)
        // valuations are multiple of 100 and in the range [0, 9900]
        std::vector<int> valuations;
        // preferences[k] is the kth favorite room (ranked by valuation)
        // ties are broken by the lower room number
        std::vector<int> preferences;

        Agent(int id, RoomArray rooms);

        int getValuation(int room);
        int getPreferenceRank(int room);

        void printValuations();
        void printPreferences();
};

class AgentArray {
    public:
        int numAgents;
        std::vector<Agent> agents;

        AgentArray(int numAgents, RoomArray rooms);

        int computeTotalWelfare(Matching &m);

        void printValuations();
        void printPreferences();
        void printResults(Matching &m);

};

#endif