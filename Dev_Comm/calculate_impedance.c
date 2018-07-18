
/***

João Araújo, 2017

Function to extract the impedance values for each electrode of the Quick20
using the first or second method in the manual out of 500 samples

***/

double* calculate_impedance(double (*raw_data)[500]){

  //return variable
  static double return_impedance[20];

  //correlation variables
  double cor1[20][500];
  double cor2[20][500];

  //sum of correlations across all samples
  double sum1[20] = {0.0};
  double sum2[20] = {0.0};

  //Sine sequences to extract the Impedance carrier wave
  double IMP1[] = {1.0/250.0, 0.0, -1.0/250.0, 0.0};
  double IMP2[] = {0.0, 1.0/250.0, 0.0, -1.0/250.0};

  //2nd Method variables
  //double fastImp[20] = {0.0};

  /***

  Calculations according to the manual

  ***/

  for(int i = 0; i < 20; i++){
    for (int j = 0; j < 500; j++){
      //1st Method - Comment if using Method2
      //Correlate the raw signal against the sine wave templates above separatelly
      cor1[i][j] = raw_data[i][j] * IMP1[j % 4];
      cor2[i][j] = raw_data[i][j] * IMP2[j % 4];
      //sum each correlation product
      sum1[i] += cor1[i][j];
      sum2[i] += cor2[i][j];

      //2nd Method - Uncomment this code if you want to use Method2, comment it otherwise
      /*if((j+1) % 4 == 0 && j > 0){

        double calc1 = fabs(raw_data[i][j-4] - raw_data[i][j-2]) / 2.0;
        double calc2 = fabs(raw_data[i][j-3] - raw_data[i][j-1]) / 2.0;

        if(calc1>calc2) fastImp[i]+= calc1;
        else fastImp[i]+= calc2;
      }*/

    }

    //1st Method The MAG is the sqrt of the difference of the sum of sum1 and sum2
    return_impedance[i] = sqrt(sum1[i]*sum1[i] + sum2[i]*sum2[i]);

    //2nd Method
    //return_impedance[i] = fastImp[i] / 125.0; //in 500 samples you do 500/4 calculations

    //Convert MAG to impedance multiplying by the constant factor of the manual
    return_impedance[i] *= 265393.0; // the constant updated acording to the Cognionics CTO

  }

  return return_impedance; // below 4000 kOhms it is the ideal impedance

}
