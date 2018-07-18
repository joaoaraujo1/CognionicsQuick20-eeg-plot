
/****

João Araújo, 2017

Initializes the MATLAB engine and the MATLAB plots / environment variables of
interest

****/



int matlab_engine_init(Engine **ep, mxArray **Data)
{

  //data chunk variable to calculate size
  double data_chunk[27][125];

  if (!(*ep = engOpen("/Applications/MATLAB_R2012b.app/bin/./matlab")))
  {
    fprintf(stderr, "Can't start MATLAB engine\n");
    return 1;
  }

  else printf("MATLAB engine interface status: OK\n");

  engEvalString(*ep, "(addpath(genpath('YOUR_PATH_HERE')))");
  engEvalString(*ep, "close all; echoudp('off');");

  //Initialize the plot before starting to read data to prevent data loss and delay
  *Data = mxCreateDoubleMatrix(125,27,mxREAL);
  memcpy((void *)mxGetPr(*Data), (void *)data_chunk, sizeof(data_chunk));
  engPutVariable(*ep, "Data", *Data);

  return 0;

}
