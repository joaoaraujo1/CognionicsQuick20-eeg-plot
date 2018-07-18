/*****************

João Araújo, 2017

What dev_comm does:

1) Initializes using -experiment or -threshold arguments. Outputs an error message otherwise
2) According to the arguments, the MATLAB scripts and task that is ran differs.
3) Opens and reads static information from Cognionics Device Dongle (FTDI Chip)
4) Sets the baud rate and other properties according to the Cognionics Programmers manual
5) Performs an optional impedance checkup using 1s of data on each refresh
6) Initializes a MATLAB engine and cleans the device memory (purge)
7) Reads incoming information from the Dongle's Rx queue
8) Keeps record of number of lost packets
9) Transforms channel bytes into differences of potential (in mv)
10) Creates and updates a csv file (Session) with the incoming data
11) Periodically (each 125 packets or ~1/4 of second cycles) sends data to MATLAB

IF RUNING THE EXPERIMENT
12a) Runs Nuno's MATLAB ball control task

IF RUNING THRESHOLD CALCULATION
12b) Runs data acquisition to calculate the thresholds and the best channels to use for the task
12c) Runs a MATLAB GUI that allows you to choose the best channel combination

13) After a predefined task time, closes file streams and engines and exits

****************/

//engine.h and matrix.h use char16_t as uint16_t type so you have to define it
#define char16_t uint16_t

//Include all headers and libraries with our own API header file
#include "bmi_c_api.h"

//Maximum number of FTDI devices the program checks
#define MAX_DEVICES		5

int main(int argc, char *argv[])
{

	//Device-related variables
	char * 	pcBufLD[MAX_DEVICES + 1];
	char 	cBufLD[MAX_DEVICES][64];
	DWORD  dwBytesRead;
	FT_STATUS	ftStatus = FT_OK;
	FT_HANDLE	ftHandle;

	//error check variable and task selecting variable
	int errors;
	int app_val;


	/***
	CHECK PROGRAM INITIALIZATION ARGUMENTS AND RUN THE RESPECTIVE TASK
	***/
	char* Filename = malloc(512*sizeof(char));
  errors = task_init(argc,argv,&app_val,&Filename);
	if(errors != 0){
		printf("No task was initialized\n");
		return 1;
	}

	/***
	OPEN DEVICE
	***/
	for(int i = 0; i < MAX_DEVICES; i++) {
		pcBufLD[i] = cBufLD[i];
	}
	pcBufLD[MAX_DEVICES] = NULL;
	char* cogni_device = device_open(&ftHandle, pcBufLD,cBufLD);
	if(cogni_device == NULL){
		printf("Error opening device\n");
		return 1;
	}
	//sleep(1);


	/***
	READ DEVICE SPECIFICATIONS AND SET DE DEVICE PROPERTIES ACCORDING TO THE MANUAL
	***/
	errors = device_set(ftHandle, cogni_device);
	if(errors != 0){
		printf("Error setting device properties\n");
		return 1;
	}
	//sleep(1);

	/***
	START MATLAB ENGINE
	***/
	Engine  *ep;
	mxArray *Data;
	errors = matlab_engine_init(&ep,&Data);
	if(errors != 0){
		printf("Error initializing MATLAB engine\n");
		return 1;
	}
	//sleep(1);

	/***
	PROMPT USER FOR AN IMPEDANCE CHECK
	***/
  bool imp_pref = false; // saves user option of checking the impedance status
	errors = check_impedance(&ep, ftHandle, &imp_pref, Filename);
	if(errors != 0){
		printf("Error setting impedance check option\n");
		return 1;
	}

	/***
	CREATE CSV FILE WHERE YOU WILL SAVE THE RECORDED DATA
	***/
	FILE* Session;
	record_file_init(&Session,Filename);
	//sleep(1);

													/****

													MAIN PROGRAM LOOP

													****/

  double data_chunk[27][125];

	int chunk_index = 0;
	BYTE	pcBufRead[3];

	struct timeval startTime;

	//Timer variables
	bool startTimers = true;
	struct timeval startExperiment;
	int experimentMinutes = 10; // 10 minutes long experiment
	int baselineTime = 5; // 5 minutes long baseline threshold calculation
	struct timeval currentTime;
	currentTime.tv_sec = 0;

	//Set session time depending if we are doing threshold calc or the experiment
	time_t totalTime = 0;
	if(app_val == 1) totalTime = experimentMinutes;
	else if(app_val == 0) totalTime = baselineTime;
	else if(app_val == 2) totalTime = 60; // 60 minute session for Joaquim task

	//Lost packet history
	int lost_packets = 0, previous_packet = -1;
	bool isRecording = false;

	//Initialize task
	run_matlab_script(&ep,app_val,1,Filename);

	/**
	PURGE THE DEVICE AND CLEAR STDIN BEFORE STARTING THE SESSION
	**/
	errors = purge_script(ftHandle);
	if(errors != 0){
		printf("Error purging RX and TX buffers, returned %d\n", (int)ftStatus);
		return 1;
	}

	// clears stdin buffer prior to session
	fseek(stdin,0,SEEK_END);

	//Run loop for the total time or until the experimenter presses a key
	while( (currentTime.tv_sec - startExperiment.tv_sec) < totalTime * 60){

			/*READ DATA*/
			if((ftStatus = FT_Read(ftHandle, pcBufRead, sizeof(BYTE), &dwBytesRead)) != FT_OK) {
				printf("Error: FT_Read returned %d\n", (int)ftStatus);
				return 1;
			}
			else {

				/***
				Once the packet header (0xFF) is read we can read the rest of the packet
				assigning each value to its respective variable according to the Quick20
				programmers manual
				***/

				if(pcBufRead[0] == 0xFF)
				{

					/* Start timers if this is the first packet read */
					if(startTimers){

						//Start MATLAB trial timers if running the experiment
						if(app_val == 1) engEvalString(ep, "BMI_Cognionics_timers");

						//Start C experiment timer
						gettimeofday(&startExperiment, NULL);

						printf("\n*** Session Started! Press any key to stop recording! ***\n\n");
						startTimers = false;
					}

					//Packet counter (0-127) byte
					FT_Read(ftHandle, pcBufRead, sizeof(BYTE), &dwBytesRead);
					data_chunk[0][chunk_index] = (double)pcBufRead[0];

					//check if any packets were lost
					check_packets((int)pcBufRead[0],&previous_packet,&lost_packets);

					//read EEG and accelerometer data
					read_EEG_ACC(&ftHandle, chunk_index, &data_chunk);

					//Impedance status
					FT_Read(ftHandle, pcBufRead, sizeof(BYTE), &dwBytesRead);
					data_chunk[24][chunk_index] = (double)pcBufRead[0];

					//Battery Status
					FT_Read(ftHandle, pcBufRead, sizeof(BYTE), &dwBytesRead);
					data_chunk[25][chunk_index] = (double)pcBufRead[0] * 5.0/128.0;

					//Wireless event trigger reading (0 if absent 1 if present)
					FT_Read(ftHandle, pcBufRead, 2*sizeof(BYTE), &dwBytesRead);
					data_chunk[26][chunk_index] = (double)((pcBufRead[0] << 8) + pcBufRead[1]);

					//Write packet data into Session file
					write_csv_packet(&Session,data_chunk,chunk_index);


					//Send data to MATLAB every 125 packets (approx .25 sec)
					if(chunk_index == 124)
					{

						//Assign the data_chunk values to the MATLAB matrix Data
						memcpy((void *)mxGetPr(Data), (void *)data_chunk, sizeof(data_chunk));
						engPutVariable(ep, "Data", Data);

						//Move ball in the MATLAB plot according to the EEG values
						//Run main MATLAB loop code
						run_matlab_script(&ep,app_val,2,Filename);

						chunk_index = 0;

						//Uncomment this code to test MATLAB engine variable values

						/*mxArray *result;

						if ((result = engGetVariable(ep,"udpPos")) == NULL)
								printf("Oops! You didn't create a variable udpPos.\n\n");
							else {
								//printf("Data is class %s\t\n", mxGetClassName(result));
								char *udp_string = mxGetData(result);
								printf("udpPos: %s\t\n", udp_string);

							}*/

						//Get inter-packet-interval
						if(isRecording){

							DWORD EventDWord;
							DWORD TxBytes;
							DWORD RxBytes;
							FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
							struct timeval now;
							struct timeval elapsed;
							gettimeofday(&now, NULL);
							timersub(&now, &startTime, &elapsed);
							//printf("Inter-125-Packet-Interval: %f s | Rx queue = %i\n",(double)elapsed.tv_usec * pow(10,-6),RxBytes);
							gettimeofday(&startTime, NULL);

						} else{
								isRecording = true;
								gettimeofday(&startTime, NULL);
						}


					} else chunk_index++;

				}
		 }

		 //Check if a key has been pressed to end Session
		 if(kbhit() && (currentTime.tv_sec - startExperiment.tv_sec) > 1) break;

		 //Update current timer in the end of each loop
		 gettimeofday(&currentTime, NULL);
	}

	//Close file stream, MATLAB engine and EEG device in the end
	cleanup(&Session, &ep, &ftHandle, cogni_device);

	return 0;

}
