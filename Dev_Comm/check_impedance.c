
/****

João Araújo, 2017

Check_impedance prompts the user for a real-time impedance check GUI. If the user
wants to check impedance values, it will procede to retrieve 500 samples (~1sec
@ 500Hz) at a time and call the calculate_impedance function to estimate the
impedance values for the 20 EEG channels

TODO code cleanup where possible

****/

#include "calculate_impedance.c"    // impedance calculation function
#include "kbhit_mac.c"					    // function to detect keyboard press by the user
#include "save_impedance_values.c"  // function to save the impedance list to a csv


int check_impedance(Engine **ep, FT_HANDLE ftHandle, bool *imp_pref, char*Filename){

  FT_STATUS ftStatus;
  char choice = 0;

  while(choice != 'y' && choice != 'n'){
		printf("Enable Real-time Impedance check? (y/n): ");

		scanf(" %c", &choice);
		fseek(stdin,0,SEEK_END); // clears stdin buffer

	}

	char set_impedance_byte = 0;
	if(choice == 'y'){
      set_impedance_byte = 0x11; // Impedance check was enabled
      *imp_pref = true;
  }
	else {
    set_impedance_byte = 0x12; // Impedance check was not enabled
    *imp_pref = false;
  }


	DWORD bytesWritten;
	if((ftStatus = FT_Write(ftHandle, &set_impedance_byte, sizeof(char), &bytesWritten) != FT_OK)) {
		printf("Error setting impedance check preferences. Error %d\n", (int)ftStatus);
		return 1;
	} else {
		if(choice == 'y'){

      printf("\n*** Checking Impedance... Press any key to exit ***\n\n");

      //Impedance track variables
      int impedance_samples = 0;
      double impedance_list[20][500];
      double *impedance_calculated;
      BYTE pcBufRead[3];

      DWORD  dwBytesRead;

      mxArray *ImpedanceMAT;
      ImpedanceMAT = mxCreateDoubleMatrix(1,20,mxREAL);

      //default threshold defined as 4000 kOhm as per manual suggestion
      engEvalString(*ep, "threshold = 500;");


      while(1){

        /*READ DATA*/
  			if((ftStatus = FT_Read(ftHandle, pcBufRead, sizeof(BYTE), &dwBytesRead)) != FT_OK) {
  				printf("Error: FT_Read returned %d\n", (int)ftStatus);
  			}
  			else {

  				/***
  				Once the packet header (0xFF) is read we can read the rest of the packet
  				assigning each value to its respective variable according to the Quick20
  				programmers manual
  				***/
  				if(pcBufRead[0] == 0xFF)
  				{


              const int numEEG = 20; // 20 canais de EEG

              //Discard Packet counter byte
              FT_Read(ftHandle, pcBufRead, sizeof(BYTE), &dwBytesRead);

              BYTE   MSB[numEEG];
              BYTE  LSB2[numEEG];
              BYTE  LSB1[numEEG];
              double eeg[numEEG];

              //EEG data reading and processing
              for(int j = 0; j < numEEG; j++){
                FT_Read(ftHandle, pcBufRead, 3*sizeof(BYTE), &dwBytesRead);
                MSB[j]  = pcBufRead[0];
                LSB2[j] = pcBufRead[1];
                LSB1[j] = pcBufRead[2];

                //Converting bytes to volts (according to manual)
                int data = (MSB[j] << 24) | (LSB2[j] << 17) | (LSB1[j] << 10);
                eeg[j] = data * (5.0 / 3.0) * (1.0/ pow(2,32)); // em Volts
                impedance_list[j][impedance_samples] = (double)eeg[j];
                //printf("EEG data of channel %i is %f for sample %i\n", j+1, eeg[j],impedance_samples);
              }

              //Read and flush the other data we do not need
              BYTE flushStuff[13];
              FT_Read(ftHandle, flushStuff, 13*sizeof(BYTE), &dwBytesRead);
              //printf("Impedance byte = %i\n",flushStuff[9]);

              ++impedance_samples;

              //Update impedance values after each second of data
              if(impedance_samples == 500){

                // calculate the 20-element impedance value vector
                impedance_calculated = calculate_impedance(impedance_list);

                // Copy variable with the 20 calculated impedances to the shared memory and
                // pass it to a MATLAB variable
                memcpy((void *)mxGetPr(ImpedanceMAT), (void *)impedance_calculated, sizeof(double) * 20);
                engPutVariable(*ep, "Impedance_values", ImpedanceMAT);


                /*
                for(int i = 0; i < numEEG; i++){
                  printf("Impedance of channel %i: %.1f\n", i+1, impedance_calculated[i]);
                }
                */

                // Run MATLAB GUI
                engEvalString(*ep, "Impedance_RT_Plot");

                impedance_samples = 0;
              }
            }
          }
          //Check if a key has been pressed to end impedance check
          if(kbhit()){
            engEvalString(*ep, "close all"); //close figure
            printf("Real-time impedance check terminated\n");
            save_impedance_values(impedance_calculated,Filename);

            set_impedance_byte = 0x12; // Disable Impedance Check
            if((ftStatus = FT_Write(ftHandle, &set_impedance_byte, sizeof(char), &bytesWritten) != FT_OK)) {
              printf("Error setting impedance check preferences. Error %d\n", (int)ftStatus);
              return 1;
            }

            break;
          }
        }

      } else printf("Real-time impedance check disabled\n");
    }

  return 0;

}
