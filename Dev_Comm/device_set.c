
/******

João Araújo, 2017

Reads EEPROM data and sets recording parameters according to the Quick20 manual

******/

int device_set(FT_HANDLE ftHandle, char* cogni_device){
  FT_STATUS ftStatus;

  /* GET FTD LIBRARY VERSION */
	DWORD libraryVersion = 0;
	ftStatus = FT_GetLibraryVersion(&libraryVersion);
	if (ftStatus == FT_OK)
	{
		printf("Library version = 0x%x\n", (unsigned int)libraryVersion);
	}


	static FT_PROGRAM_DATA Data_;
	/* MUST set Signature1 and 2 before calling FT_EE_Read */
	Data_.Signature1 = 0x00000000;
	Data_.Signature2 = 0xffffffff;
	Data_.Manufacturer = (char *)malloc(256); /* E.g "FTDI" */
	Data_.ManufacturerId = (char *)malloc(256); /* E.g. "FT" */
	Data_.Description = (char *)malloc(256); /* E.g. "USB HS Serial Converter" */
	Data_.SerialNumber = (char *)malloc(256); /* E.g. "FT000001" if fixed, or NULL */
	if (Data_.Manufacturer == NULL ||
			Data_.ManufacturerId == NULL ||
			Data_.Description == NULL ||
			Data_.SerialNumber == NULL)
	{
		printf("Failed to allocate memory.\n"); //
		return 1;
	}

	/* READ EEPROM DATA and free memory afterwards*/
	ftStatus = FT_EE_Read(ftHandle, &Data_);
	if(ftStatus != FT_OK) {
		printf("FT_EE_Read failed\n");
		return 1;
	}

	printf("EEPROM read succeeded.\n");
	printf("VendorId = 0x%04X\n", Data_.VendorId);
	printf("ProductId = 0x%04X\n", Data_.ProductId);
	printf("Manufacturer = %s\n", Data_.Manufacturer);
	printf("ManufacturerId = %s\n", Data_.ManufacturerId);
	printf("Description = %s\n\n", Data_.Description);

	//Free memory
	free(Data_.Manufacturer);
	free(Data_.ManufacturerId);
	free(Data_.Description);
	free(Data_.SerialNumber);
	//sleep(1);


	/*
	  SET FLOW CONTROL using Quick20 programmers manual data.
	  The last 2 arguments can be random because they are only used in
		XON_XOFF flow control
	*/
	ftStatus = FT_SetFlowControl(ftHandle, FT_FLOW_RTS_CTS, 0x11, 0x13);
		 if (ftStatus == FT_OK) {
				printf("Flow control FT_FLOW_RTS_CTS set in device %s\n", cogni_device);
		 } else {
				printf("Failed to set flow control\n");
				return 1;
		 }

	//sleep(1);

	/* SET BAUD RATE using Quick20 programmers manual data*/
	if((ftStatus = FT_SetBaudRate(ftHandle, 3000000)) != FT_OK) {
		printf("Error: FT_SetBaudRate returned %d, cogni_device = %s\n", (int)ftStatus, cogni_device);
		return 1;
	}

	printf("Baud rate set to 3,000,000 in device %s\n", cogni_device);

	//sleep(1);

	/* SET DATA CHARACTERISTICS using Quick20 programmers manual data */
	if((ftStatus = FT_SetDataCharacteristics(ftHandle, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE)) != FT_OK) {
		printf("Error: FT_SetDataCharacteristics returned %d, cogni_device = %s\n", (int)ftStatus, cogni_device);
		return 1;
	}

	printf("Data Characteristics set successfully in device %s\n", cogni_device);

	//sleep(1);

	/* SET LATENCY using Quick20 programmers manual data */
	UCHAR LatencyTimer = 1;
	if((ftStatus = FT_SetLatencyTimer(ftHandle, LatencyTimer)) != FT_OK) {
		printf("Error: FT_SetLatencyTimer returned %d, cogni_device = %s\n", (int)ftStatus, cogni_device);
		return 1;
	}

	printf("Latency timer (%d) set successfully in device %s\n", (int)LatencyTimer,cogni_device);

  return 0;

}
