#include "agent.h"

using namespace std;

/* Agent Class Methods */

Agent::Agent(int id, RoomArray rooms) {
    this->id = id;

    random_device rd;
    mt19937 g(rd());

    // IGNORE FOR NOW
    this->budget = 10000;

    this->valuations = vector<int>(rooms.numRooms);
    for (int i = 0; i < rooms.numRooms; i++) {
        int value = rooms.valuationDistributions[i](g);
        this->valuations[i] = (value < 0) ? 0 : value;
    }

    // rank preferences by valuation (tie-breaking by lower room number)
    this->preferences = vector<int>(rooms.numRooms);
    iota(this->preferences.begin(), this->preferences.end(), 0);
    stable_sort(this->preferences.begin(), this->preferences.end(), [this](int i, int j) {
        return this->valuations[i] > this->valuations[j];
    });
};

int Agent::getValuation(int room) {
    return valuations[room];
}

int Agent::getPreferenceRank(int room) {
    for (int i = 0; i < sizeof(preferences) + 1; i++) {
        if (preferences[i] == room) {
            return i;
        }
    }
    return -1;
};

void Agent::printValuations() {
    cout << "Valuations for agent " << id << ": ";
    for (int i = 0; i < valuations.size(); i++) {
        cout << valuations[i] << " ";
    }
    cout << endl;
}

void Agent::printPreferences() {
    cout << "Preferences for agent " << id << ": ";
    for (int i = 0; i < preferences.size(); i++) {
        cout << preferences[i] << " ";
    }
    cout << endl;
}

/* AgentArray Class Methods */

AgentArray::AgentArray(int numAgents, RoomArray rooms) {
    this->numAgents = numAgents;
    for (int i = 0; i < numAgents; i++) {
        this->agents.push_back(Agent(i, rooms));
    }
};

int AgentArray::computeTotalWelfare(Matching &m) {
    int total_welfare = 0;
    for (int i = 0; i < agents.size(); i++) {
        int room_num = m.getAssignmentForAgent(i);
        if (room_num != -1) {
            total_welfare += agents[i].getValuation(room_num);
            // total_welfare -= agents[i].budget; // INCORPORATING BUDGET
        }
    }
    return total_welfare;
}

void AgentArray::printValuations() {
    for (int i = 0; i < agents.size(); i++) {
        agents[i].printValuations();
    }
}

void AgentArray::printPreferences() {
    for (int i = 0; i < agents.size(); i++) {
        agents[i].printPreferences();
    }
}

void AgentArray::printResults(Matching & m) {
    for (int i = 0; i < numAgents; i++) {
        int assign = m.getAssignmentForAgent(i);
        cout << "person " << i << " got assignment " << assign << " which is their " << agents[i].getPreferenceRank(assign) + 1 << " choice" << endl;
        }
}