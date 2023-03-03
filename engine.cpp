#include "engine.h"

using namespace std;

int main() {
    int numRooms = 1000;
    int numAgents = 1000;
    bool verbose = false;

    RoomArray rooms(numRooms);
    AgentArray agents(numAgents, rooms);

    print_title();
    cout << "Number of rooms: " << numRooms << endl;
    cout << "Number of agents: " << numAgents << endl << endl;

    run_experiment_1(agents, 20, verbose);
    run_experiment_2(agents, 30, verbose);
    run_experiment_3(agents, 10, verbose);
}

void run_experiment_1(AgentArray agents, int num_trials, bool verbose) {
    print_line();
    cout << "Experiment 1: Utility increase from transfers" << endl;
    print_line();
    cout << "Running " << num_trials << " trials and averaging the results..." << endl;
    int total_utility_pre_swap = 0;
    vector<int> total_utility_increases_per_agent = vector<int>(agents.numAgents, 0);
    for (int trial = 0; trial < num_trials; trial++) {
        vector<int> order = gen_ordering(agents.numAgents);
        // Random Serial Dictatorship
        Matching rsd_matching = run_random_serial_dictatorship(agents.preferences, order);

        // with transfers!
        AgentArray agents_copy = agents.copy();
        Matching post_transfer_matching = run_transfers(agents_copy, rsd_matching, order, 0, verbose);
        for (int i = 0; i < agents.numAgents; i++) {
            int pre_swap_utility = agents.valuations[i][rsd_matching.getAssignmentForAgent(i)];
            int post_swap_utility = agents.valuations[i][post_transfer_matching.getAssignmentForAgent(i)];
            total_utility_increases_per_agent[i] += post_swap_utility - pre_swap_utility + agents_copy.agents[i].budget - 10000; // 10k is the initial budget
        }
    }
    for (int i = 0; i < agents.numAgents; i++) {
        total_utility_increases_per_agent[i] /= num_trials;
    }
    ofstream outfile;
    outfile.open("output/experiment_1.csv");
    for (int i = 0; i < agents.numAgents; i++) {
        outfile << total_utility_increases_per_agent[i] << endl;
    }
    cout << "Average utility increases per agent written to file" << endl;
    print_line();
    cout << endl;
}

void run_experiment_2(AgentArray agents, int num_trials, bool verbose) {
    print_line();
    cout << "Experiment 2: RSD with Augmented Preferences" << endl;
    print_line();
    cout << "Running " << num_trials << " trials and averaging the results..." << endl;
    int total_utility_unug_rsd = 0;
    int total_utility_aug_rsd = 0;
    for (int trial = 0; trial < num_trials; trial++) {
        vector<int> order = gen_ordering(agents.numAgents);

        Matching unaug_rsd_matching = run_random_serial_dictatorship(agents.preferences, order);
        vector<int> public_valuations = vector<int>(agents.numRooms, 0);
        for (int i = 0; i < agents.numRooms; i++) {
            public_valuations[i] = agents.agents[unaug_rsd_matching.assignments[i]].valuations[i];
        }
        // augment preferences by taking max of private valuation and average of public+private valuations
        vector<vector<int>> augmented_preferences = vector<vector<int>>(agents.numAgents, vector<int>(agents.numRooms, 0));
        for (int i = 0; i < agents.numAgents; i++) {
            vector<int> augmented_agent_valuations = vector<int>(agents.numRooms, 0);
            for (int j = 0; j < agents.numRooms; j++) {
                augmented_agent_valuations[j] = max(agents.agents[i].valuations[j], (agents.agents[i].valuations[j] + public_valuations[j]) / 2);
            }
            vector<int> augmented_agent_preferences = vector<int>(agents.numRooms, 0);
            iota(augmented_agent_preferences.begin(), augmented_agent_preferences.end(), 0);
            stable_sort(augmented_agent_preferences.begin(), augmented_agent_preferences.end(), [&augmented_agent_valuations](int i, int j) {return augmented_agent_valuations[i] > augmented_agent_valuations[j];});
            augmented_preferences[i] = augmented_agent_preferences;
        }
        Matching aug_rsd_matching = run_random_serial_dictatorship(augmented_preferences, order);

        unaug_rsd_matching = run_transfers(agents, unaug_rsd_matching, order, 0, verbose);
        aug_rsd_matching = run_transfers(agents, aug_rsd_matching, order, 0, verbose);

        total_utility_unug_rsd += agents.computeTotalWelfare(unaug_rsd_matching);
        total_utility_aug_rsd += agents.computeTotalWelfare(aug_rsd_matching);
    }
    cout << "Average utility unaugmented RSD: " << total_utility_unug_rsd / num_trials << endl;
    cout << "Average utility augmented RSD: " << total_utility_aug_rsd / num_trials << endl;
    print_line();
    cout << "Percentage utility gain from augmented RSD: " << (total_utility_aug_rsd - total_utility_unug_rsd) * 100.0 / total_utility_unug_rsd << "%" << endl;
    print_line();
    cout << endl;
}

void run_experiment_3(AgentArray agents, int num_trials, bool verbose) {
    print_line();
    cout << "Experiment 3: Utility increase from transfers, varying friction" << endl;
    print_line();
    cout << "Running " << num_trials << " trials and averaging the results..." << endl;
    vector<double> total_percentage_utility_gains = vector<double>();
    for (int i = 1; i < pow(2, 30); i *= 2) {
        total_percentage_utility_gains.push_back(0);
    }
    for (int trial = 0; trial < num_trials; trial++) {
        vector<int> order = gen_ordering(agents.numAgents);
        // no friction
        Matching rsd_matching = run_random_serial_dictatorship(agents.preferences, order);
        int utility_pre_swap = agents.computeTotalWelfare(rsd_matching);
        // varying the level of friction
        for (int friction_factor = 1; friction_factor < pow(2, 30); friction_factor *= 2) {
            AgentArray agent_copy = agents.copy();
            Matching post_transfer_matching = run_transfers(agent_copy, rsd_matching, order, friction_factor, verbose);
            total_percentage_utility_gains[log2(friction_factor)] += (agent_copy.computeTotalWelfare(post_transfer_matching) - utility_pre_swap) * 100.0 / utility_pre_swap;

        }
    }
    ofstream outfile;
    outfile.open("output/experiment_3.csv");
    for (int i = 0; i < total_percentage_utility_gains.size(); i++) {
        outfile << pow(2, i) << "," << total_percentage_utility_gains[i] / num_trials << endl;
    }
    cout << "Average percentage utility gains written to file" << endl;
    print_line();
    cout << endl;
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

Matching run_transfers(AgentArray &agents, Matching &m, const vector<int> &order, int friction_factor, bool verbose) {
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
            orig_utility_from_trade -= friction_factor * offer_price; // friction
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
            agents.agents[orig].budget -= transaction_price + (friction_factor * transaction_price); // friction
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