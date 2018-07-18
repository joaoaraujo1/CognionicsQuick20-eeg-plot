
/**

João Araújo, 2017

Purges the device (ie eliminates data that may still be on the queue from previous
sessions and that could contaminate the present session) and checks if the Rx and
Tx queue are in fact purged by checking their data.

**/

int purge_script(FT_HANDLE *ftHandle){


  FT_STATUS ftStatus;

  if((ftStatus = FT_Purge(ftHandle, FT_PURGE_RX | FT_PURGE_TX)) != FT_OK) {
    return (int) ftStatus; // Error purging the device
  }

  //Get number of bytes on the Rx and Tx queue (it should be zero)
  //just to make sure the purge was successful
  else{
    DWORD EventDWord;
    DWORD TxBytes;
    DWORD RxBytes;
    FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
    printf("Device Purge sucessful. RxBytes = %d | TxBytes = %d\n",RxBytes,TxBytes);

  }

  return 0;

}
