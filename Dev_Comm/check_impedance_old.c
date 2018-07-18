
/****

João Araújo, 2017

Check_impedance prompts the user for an impedance check. If the user wants to
check impedance values, it will procede to retrieve 500 samples (~1sec @ 500Hz)
and call the calculate_impedance function to estimate the impedance values for
the 20 EEG channels

TODO code cleanup where possible

****/

#include "calculate_impedance.c"// impedance calculation function

int check_impedance(FT_HANDLE ftHandle, bool *imp_pref){

  FT_STATUS ftStatus;
  char choice = 0;

  while(choice != 'y' && choice != 'n'){
		printf("Enable Impedance check? (y/n): ");

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

      printf("Checking Impedance...\n");

      char stop_imp = 0;
      //Impedance track variables
      int impedance_samples = 0;
      double impedance_list[20][500];
      BYTE pcBufRead[3];

      DWORD  dwBytesRead;

      while(stop_imp != 'y'){

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

              //Read the other stuff we do not need
              BYTE flushStuff[13];
              FT_Read(ftHandle, flushStuff, 13*sizeof(BYTE), &dwBytesRead);
              //printf("Impedance byte = %i\n",flushStuff[9]);

              ++impedance_samples;

              if(impedance_samples == 500){


                double* impedance_calculated = calculate_impedance(impedance_list);
                for(int i = 0; i < numEEG; i++){
                  printf("Impedance of channel %i: %.1f\n", i+1, impedance_calculated[i]);
                }
                impedance_samples = 0;

                while(stop_imp != 'y' && stop_imp != 'n'){
                  printf("Exit Impedance check? (y/n): ");

                  scanf(" %c", &stop_imp);
                  fseek(stdin,0,SEEK_END); // clears stdin buffer
                }

                if(stop_imp == 'y'){
                  set_impedance_byte = 0x12;

                  if((ftStatus = FT_Write(ftHandle, &set_impedance_byte, sizeof(char), &bytesWritten) != FT_OK)) {
                    printf("Error setting impedance check preferences. Error %d\n", (int)ftStatus);
                    return 1;
                  }
                  printf("Impedance check disabled.\n");
                }else{
                  //Purge device to eliminate any EEG data and start the calculations all over
                  if((ftStatus = FT_Purge(ftHandle, FT_PURGE_RX | FT_PURGE_TX)) != FT_OK) {
                    printf("Error purging RX and TX buffers, returned %d\n", (int)ftStatus);
                    return 1;
                  }
                  stop_imp = 0;
                }
              }
            }
          }
        }
      } else printf("Impedance check disabled\n");
    }

  return 0;

}
