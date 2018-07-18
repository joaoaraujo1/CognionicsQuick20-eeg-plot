
/****

João Araújo, 2017

Nuno's impedance wave filter implemented in C

****/

//Filter values according to the manual to be used on the filter function
/*double B1[] = {.85, 0.0, .85};
double A1[] = {1.0, 0.0, .7};
double B2[] = {.8, .8};
double A2[] = {1.0, .6};*/

void impedance_filter(double B[], double A[], int n, double (*data_chunk)[27][125]){

  double *z = (double *)malloc(n * sizeof(double));

  for(int i = 0; i < n; i++)
  {
    B[i] = B[i] / A[0];
    z[i] = 0;
  }

  for(int i = 0; i < n; i++)
  {
    A[i] = A[i] / A[0];
  }

  for(int i = 0; i < 23; i++){ // for each of the 20 EEG + 3 accelerometer channels
    for(int j = 0; j < 125; j++){ // and 125 samples (0.25s)
        double unfiltered_channel = (*data_chunk)[i+1][j]; //save X(m) in this variable
        (*data_chunk)[i+1][j] = B[0] * (*data_chunk)[i+1][j] + z[0]; //Y(m) becomes the updated data_chunk

        for(int k = 1; k < n; k++){
          z[k-1] = B[k] * unfiltered_channel + z[k] - A[k] * (*data_chunk)[i+1][j]  ;
        }
     }
  }
  free(z);

}
