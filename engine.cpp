#include "engine.h"

using namespace std;

int main() {
    int numRooms = 10;
    int numAgents = 10;
    int friction = 100; // friction
    bool verbose = false;

    int num_trials = 1; // for averaging (1 for single run)
    int running_total = 0;

    for (int a=0; a < num_trials; a++) {
        RoomArray rooms(numRooms);
        AgentArray agents(numAgents, rooms);
        vector<int> order = gen_ordering(numAgents);

        // Random Serial Dictatorship
        rooms.matching = run_random_serial_dictatorship(agents, rooms, order, verbose);
        cout << "Total utility pre-swap: " << agents.computeTotalWelfare(rooms.matching) << endl;

        // with transfers!
        rooms.matching = run_transfers(agents, rooms.matching, order, friction, verbose);
        cout << "Total utility post-swap: " << agents.computeTotalWelfare(rooms.matching) << endl;
    }
}

Matching run_random_serial_dictatorship(AgentArray &agents, RoomArray &rooms, vector<int> &order, bool verbose) {
    // Runs RSD for an ordering and returns the matching
    Matching m = Matching(rooms.numRooms);
    for (int i = 0; i < order.size(); i++) {
        int pick = order[i];
        for (int j = 0; j < rooms.numRooms; j++) {
            int room = agents.agents[pick].preferences[j];
            if (m.assignments[room] == -1) {
                m.assignments[room] = pick;
                break;
            }
        }
    }
    return m;
}

Matching run_transfers(AgentArray &agents, Matching &m, vector<int> &order, int friction, bool verbose) {
    Matching post_transfer_matching = m.copy();
    set<int> already_traded = set<int>(); // agents that already initiated a trade no longer trade
    for (int i = 0; i < agents.numAgents; i++) {
        int orig = order[i];
        int orig_room = post_transfer_matching.getAssignmentForAgent(orig);
        int orig_valuation = agents.agents[orig].valuations[orig_room];
        // agents find the room they prefer the most that they can afford
        int highest_swap_utility = -1;
        int new_room = -1;
        int other_owner = -1;
        int transaction_price = -1;
        // iterate through all rooms that orig would prefer more
        for (int j = 0; j < m.assignments.size(); j++) {
            int pref_room = agents.agents[orig].preferences[j];
            if (pref_room == orig_room) {
                break;
            }
            int pref_assign = post_transfer_matching.assignments[pref_room];
            // cout << "pref_assign: " << pref_assign << " pref_room: " << pref_room << endl;
            if (already_traded.find(pref_assign) != already_traded.end()) continue; // already traded
            // difference in valuation for the other person
            int offer_price = agents.agents[pref_assign].valuations[pref_room] - agents.agents[pref_assign].valuations[orig_room];
            if (agents.agents[orig].budget < offer_price) continue; // out of budget
            // quasi-linear utility (TODO: extend to other utility fns)
            int orig_utility_from_trade = agents.agents[orig].valuations[pref_room] - agents.agents[orig].valuations[orig_room] - offer_price;
            if (orig_utility_from_trade < 0) continue; // not worth it
            if (orig_utility_from_trade > highest_swap_utility) {
                highest_swap_utility = orig_utility_from_trade;
                new_room = pref_room;
                other_owner = pref_assign;
                transaction_price = offer_price;
            }
        }
        if (new_room != -1) {
            // swap rooms!
            post_transfer_matching.assignments[orig_room] = other_owner;
            post_transfer_matching.assignments[new_room] = orig;
            agents.agents[orig].budget -= transaction_price;
            agents.agents[other_owner].budget += transaction_price;
            already_traded.insert(orig);
            if (verbose) {
                cout << orig << " bought room " << new_room << " from " << other_owner << " for " << transaction_price << ". " << other_owner << " got room " << orig_room << " in return." << endl;
            }
        }
    }
    return post_transfer_matching;
}

vector<int> gen_ordering(int num_agents) {
    // generates a random ordering of the agents
    vector<int> order(num_agents);
    iota(order.begin(), order.end(), 0);
    random_device rd;
    mt19937 g(rd());
    shuffle(order.begin(), order.end(), g);
    return order;
}