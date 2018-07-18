
/****

João Araújo, 2017

Checks how many arguments were used to initialize the program, showing how the
device_comm is run when the number of arguments is incorrect and choosing to run
the experiment or the threshold calculation task when the right arguments are given

****/

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>

int task_init(int argc, char** argv, int* app_val, char**Filename){

  // Show msg when there are no arguments or more than 1 argument
  if(argc != 2){
    printf("****************************************************\n"
           "*                                                  *\n"
           "*                     DEV_COMM                     *\n"
           "*                                                  *\n"
           "*  This software makes EEG tasks from MATLAB work  *\n"
           "*     with a Cognionics Quick20 dry-EEG device     *\n"
           "*                                                  *\n"
           "*          Run online EEG activity plots:          *\n"
           "*           sudo ./device_comm -activity           *\n"
           "*                                                  *\n"
           "*                                                  *\n"
           "*          Program written by João Araújo        *\n"
           "****************************************************\n\n");

    return 1;
  }

  //If number of arguments is correct, change the bool variable to indicate the
  //respective task
  else if(strcmp(argv[1],"-activity")   == 0) *app_val= 2;

  //If the argument is invalid, show error message to user
  else{

    printf("\nInvalid argument. Use sudo ./dev_comm to see the usage method\n\n");
    return 1;
  }

  bool name_is_correct = false;
  bool parent_exists = false;

  while(!name_is_correct)
  {
    // Get subject's name and session code to create new storage directory
    char *subj_name = malloc(sizeof(char)*256);
    printf("\nPlease Enter subject's name (20 characters max): ");
    scanf("%s", subj_name);

    // Check if this subject's directory already exists. If not create one
    DIR* subject_dir = opendir(subj_name);

    if(subject_dir){
        printf("\nSubject %s already exists. Proceeding to experiment.",subj_name);
        parent_exists = true;
        closedir(subject_dir);
    }

    else if(ENOENT == errno){

      printf("\nCreating folder for subject %s. ",subj_name);
    }

    else{
      printf("\nFailed to check if subject exists!!");
      return 1;
    }



    char *session_name = malloc(sizeof(char)*256);
    printf("\nPlease enter Run code (10 characters max): ");
    scanf("%s", session_name);

    //Get timeofday and convert it to a concatenated string to add to the filename
    time_t t = time(NULL);
    struct tm session_time = *localtime(&t);
    char s_year[256], s_month[256], s_day[256], s_hour[256], s_min[256], s_date[256];
    sprintf(s_year,"%02d", session_time.tm_year + 1900);
    sprintf(s_month,"%02d",session_time.tm_mon +1);
    sprintf(s_day,"%02d", session_time.tm_mday);
    sprintf(s_hour,"%02d", session_time.tm_hour);
    sprintf(s_min,"%02d", session_time.tm_min);
    strcpy(s_date,s_year);
    strcat(s_date,s_month);
    strcat(s_date,s_day);
    strcat(s_date,"_");
    strcat(s_date,s_hour);
    strcat(s_date,"_");
    strcat(s_date,s_min);

    // Concatenate filenames separated by an underscore
    char *temp_filename = malloc(sizeof(char)*512);
    strcat(temp_filename, s_date);
    strcat(temp_filename, "__");
    strcat(temp_filename, session_name);
    strcat(temp_filename, "/");

    // Create final parent + child directory string
    char* subj_dir_slash = malloc(sizeof(char)*512);
    strcpy(subj_dir_slash,subj_name);
    strcat(subj_dir_slash,"/");
    strcat(subj_dir_slash,temp_filename);
    memset(temp_filename,0,strlen(temp_filename));
    temp_filename = subj_dir_slash;


    char choice = 0;
    while(choice != 'y' && choice != 'n'){
  		printf("A new directory %s will be created, OK (y/n)? ",temp_filename);

  		scanf(" %c", &choice);

      //If we have a good name, we save it to the permanent file and end the loop
      if(choice == 'y'){
        strcpy(*Filename,temp_filename);
        name_is_correct = true;
      }

  	}

    struct stat st;

    // If the directory already exists
    if(stat(*Filename,&st) == 0 && name_is_correct){
      char choice = 0;
      while(choice != 'y' && choice != 'n'){
        printf("%s directory already exists. Overwrite session files (y/n)? ",*Filename);

        scanf(" %c", &choice);

        if(choice == 'n') name_is_correct = false;

      }

    }

    else {
      //Create directory
      int dir_state_parent = mkdir(subj_name, S_IRWXO | S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
      if(dir_state_parent != 0 && !parent_exists){
        printf("Could not create parent directory!\n");
        return dir_state_parent;
      }
      else{
        int chmod_state_parent = chmod(subj_name, S_IRWXO | S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if(chmod_state_parent != 0 && !parent_exists){
          printf("Could not set permissions in parent directory!\n");
          return chmod_state_parent;
        }
        else{

          int dir_state_child = mkdir(*Filename, S_IRWXO | S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
          if(dir_state_child != 0){
            printf("Could not create child directory!\n");
            return dir_state_child;
          }
          else{
            int chmod_state_child = chmod(*Filename, S_IRWXO | S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if(chmod_state_child != 0){
              printf("Could not set permissions in child directory!\n");
              return chmod_state_child;
            }
          }
        }
      }
    }

    //Free memory from temporary strings
    free(session_name);
    free(subj_name);
    free(subj_dir_slash);
  }

  return 0;

}
