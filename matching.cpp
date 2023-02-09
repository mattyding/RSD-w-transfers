#include "matching.h"

using namespace std;

Matching::Matching(int numRooms) {
    this->numRooms = numRooms;
    this->assignments = vector<int>(numRooms);
    for (int i = 0; i < numRooms; i++) {
        assignments[i] = -1;
    }
}

int Matching::getAssignment(int agent_id) {
    for (int i = 0; i < numRooms; i++) {
        if (assignments[i] == agent_id) {
            return i;
        }
    }
    return -1;
}
