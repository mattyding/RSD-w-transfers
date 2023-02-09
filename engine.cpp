#include "engine.h"

using namespace std;

int main() {
    int numRooms = 10;
    int numAgents = 10;
    bool verbose = true;

    int num_trials = 20;
    int running_total = 0;
    

    for (int a=0; a < num_trials; a++) {

        Matching m(numRooms);
        AgentArray ppl(numAgents, numRooms);
        vector<int> order = gen_ordering(numAgents);
        
        // Randomized Serial Dictatorship
        for (int i = 0; i < numAgents; i++) {
            int pick = order[i];
            for (int j = 0; j < numRooms; j++) {
                int room = ppl.agents[pick].preferences[j];
                if (m.assignments[room] == -1) {
                    m.assignments[room] = pick;
                    break;
                } 
            }
        }
        if (verbose) {cout << "RSD welfare: " << ppl.computeTotalWelfare(m) << "   ";}
        int temp = ppl.computeTotalWelfare(m);
        // with transfers!
        set<tuple<int, int>> transfers;
        bool not_converged = true;
        while (not_converged) {
            not_converged = false;
            for (int i = 0; i < numAgents; i++) {
                int i_assign = m.getAssignment(i);
                int i_room_ranking = ppl.agents[i].getPreferenceRank(i_assign);
                int i_valuation = ppl.agents[i].valuations[i_assign];
                for (int j = 0; j < i_room_ranking; j++) {
                    int pref_room = ppl.agents[i].preferences[j];
                    int k_assign = m.assignments[pref_room];
                    int k_valuation = ppl.agents[k_assign].valuations[pref_room];
                    // check if transfer has already been made
                    if (transfers.find(tuple<int, int>(i, k_assign)) != transfers.end()) {
                        continue;
                    }
                    // i prefers pref_room AND is willing to pay for it
                    if (ppl.agents[i].budget > k_valuation - ppl.agents[k_assign].valuations[i_assign]) {
                        m.assignments[i_assign] = k_assign;
                        m.assignments[pref_room] = i;
                        ppl.agents[i].budget += k_valuation - i_valuation;
                        not_converged = true;
                        transfers.insert(tuple<int, int>(i, k_assign));
                        transfers.insert(tuple<int, int>(k_assign, i));
                        // cout << "Transfer from " << k_assign << " to " << i << " for room " << pref_room << endl;
                        break;
                    }
                }
            }   
        }
        // print results
        cout << "welfare after swaps: " << ppl.computeTotalWelfare(m) << endl;
        running_total += ppl.computeTotalWelfare(m) - temp;
    }

    if (verbose) {cout << "Average welfare change: " << running_total / num_trials << endl;}

    return 0;
}

vector<int> gen_ordering(int num_agents) {
    vector<int> order(num_agents);
    iota(order.begin(), order.end(), 0);
    random_device rd;
    mt19937 g(rd());
    shuffle(order.begin(), order.end(), g);
    return order;
}