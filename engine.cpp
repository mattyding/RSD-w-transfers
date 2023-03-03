#include "engine.h"

using namespace std;

int main() {
    int numRooms = 1000;
    int numAgents = 1000;
    int friction_factor = 1;
    bool verbose = false;
    int num_trials = 20;

    RoomArray rooms(numRooms);
    AgentArray agents(numAgents, rooms);

    print_title();
    cout << "Number of rooms: " << numRooms << endl;
    cout << "Number of agents: " << numAgents << endl << endl;

    print_line();
    cout << "Experiment 1: Utility with and without transfers" << endl;
    print_line();
    cout << "Running " << num_trials << " trials and averaging the results..." << endl;
    int total_utility_pre_swap = 0;
    int total_utility_post_swap = 0;
    int total_utility_post_swap_no_friction = 0;
    for (int trial = 0; trial < num_trials; trial++) {
        vector<int> order = gen_ordering(numAgents);
        // Random Serial Dictatorship
        Matching rsd_matching = run_random_serial_dictatorship(agents.preferences, order);
        total_utility_pre_swap += agents.computeTotalWelfare(rsd_matching);

        // with transfers!
        AgentArray agents_transfer_with_friction = agents.copy();
        Matching post_transfer_matching = run_transfers(agents_transfer_with_friction, rsd_matching, order, friction_factor, verbose);
        total_utility_post_swap += agents.computeTotalWelfare(post_transfer_matching);
        AgentArray agents_no_friction = agents.copy();
        Matching post_transfer_matching_no_friction = run_transfers(agents_no_friction, rsd_matching, order, 0, verbose);
        total_utility_post_swap_no_friction += agents.computeTotalWelfare(post_transfer_matching_no_friction);
    }
    cout << "Average utility pre-swap: " << total_utility_pre_swap / num_trials << endl;
    cout << "Average utility post-swap with friction: " << total_utility_post_swap / num_trials << endl;
    cout << "Average utility post-swap no friction: " << total_utility_post_swap_no_friction / num_trials << endl;
    print_line();
    cout << "Percentage utility gain from swaps with friction: " << (total_utility_post_swap - total_utility_pre_swap) * 100.0 / total_utility_pre_swap << "%" << endl;
    cout << "Percentage utility gain from swaps no friction: " << (total_utility_post_swap_no_friction - total_utility_pre_swap) * 100.0 / total_utility_pre_swap << "%" << endl;
    print_line();
}


Matching run_random_serial_dictatorship(const vector<vector<int>> &preferences, const vector<int> &order) {
    // Runs RSD for an ordering and returns the matching
    Matching m = Matching(preferences.size());
    for (int i = 0; i < order.size(); i++) {
        int pick = order[i];
        for (int j = 0; j < preferences[pick].size(); j++) {
            int room = preferences[pick][j];
            if (m.assignments[room] == -1) {
                m.assignments[room] = pick;
                break;
            }
        }
    }
    return m;
}

Matching run_transfers(AgentArray &agents, Matching &m, const vector<int> &order, int friction_fac, bool verbose) {
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
            if (already_traded.find(pref_assign) != already_traded.end()) continue; // already traded
            // difference in valuation for the other person
            int offer_price = agents.agents[pref_assign].valuations[pref_room] - agents.agents[pref_assign].valuations[orig_room];
            offer_price = max(offer_price, 0);
            if (agents.agents[orig].budget < offer_price) continue; // out of budget
            // quasi-linear utility (TODO: extend to other utility fns)
            int orig_utility_from_trade = agents.agents[orig].valuations[pref_room] - agents.agents[orig].valuations[orig_room] - offer_price;
            orig_utility_from_trade -= friction_fac * offer_price; // friction
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
            agents.agents[orig].budget -= transaction_price + (friction_fac * transaction_price); // friction
            agents.agents[other_owner].budget += transaction_price;
            already_traded.insert(orig);
            if (verbose) {
                cout << orig << " bought room " << new_room << " from " << other_owner << " for " << transaction_price << ". " << other_owner << " got room " << orig_room << " in return." << endl << "Increase in utility for " << orig << ": " << highest_swap_utility << endl << endl;
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


// PRINTING FUNCTIONS

void print_title() {
    cout << "----------------------------------------------------" << endl;
    cout << "CS269I: Incentives in Computer Science Final Project" << endl;
    cout << "           RSD with Transfers Simulation            " << endl;
    cout << "----------------------------------------------------" << endl;
}

void print_line() {
    cout << "----------------------------------------------------" << endl;
}