/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2015 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the top-level of Noxim
 */

#include "ConfigurationManager.h"
#include "NoC.h"
#include "GlobalStats.h"
#include "DataStructs.h"
#include "GlobalParams.h"

#include <csignal>

#include <execinfo.h>

using namespace std;

// need to be globally visible to allow "-volume" simulation stop
unsigned int drained_volume;
NoC *n;

void PrintStackTrace()
{
   void *array[256];
   size_t size = backtrace(array, 256);
   char ** strings = backtrace_symbols(array, 256);
   if (strings)
   {
      printf("-- Stack trace follows (%zd frames):\n", size);
      for (size_t i = 0; i < size; i++)
      {
         printf("  %s\n", strings[i]);
      }
      printf("-- End Stack trace\n");
      free(strings);
   }
   else
      printf("PrintStackTrace:  Error, could not generate stack trace!\n");
}

static void CrashSignalHandler(int signo)
{
    switch(signo) {
        case SIGABRT : cout << "Noxim received SIGABRT signal."; break;
        case SIGILL  : cout << "Noxim received SIGILL  signal."; break;
        case SIGINT  : cout << "Noxim received SIGINT  signal."; break;
        case SIGSEGV : cout << "Noxim received SIGSEGV signal."; break;
        case SIGTERM : cout << "Noxim received SIGTERM signal."; break;
    }

   // Uninstall this handler, to avoid the possibility of an infinite regress
   signal(SIGFPE,  SIG_DFL);
   signal(SIGILL,  SIG_DFL);
   signal(SIGSEGV, SIG_DFL);
   signal(SIGBUS,  SIG_DFL);
   signal(SIGABRT, SIG_DFL);
   signal(SIGSYS, SIG_DFL);

   printf("\n\n  CrashSignalHandler called with signal %i... \n", signo);
   printf(" I'm going to print a stack trace, then kill the process.\n\n");

   PrintStackTrace();

   printf("\n Crashed process exiting now.... bye!\n\n");
   fflush(stdout);
   exit(1);
}

void signalHandler( int signum )
{
    cout << "\b\b  " << endl;
    cout << endl;
    cout << "Current Statistics:" << endl;
    cout << "(" << sc_time_stamp().to_double() / GlobalParams::clock_period_ps
    << " sim cycles executed)" << endl;

    GlobalStats gs(n);
    gs.showStats(std::cout, GlobalParams::detailed);
}

int sc_main(int arg_num, char *arg_vet[])
{
    signal(SIGQUIT, signalHandler);

    signal(SIGFPE,  CrashSignalHandler);
    signal(SIGILL,  CrashSignalHandler);
    signal(SIGSEGV, CrashSignalHandler);
    signal(SIGBUS,  CrashSignalHandler);
    signal(SIGABRT, CrashSignalHandler);
    signal(SIGSYS,  CrashSignalHandler);

        // ~ gettimeofday(&stat_timer_start, NULL);
        // ~ gettimeofday(&stat_timer_end, NULL);


    // TEMP
    drained_volume = 0;

    // Flag to stop simulation by user
    GlobalParams::stop_simulation = false;

    // Handle command-line arguments
    cout << endl << "\t\tNoxim - the NoC Simulator" << endl;
    cout << "\t\t(C) University of Catania" << endl << endl;

    cout << "Catania V., Mineo A., Monteleone S., Palesi M., and Patti D. (2016) Cycle-Accurate Network on Chip Simulation with Noxim. ACM Trans. Model. Comput. Simul. 27, 1, Article 4 (August 2016), 25 pages. DOI: https://doi.org/10.1145/2953878" << endl;
    cout << endl;
    cout << endl;

    configure(arg_num, arg_vet);


    // Signals
    sc_clock clock("clock", GlobalParams::clock_period_ps, SC_PS);
    sc_signal <bool> reset;

    // NoC instance
    n = new NoC("NoC");

    n->clock(clock);
    n->reset(reset);

    // Trace signals
    sc_trace_file *tf = NULL;
    if (GlobalParams::trace_mode) {
    tf = sc_create_vcd_trace_file(GlobalParams::trace_filename.c_str());
    sc_trace(tf, reset, "reset");
    sc_trace(tf, clock, "clock");

    for (int i = 0; i < GlobalParams::mesh_dim_x; i++) {
        for (int j = 0; j < GlobalParams::mesh_dim_y; j++) {
        char label[30];

        sprintf(label, "req(%02d)(%02d).east", i, j);
        sc_trace(tf, n->req[i][j].east, label);
        sprintf(label, "req(%02d)(%02d).west", i, j);
        sc_trace(tf, n->req[i][j].west, label);
        sprintf(label, "req(%02d)(%02d).south", i, j);
        sc_trace(tf, n->req[i][j].south, label);
        sprintf(label, "req(%02d)(%02d).north", i, j);
        sc_trace(tf, n->req[i][j].north, label);

        sprintf(label, "ack(%02d)(%02d).east", i, j);
        sc_trace(tf, n->ack[i][j].east, label);
        sprintf(label, "ack(%02d)(%02d).west", i, j);
        sc_trace(tf, n->ack[i][j].west, label);
        sprintf(label, "ack(%02d)(%02d).south", i, j);
        sc_trace(tf, n->ack[i][j].south, label);
        sprintf(label, "ack(%02d)(%02d).north", i, j);
        sc_trace(tf, n->ack[i][j].north, label);
        }
    }
    }
    // Reset the chip and run the simulation
    reset.write(1);
    cout << "Reset for " << (int)(GlobalParams::reset_time) << " cycles... " << endl;
    srand(GlobalParams::rnd_generator_seed);
    sc_start(GlobalParams::reset_time, SC_NS);

    reset.write(0);

    if(GlobalParams::traffic_distribution == TRAFFIC_TRACE) // traffic trace
    cout << " done! Now running while scrolling through the traffic trace file..." << endl;
    else
        cout << " done! Now running for " << GlobalParams:: simulation_time << " cycles..." << endl;

    //~ sc_start(GlobalParams::simulation_time, SC_NS);
    sc_start();

    // Close the simulation
    if (GlobalParams::trace_mode) sc_close_vcd_trace_file(tf);
    cout << endl << "Noxim simulation completed." << endl;
    cout << " ( " << sc_time_stamp().to_double() / GlobalParams::clock_period_ps << " cycles executed)" << endl;

    // Show statistics
    GlobalStats gs(n);
    gs.showStats(std::cout, GlobalParams::detailed);

    // traffic trace - Releasing the storage
    delete[] GlobalParams::trace_eof;
    delete[] GlobalParams::hasBarrier;
    delete[] GlobalParams::blocked;
    delete[] GlobalParams::proctime;
    delete[] GlobalParams::packet_queue;
    delete[] GlobalParams::flits_queue;
    for(int i = 0; i < (GlobalParams::mesh_dim_x*GlobalParams::mesh_dim_y); ++i){
        delete GlobalParams::finalized[i];
	}
    delete GlobalParams::finalized;

    if ((GlobalParams::max_volume_to_be_drained > 0) &&
    (sc_time_stamp().to_double() / GlobalParams::clock_period_ps - GlobalParams::reset_time >=
     GlobalParams::simulation_time)) {
    cout << endl
         << "WARNING! the number of flits specified with -volume option" << endl
         << "has not been reached. ( " << drained_volume << " instead of " << GlobalParams::max_volume_to_be_drained << " )" << endl
         << "You might want to try an higher value of simulation cycles" << endl
         << "using -sim option." << endl;

#ifdef TESTING
    cout << endl
         << " Sum of local drained flits: " << gs.drained_total << endl
         << endl
         << " Effective drained volume: " << drained_volume;
#endif

    }

#ifdef DEADLOCK_AVOIDANCE
    cout << "***** WARNING: DEADLOCK_AVOIDANCE ENABLED!" << endl;
#endif
    return 0;
}
