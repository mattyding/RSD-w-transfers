/* File: matching.h
* -------------
* This file defines the Matching class, which is responsible for
* storing a matching between agents and rooms.
*/
#ifndef _env_h
#define _env_h

#include <vector>

class Matching {
    public:
        int numRooms;
        std::vector<int> assignments;

        Matching(int numRooms);

        int getAssignment(int agent_id);
};


#endif

