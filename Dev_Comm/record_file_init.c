

/***

João Araújo, 2017

Creates a csv file with session data

***/

void record_file_init(FILE**Session,char *Filename)
{
  char* csvFullName = malloc(sizeof(char)*512);

  char* dir = malloc(sizeof(char)*512);
  strcpy(dir,Filename);

  int i = 0;

  //Find where the session subject and name start
  while(dir[i] != '_' || dir[i+1] != '_') ++i;

  char s_name[512];

  for(unsigned long j = 0; (j+i+1) <= strlen(dir); j++)
  {
    s_name[j] = dir[j+(i+1)];
  }

  //Delete de "/" char from the directory string
  s_name[strlen(s_name)-1] = '\0';

  char* csvName = "Session";

  csvFullName = strcat(dir,csvName);
  strcat(csvFullName,s_name);
  strcat(csvFullName,".csv");

  printf("Writing data on %s\n", csvFullName);

  *Session = fopen(csvFullName,"w+");

  //free(dir);
  //free(csvFullName);
}
