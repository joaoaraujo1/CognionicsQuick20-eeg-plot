CC=gcc
UNAME := $(shell uname)
# Assume target is Mac OS if build host is Mac OS; any other host targets Linux
ifeq ($(UNAME), Darwin)
	DEPENDENCIES := -lpthread -lobjc -framework IOKit -framework CoreFoundation
else
	DEPENDENCIES := -lpthread -lrt
endif

# Include the flags and the FTD library static file
CFLAGS= -leng -lmx -lm -lmat -lut libftd2xx.a $(DEPENDENCIES) -Wall -Wextra
APP = device_comm
# Include MATLAB and its libraries
LINKMAT= -I/Applications/MATLAB_R2012b.app/extern/include/ -L/Applications/MATLAB_R2012b.app/bin/maci64/

all: $(APP)

# include the API
$(APP): main.c
	$(CC) main.c -o $(APP) $(CFLAGS) $(LINKMAT)

clean:
	-rm -f *.o ; rm $(APP)
