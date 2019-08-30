#include "GlobalTrafficTrace.h"

GlobalTrafficTrace::GlobalTrafficTrace(const char *fname, int id){

   // Open file
  this->nlines = 0;
  this->totalsize = 0;
  this->delivered_size = 0;

  char pe_fname [512];
  sprintf(pe_fname, "%03d_%s", id, fname);
  //~ cout << "Opening file: " << pe_fname <<  endl;

  // Create a communication from the parameters read on the line
  Commtrace ct;

  ifstream fin(pe_fname, ios::in);
  if (!fin)
    assert(false);

  this->local_id = id;

    // Cycle reading file
    while (!fin.eof()) {
        char line[512];
        fin.getline(line, sizeof(line) - 1);
        if (line[0] != '\0' && line[0] != '%') {
            int dst, sz;
            unsigned long long int ts_start, ts_end;
            char primitive[128];
            int params = sscanf(line, "%127s %llu %llu %d %d", primitive, &ts_start, &ts_end,  &dst, &sz);
            if (params >= 5) {
                ct.src = id;
                ct.dst = dst;
                ct.sz = sz;

                //~ ct.primitive = primitive;

                /// Synchronization
                if (strcmp(primitive, "MPI_Barrier") == 0)
                    ct.primitive = MPI_BARRIER;
                /// Collective communication
                else if (strcmp(primitive, "MPI_Allgather") == 0)
                    ct.primitive = MPI_ALLGATHER;
                else if (strcmp(primitive, "MPI_Allgatherv") == 0)
                    ct.primitive = MPI_ALLGATHERV;
                else if (strcmp(primitive, "MPI_Allreduce") == 0)
                    ct.primitive = MPI_ALLREDUCE;
                else if (strcmp(primitive, "MPI_Alltoall") == 0)
                    ct.primitive = MPI_ALLTOALL;
                else if (strcmp(primitive, "MPI_Alltoallv") == 0)
                    ct.primitive = MPI_ALLTOALLV;
                else if (strcmp(primitive, "MPI_Bcast") == 0)
                    ct.primitive = MPI_BCAST;
                else if (strcmp(primitive, "MPI_Gather") == 0)
                    ct.primitive = MPI_GATHER;

                /// Non-collective communication
                else if (strcmp(primitive, "MPI_Gatherv") == 0)                 // I guess this is wrong!
                    ct.primitive = MPI_GATHERV;
                else if (strcmp(primitive, "MPI_Reduce") == 0)                  // I guess this is wrong!
                    ct.primitive = MPI_REDUCE;
                else if (strcmp(primitive, "MPI_Scatter") == 0)                 // I guess this is wrong!
                    ct.primitive = MPI_SCATTER;
                else if (strcmp(primitive, "MPI_Scatterv") == 0)                // I guess this is wrong!
                    ct.primitive = MPI_SCATTERV;
                else if (strcmp(primitive, "MPI_Send") == 0)
                    ct.primitive = MPI_SEND;
                else if (strcmp(primitive, "MPI_Isend") == 0)
                    ct.primitive = MPI_ISEND;
                else if (strcmp(primitive, "MPI_Recv") == 0)
                    ct.primitive = MPI_RECV;
                else if (strcmp(primitive, "MPI_Irecv") == 0)
                    ct.primitive = MPI_IRECV;
                else {
                    ct.primitive = MPI_OTHER;
                    cout << "WARNING: Not treating the following MPI primitive:" << primitive << endl;
                }

                ct.ts_start = ts_start;
                ct.ts_end = ts_end;

                // Add this communication to the vector of communications
                traffic_trace.push(ct);
                totalsize += sz;
                nlines++;
            }
            else {
                cout << "Trace not in the correct format"  << endl;
                cout << " ["<<id<<"]\tLines: " << nlines << "\tBytes: " << totalsize << endl;
                assert(false);
            }
        }
    }

	cout << "Total Size: " << totalsize << "\t Number of lines: " << nlines << endl;

}

bool GlobalTrafficTrace::isEmpty(){
    if(traffic_trace.empty() && traffic_trace.size() == 0){
        if(!GlobalParams::trace_eof[this->local_id]){
            GlobalParams::trace_eof[this->local_id] = true;
            cout << "["<< sc_time_stamp() << "] " << local_id << " EOF" << endl;
        }
        return true;
    }
    return false;
}

Commtrace GlobalTrafficTrace::getComm(){
    Commtrace comm = traffic_trace.front();

    delivered_size += comm.sz;
    traffic_trace.pop();

    return comm;
}

Commtrace GlobalTrafficTrace::viewNextCommTrace(){
    return traffic_trace.front();
}

void GlobalTrafficTrace::pop(){
    traffic_trace.pop();
}

bool GlobalTrafficTrace::trace_empty(){
    return traffic_trace.empty();
}
