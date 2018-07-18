
/**

João Araújo, 2017

This function writes the processed data from one single packet to the Session file.
It prints the int data like impedance check and battery status without decimal places
and prints the battery value with only 2 decimal places

**/

void write_csv_packet(FILE**Session,double data_chunk[27][125],int chunk_index){

  for(int p = 0; p < 27; p++)
  {
    if(p == 0 || p == 24 || p == 26)
      if(p != 26) fprintf(*Session,"%.0lf,",data_chunk[p][chunk_index]);
      // do not write a comma if it is the last value
      else fprintf(*Session,"%.0lf",data_chunk[p][chunk_index]);

    else if(p == 25) fprintf(*Session,"%.2lf,",data_chunk[p][chunk_index]);

    else fprintf(*Session,"%.15lf,",data_chunk[p][chunk_index]);
  }

  fprintf(*Session,"\n");

}
