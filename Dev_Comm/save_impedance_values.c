
/***

João Araújo, 2017

Function to save the last impedance values recorded from the real-time impedance
check

***/

void save_impedance_values(double* impedance_calculated, char* Filename){
  FILE *Imp_csv;
  char* Path = malloc(512 * sizeof(char));
  strcpy(Path,Filename);
  Imp_csv = fopen(strcat(Path,"Impedance_values.csv"),"w");

  for(int i = 0; i < 20; i++){
    fprintf(Imp_csv,"Channel %d,%lf\n",i+1,impedance_calculated[i]);
  }

  fclose(Imp_csv);
  free(Path);
}
