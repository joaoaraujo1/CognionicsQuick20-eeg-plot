
/****

João Araújo, 2017

Checks how many FTDI devices are connected and opens your device using its Serial
number

****/



char *device_open(FT_HANDLE *ftHandle, char **pcBufLD, char cBufLD[5][64]){
  FT_STATUS ftStatus;
  int	iNumDevs = 0; //number of devices

  ftStatus = FT_ListDevices(pcBufLD, &iNumDevs, FT_LIST_ALL | FT_OPEN_BY_SERIAL_NUMBER);

  if(ftStatus != FT_OK) {
    printf("Error: FT_ListDevices returned %d\n", (int)ftStatus);
    return NULL;
  }

  if(iNumDevs != 1) {
    printf("Error: You must have ONE device plugged in.\nNumber of devices: %d\n", iNumDevs);
    return NULL;
  }

  char *cogni_device = cBufLD[0];
  printf("Device Serial Number - %s\n", cogni_device);

  /* OPEN DEVICE BY SERIAL NUMBER */
  if((ftStatus = FT_OpenEx(cogni_device, FT_OPEN_BY_SERIAL_NUMBER, ftHandle)) != FT_OK){

    printf("Error: FT_OpenEx returned %d for device %s\n", (int)ftStatus, cogni_device);
    return NULL;
  }

  printf("Opened device %s\n", cogni_device);

  return cogni_device;

}
