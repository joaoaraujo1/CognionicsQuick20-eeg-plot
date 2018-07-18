
/***

João Araújo, 2017

Cleanup function closes the data file stream, the MATLAB engine and the
Cognionics device, sending error messages when closing is unsuccessful.

***/

void cleanup(FILE **Session, Engine **ep, FT_HANDLE *ftHandle, char* cogni_device){

  int errors;
  FT_STATUS ftStatus;

  printf("\n\nUser pressed keyboard or the experiment time is due. Ending program...\n");

  if(fclose(*Session) == EOF) printf("Error closing Session CSV file.\n");
  else printf("Closed Session file stream.\n");

  if((errors = engClose(*ep)) != 0) printf("Error closing MATLAB engine. Error %d\n",errors);
  else printf("Closed MATLAB engine.\n");

  if((ftStatus = FT_Close(*ftHandle)) != FT_OK) printf("Error closing device. Error %d",(int)ftStatus);
  else printf("Closed device %s.\n\n", cogni_device);

}
