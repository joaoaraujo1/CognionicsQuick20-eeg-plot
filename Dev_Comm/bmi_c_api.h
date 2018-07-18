#ifndef BMI_C_API_H_
#define BMI_C_API_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>
#include <sys/time.h>
#include <string.h>
#include <engine.h>
#include <matrix.h>
#include "ftd2xx.h"						  // FTDI functions library
#include "task_init.c"				  // task initialization function
#include "device_open.c"        // device opening function
#include "device_set.c"         // device settings load function
#include "check_impedance.c"    // impedance check function
#include "record_file_init.c"   // function to create csv file to record data
#include "matlab_engine_init.c" // MATLAB engine start function
#include "write_csv_packet.c"	  // Packet writing function to csv
#include "check_packets.c"      // Lost packets check function
#include "purge_script.c"				// Purge device function with Rx and Tx check
#include "read_EEG_ACC.c"       // EEG and ACC data read and conversion function
#include "filter.c" 					  // impedance wave filter function (NOT USED ANYMORE DUE TO CODE CHANGES)
//#include "kbhit_mac.c"					// function to detect keyboard press by the user
#include "cleanup.c"						// memory cleanup function
#include "run_matlab_script.c"  // run matlab script function

#endif
