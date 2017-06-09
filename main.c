#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#define FILE_NAME "pbndata.db"   //Defines a symbol for the PB file
#define MAX_PBN 99  //Defines maximun PB value


//Function Prototypes
bool quit(void);  //Tell user and set done flag to 0
void numberGen(void);  //RNG for power ball numbers
bool nextNum(void);  //tell user and set done flag to 1
void seed_RNG(void); //RNG seeding function
bool add_ToFile(short int rngArray[]);   //Call to Append PB Number File

short int rngArray[6] = {0};   // Power Ball Number Set Array

char date_time[21] = {0};  //date and time of PBN generation

/* declare a short interger flag var "fileActResult" to indicate file action that took place:
    * 0 = No Action Taken
    * 1 = Data was Added
    * 2 = Data was Modified
    * 3 = Data was Deleted
    * 4 = Data was VIewed
    */
    short int fileActResult = 0;

int main(void) {

    bool done = 1; //declare a bool flag var "done" that, if 0, indicates user is done using the progeam; initialize to 1



    char userEntry[6];  //arrary is set to 6 to hold command plus two bytes for the Windows EOL

    /* declare bool flag var "firstPass" that, if 1, indicates user's first pass through the program, so show
    * the initial Welcome screen/message;
    * initialize flag to 1;
    * program clears flag after user had initially generated a PB number set, thus, inhibiting display of
    * the Welcome screen each time the user choose to continue generating PB numbers.
    */
    bool  firstPass = 1;


    printf("Random Power Ball Number Generator\n\n");     //Title of program

/***************************************************************************************************************
* This is the main <do/while> loop. On entry, we first clear the user command array, <userEntry>.
*  then we get user's command from keyboard.
*  then we decide what action to take based upon the command
*      if 'quit', then we clear done flag and fall out of the bottom of the <do/while> loop
*      if 'next' then we run the <nextNum> func,
*         set done flag,
*         exit out of the <if> construct,
*         go to top of <do/while> loop
*      if a wrong command is typed, we tell user and go to the top of the loop
*******************************************************************************************************************/

do {

  seed_RNG();  // call to seed and initialize RNG

  int i;  //counter var
  for (i=0; i<sizeof(userEntry); i++) {  //use to clear the userEntry array
    userEntry[i]=0;
  }

  //puts(userEntry);  //TEST POINT = display whats in userEntry

  if (firstPass) {   // if first time through
    puts("Welcome To The Power Ball Number Generator");  //display program header
    firstPass = 0;  //clear firstPass flag
  }

  puts("Type 'next' to generate numbers, or 'quit' to exit program");  //display commands
  puts("Press 'Enter' to accept command");

  fgets(userEntry, sizeof(userEntry), stdin);  //get user command from keyboard

  //puts(userEntry);  //TEST POINT = display whats in userEntry

    if (strncmp(userEntry, "quit", 4) == 0) {  //User wants to quit
    done = quit();   //calls quit function and sets flag to 0
  }
    else if (strncmp(userEntry, "next", 4) == 0)  {
    done = nextNum();  // call func and set flag to 1 on return
  }
    else {   //user entered invalid command, tell, and go to top of loop
    puts(userEntry);   //what is in userEntry
    printf("\n I do not understand your command, try again...\n");
  }

  printf("\n\n");

}  while (done == 1);  // continue the do loop until done is 0

//User entered quit, so we exited Control Loop and RNG loop

printf("\n You quit, bye!\n");    //Tell user what they did

return 0;    //exit program
}

bool quit() {  //the quit func
  printf("You are quitting... \n");
  return 0;   //set done flag to 0
}

bool  nextNum() {  //this func calls the <numberGen> func and sets done flag on exit
  puts("Generate Power Ball Numbers");   //display what is going on
  numberGen();  //call RNG func
  return 1;  //set the done flag
}

void numberGen() {  //generate 6 sets of numbers 1 - 99, and time stamp
  time_t mytime;
  mytime = time(NULL);
  printf(ctime(&mytime));  //prints date and time

  // capture date/time for PBN file record
  strftime(date_time, sizeof(date_time), "%m-%d-%Y\n", localtime(&mytime));

  int i;   //counter for RNGl
  for(i=0; i<6; i++)  {
    while ((rngArray[i])==0) {  //if the RNG value is zero, than
    rngArray[i] = rand()% MAX_PBN +1;  //gen new random value that is >= 1 and MAX_PBN
  }
  printf("%d,", rngArray[i]);  // display value generated
  }

  char cmd = " ";  // add to file command var, (Y/N)

  //Ask if user wants to save this set to the PBN file
  printf("\n\nDo you want to save this set to a file?  (Y/N) ");
  scanf(" %c", &cmd);  //read user command
  while (getchar() != '\n');  // idom to clear end of line / enter key from keyboard buffer
  printf("\n");

  if (cmd == 'Y' || cmd =='y')  {   //verify that user entered a valid "Y" or "y" command

    //we call the <add_ToFile> func, passing to it <rngArray>, the return value is put in  <fileActResult>
    fileActResult = add_ToFile(rngArray);

    if (fileActResult==3) {
      puts("Data was Deleted from "FILE_NAME);  //Tell user what happened
    }
    else if (fileActResult==2) {
      puts("Data was Modified in "FILE_NAME);  //Tell user what happened
    }
    else if (fileActResult==1) {
      puts("Data was Added to "FILE_NAME);  //Tell user what happened
    }
    else if (fileActResult==0) {
      puts("No Action Taken Concerning "FILE_NAME);  //Tell user what happened
    }
    else {
      fputs("Undefined Action: ",fileActResult);  //Tell user what happened
    }
  } //Ends command <if>
} //Ends <numberGen()> func

/***********************************************************************************
* add_ToFile: Appends a set of Power Ball Numbers to rngdata.txt;
* creates the file if not found; returns bool 0 if file could not be opened,
* returns bool 1 if set is successfully added to file
***********************************************************************************/
bool  add_ToFile(short int rngArray[])  {
  FILE *pToFile;  //pointer to file
  bool success = 0;  //declare and initialize local flag for file operation success

  //Try opening file for appending, testing it to NULL and
  // pass handle to pToFile pointer is success

  if ((pToFile = fopen(FILE_NAME, "ab"))==NULL)  {
    printf("\n%s can't be opened.\n", FILE_NAME);
    success = 0;  //clear file operation success flag
  }
  else  {
    printf("%s opened for appending.\n", FILE_NAME);

    /*******************************************************************************************************
    * THE <fwrite> writes an array to a file. The first argument, here rngArray, is the array's
    * address, the second argument is the size of each array element (in bytes), and
    * the third argument is the number of elements to write. For the third, we could have
    * stated a value or referenced some var, but here we used some math to figure
    * out that size dymanicaly by examining the array itself, that is we took the
    * size of the entire array <rngArray> and divided that by the size of each element,
    * arriving at the number of elements in the array, The forth argument is a file
    * pointer, indicating where the data should be written.
    *******************************************************************************************************/

    printf("\n\n");

    fwrite(rngArray, sizeof(short int), 6, pToFile);
    success = 1;  //set file operation success flag
    fclose(pToFile); //close the file handle
  }
  return success;  //return Action Result to caller
}

/************************************************
* seed_RNG: Initializes the random number generator
*                  using the time of day
*************************************************/
void seed_RNG(void) {
  srand ((unsigned) time(NULL));  // seed it
}
