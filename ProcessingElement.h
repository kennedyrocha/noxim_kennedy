/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the processing element
 */

#ifndef __NOXIMPROCESSINGELEMENT_H__
#define __NOXIMPROCESSINGELEMENT_H__

#include <queue>
#include <systemc.h>

#include "DataStructs.h"
#include "GlobalTrafficTable.h"
#include "GlobalTrafficTrace.h"
#include "Utils.h"

using namespace std;

SC_MODULE(ProcessingElement)
{

    // I/O Ports
    sc_in_clk clock;        // The input clock for the PE
    sc_in < bool > reset;   // The reset signal for the PE

    sc_in < Flit > flit_rx; // The input channel
    sc_in < bool > req_rx;  // The request associated with the input channel
    sc_out < bool > ack_rx; // The outgoing ack signal associated with the input channel

    sc_out < Flit > flit_tx;    // The output channel
    sc_out < bool > req_tx; // The request associated with the output channel
    sc_in < bool > ack_tx;  // The outgoing ack signal associated with the output channel

    sc_in < int >free_slots_neighbor;

    // Registers
    int local_id;       // Unique identification number
    bool current_level_rx;  // Current level for Alternating Bit Protocol (ABP)
    bool current_level_tx;  // Current level for Alternating Bit Protocol (ABP)
    queue < Packet > packet_queue;  // Local queue of packets
    bool transmittedAtPreviousCycle;    // Used for distributions with memory

    // Functions
    void rxProcess();       // The receiving process
    void txProcess();       // The transmitting process
    bool canShot(Packet & packet);  // True when the packet must be shot
    Flit nextFlit();    // Take the next flit of the current packet
    Packet trafficTest();   // used for testing traffic
    Packet trafficRandom(); // Random destination distribution
    Packet trafficTranspose1(); // Transpose 1 destination distribution
    Packet trafficTranspose2(); // Transpose 2 destination distribution
    Packet trafficBitReversal();    // Bit-reversal destination distribution
    Packet trafficShuffle();    // Shuffle destination distribution
    Packet trafficButterfly();  // Butterfly destination distribution
    Packet trafficLocal();  // Random with locality
    Packet trafficULocal(); // Random with locality
    Packet trafficTrace();  // Traffic trace


    GlobalTrafficTable *traffic_table;  // Reference to the Global traffic Table
    
    //~ TRACE attributes   
    GlobalTrafficTrace *traffic_trace;  // Reference to the Global traffic trace
    unsigned long long int traffic_trace_size;
    unsigned long long int blocked_at;
    bool immediate = false;    // Flag to MPI immediate or not 
    

    bool never_transmit;    // true if the PE does not transmit any packet
    //  (valid only for the table based traffic)

    void fixRanges(const Coord, Coord &);   // Fix the ranges of the destination
    int randInt(int min, int max);  // Extracts a random integer number between min and max
    int getRandomSize();    // Returns a random size in flits for the packet
    void setBit(int &x, int w, int v);
    int getBit(int x, int w);
    double log2ceil(double x);

    // TRACE methods
    bool hasFlitsInQueue();
    inline bool mustStop();
    inline bool check_heart_beat();    
    bool check_blocked(int local_id);
    bool check_barrier(int local_id);
    bool check_processing(int local_id);
    bool isCollective( mpi_primitive_t primitive );
    bool isBarrier( mpi_primitive_t primitive );
    bool isSend( mpi_primitive_t primitive );    
    bool isRecv( mpi_primitive_t primitive );    
    unsigned long long int barrierCheckCounter = 0;


    int roulett();
    int findRandomDestination(int local_id,int hops);

    // Constructor
    SC_CTOR(ProcessingElement) {
    SC_METHOD(rxProcess);
    sensitive << reset;
    sensitive << clock.pos();

    SC_METHOD(txProcess);
    sensitive << reset;
    sensitive << clock.pos();
    }

};

#endif
