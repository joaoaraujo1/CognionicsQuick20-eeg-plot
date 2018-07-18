/***

João Araújo, 2017

Runs initialization, main loop and termination MATLAB scripts according to the
task that was chosen by the user

***/

void run_matlab_script(Engine **ep, int app_val, int code, char*Filename){

  char* matlabFileName;

  //Initialization scripts - code 1
  if(code == 1){

	if(app_val == 2) {
      	matlabFileName = "Activity_Plot_1";
    }

    char* dir_exp = malloc(sizeof(char)*50);
    strcpy(dir_exp,Filename);
    char* dir_str = malloc(sizeof(char)*50);
    strcpy(dir_str,"Dir='");
    strcat(dir_str,dir_exp);
    strcat(dir_str,"'");
    engEvalString(*ep,dir_str);
    engEvalString(*ep,matlabFileName);
    free(dir_exp);
    free(dir_str);

  }

  //Main loop scripts - code 2
  if(code == 2){

	if(app_val == 2){
		matlabFileName = "Activity_Plot_2";
    }

    engEvalString(*ep, matlabFileName);

  }

}
