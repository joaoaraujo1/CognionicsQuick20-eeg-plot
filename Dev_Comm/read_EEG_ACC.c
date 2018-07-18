
/**

João Araújo, 2017

Function that reads data from EEG channels / acceelerometer, transforms the data
and stores it in the data_chunk matrix

**/

void read_EEG_ACC(FT_HANDLE* ftHandle, int chunk_index ,double (*data_chunk)[27][125]){

  BYTE pcBufRead[3];
  DWORD  dwBytesRead;

  const int numEEG = 20; // 20 EEG channels
  BYTE   MSB[numEEG];
  BYTE  LSB2[numEEG];
  BYTE  LSB1[numEEG];
  double eeg[numEEG];

  //EEG data reading and processing
  for(int j = 0; j < numEEG; j++){
    FT_Read(*ftHandle, pcBufRead, 3*sizeof(BYTE), &dwBytesRead);
    MSB[j]  = pcBufRead[0];
    LSB2[j] = pcBufRead[1];
    LSB1[j] = pcBufRead[2];

    //Converting bytes to volts (according to manual)
    int data = (MSB[j] << 24) | (LSB2[j] << 17) | (LSB1[j] << 10);
    eeg[j] = data * (5.0 / 3.0) * (1.0/ pow(2,32)); // em Volts
    (*data_chunk)[j + 1][chunk_index] = (double)eeg[j];
  }

  //ACCELEROMETER data reading and processing
  const int numACC = 3; // 3 eixos de acelerometro
  BYTE  MSB_a[numACC];
  BYTE LSB2_a[numACC];
  BYTE LSB1_a[numACC];
  double  acc[numACC];

  for(int k = 0; k < numACC; k++){
    FT_Read(*ftHandle, pcBufRead, 3*sizeof(BYTE), &dwBytesRead);
    MSB_a[k]  = pcBufRead[0];
    LSB2_a[k] = pcBufRead[1];
    LSB1_a[k] = pcBufRead[2];

    //Converting bytes to volts (according to manual)
    int data = (MSB_a[k] << 24) + (LSB2_a[k] << 17) + (LSB1_a[k] << 10);
    acc[k] = data * 2.5 * (1.0 / pow(2,32));
    (*data_chunk)[20 + k + 1][chunk_index] = (double)acc[k];
  }

}
