/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the global params needed by Noxim
 * to forward configuration to every sub-block
 */

#ifndef __NOXIMGLOBALPARAMS_H__
#define __NOXIMGLOBALPARAMS_H__

#include <map>
#include <utility>
#include <vector>
#include <string>

using namespace std;

#define CONFIG_FILENAME        "config.yaml"
#define POWER_CONFIG_FILENAME  "power.yaml"

// Define the directions as numbers
#define DIRECTIONS             4
#define DIRECTION_NORTH        0
#define DIRECTION_EAST         1
#define DIRECTION_SOUTH        2
#define DIRECTION_WEST         3
#define DIRECTION_LOCAL        4
#define DIRECTION_HUB          5
#define DIRECTION_WIRELESS     747

// Generic not reserved resource
#define NOT_RESERVED          -2

// To mark invalid or non exhistent values
#define NOT_VALID             -1

// Routing algorithms
#define ROUTING_DYAD           "DYAD"
#define ROUTING_TABLE_BASED    "TABLE_BASED"

// Selection strategies
#define SEL_RANDOM             0
#define SEL_BUFFER_LEVEL       1
#define SEL_NOP                2
#define INVALID_SELECTION     -1

// Traffic distribution
#define TRAFFIC_RANDOM         "TRAFFIC_RANDOM"
#define TRAFFIC_TRANSPOSE1     "TRAFFIC_TRANSPOSE1"
#define TRAFFIC_TRANSPOSE2     "TRAFFIC_TRANSPOSE2"
#define TRAFFIC_HOTSPOT        "TRAFFIC_HOTSPOT"
#define TRAFFIC_TABLE_BASED    "TRAFFIC_TABLE_BASED"
#define TRAFFIC_BIT_REVERSAL   "TRAFFIC_BIT_REVERSAL"
#define TRAFFIC_SHUFFLE        "TRAFFIC_SHUFFLE"
#define TRAFFIC_BUTTERFLY      "TRAFFIC_BUTTERFLY"
#define TRAFFIC_LOCAL          "TRAFFIC_LOCAL"
#define TRAFFIC_ULOCAL         "TRAFFIC_ULOCAL"
#define TRAFFIC_TRACE          "TRAFFIC_TRACE" // Traffic Trace

// Verbosity levels
#define VERBOSE_OFF            "VERBOSE_OFF"
#define VERBOSE_LOW            "VERBOSE_LOW"
#define VERBOSE_MEDIUM         "VERBOSE_MEDIUM"
#define VERBOSE_HIGH           "VERBOSE_HIGH"


// Wireless MAC constants
#define RELEASE_CHANNEL 1
#define HOLD_CHANNEL    2

#define TOKEN_HOLD             "TOKEN_HOLD"
#define TOKEN_MAX_HOLD         "TOKEN_MAX_HOLD"
#define TOKEN_PACKET           "TOKEN_PACKET"

/// Enumerates the MPI primitives
enum mpi_primitive_t {

/*   /// Synchronization
   MPI_BARRIER ,

   /// Collective communication
   MPI_ALLGATHER   ,   todos-todos 
   MPI_ALLGATHERV  ,   todos-todos
   MPI_ALLREDUCE   ,   todos-todos
   MPI_ALLTOALL    ,   todos-todos
   MPI_ALLTOALLV   ,   todos-todos

   MPI_BCAST       ,   um-todos
   MPI_SCATTER    ,    um-todos
   MPI_SCATTERV   ,    um-todos

   MPI_GATHER     ,    todos-um
   MPI_GATHERV    ,    todos-um
   MPI_REDUCE     ,    todos-um
 */

    /// Synchronization
    MPI_BARRIER ,

    /// Collective communication
    MPI_ALLGATHER   ,
    MPI_ALLGATHERV  , 
    MPI_ALLREDUCE   ,
    MPI_ALLTOALL    ,
    MPI_ALLTOALLV   ,
    MPI_BCAST       ,
    MPI_GATHER      ,

    MPI_GATHERV    ,
    MPI_REDUCE     ,
    MPI_SCATTER    ,
    MPI_SCATTERV   ,   // bcast é a mesma msg, scatter pedaças da msg

    /// Non-collective communication
    MPI_SEND       ,
    MPI_ISEND      ,

    MPI_RECV       ,
    MPI_IRECV      ,

    MPI_OTHER      
};

/*
 * NAS size A uses only:
    MPI_Allreduce
    MPI_Alltoall
    MPI_Alltoallv
    MPI_Bcast

    MPI_Irecv
    MPI_Isend
    MPI_Recv
    MPI_Reduce
    MPI_Send
 */

typedef struct {
    pair<double, double> ber;
    int dataRate;
    vector<string> macPolicy;
} ChannelConfig;

typedef struct {
    vector<int> attachedNodes;
    vector<int> rxChannels;
    vector<int> txChannels;
    int toTileBufferSize;
    int fromTileBufferSize;
    int txBufferSize;
    int rxBufferSize;
} HubConfig;

typedef struct {
    map<pair <int, int>, double> front;
    map<pair <int, int>, double> pop;
    map<pair <int, int>, double> push;
    map<pair <int, int>, double> leakage;
} BufferPowerConfig;

typedef map<double, pair <double, double> > LinkBitLinePowerConfig;

typedef struct {
    map<pair<double, double>, pair<double, double> > crossbar_pm;
    map<int, pair<double, double> > network_interface;
    map<string, pair<double, double> > routing_algorithm_pm;
    map<string, pair<double, double> > selection_strategy_pm;
} RouterPowerConfig;

typedef struct {
    pair<double, double> transceiver_leakage;
    pair<double, double> transceiver_biasing;
    double rx_dynamic;
    double rx_snooping;
    double default_tx_energy;
    map<pair <int, int>, double> transmitter_attenuation_map;
} HubPowerConfig;

typedef struct {
    BufferPowerConfig bufferPowerConfig;
    LinkBitLinePowerConfig linkBitLinePowerConfig;
    RouterPowerConfig routerPowerConfig;
    HubPowerConfig hubPowerConfig;
} PowerConfig;

struct GlobalParams {
    static string verbose_mode;
    static int trace_mode;
    static string trace_filename;
    static int mesh_dim_x;
    static int mesh_dim_y;
    static double r2r_link_length;
    static double r2h_link_length;
    static int buffer_depth;
    static int flit_size;
    static int min_packet_size;
    static int max_packet_size;
    static string routing_algorithm;
    static string routing_table_filename;
    static string selection_strategy;
    static double packet_injection_rate;
    static double probability_of_retransmission;
    static double locality;
    static string traffic_distribution;
    static string traffic_table_filename;
    static string config_filename;
    static string power_config_filename;
    static int clock_period_ps;
    static int simulation_time;
    static int reset_time;
    static int stats_warm_up_time;
    static int rnd_generator_seed;
    static bool detailed;
    static vector <pair <int, double> > hotspots;
    static double dyad_threshold;
    static unsigned int max_volume_to_be_drained;
    static bool show_buffer_stats;
    static bool use_winoc;
    static bool use_powermanager;
    static ChannelConfig default_channel_configuration;
    static map<int, ChannelConfig> channel_configuration;
    static HubConfig default_hub_configuration;
    static map<int, HubConfig> hub_configuration;
    static map<int, int> hub_for_tile;
    static PowerConfig power_configuration;

    /* --- TRAFIC TRACE AND CUSTOM --- */
    static string traffic_trace_filename;
    static unsigned long long int barrier;   // MPI Barrier
    static bool * hasBarrier; // MPI_BARRIER flags
    static bool * blocked;  // MPI Blocking primitives
    static unsigned long long int * proctime;   // MPI Processing time
    static bool * trace_eof;
    static bool stop_simulation;
    static unsigned long long int traffic_trace_flit_headtail_size;
    static unsigned long long int total_payload;
    static unsigned long long int total_headtail;
    static unsigned long long int total_padding;
    static unsigned long long int total_wirelessflits;
    static unsigned long long int ** finalized;
    static unsigned long long int * packet_queue; //  Global counter of packets in queue
    static unsigned long long int proctime_step_forward; // To speed up proctime
    static unsigned long long int * flits_queue; //  Global counter of flits in queue    
};

#endif
