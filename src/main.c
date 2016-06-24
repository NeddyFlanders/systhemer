// C includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

//Local includes
#include "main.h"

int main(int argc, char *argv[]) {
  parseArgs(argc, argv);
  
  
  return 0;
}

void printHelp(const int exval) {
  printf("%s,%s show working getopt example\n", PACKAGE, VERSION); 
  printf("%s [-h] [-V]\n\n", PACKAGE);

  printf("  -h              print this help and exit\n");
  printf("  -V              print version and exit\n");
  printf("  -v              set verbose flag\n");

  if (!exval || EXIT_ON_ERR)
    exit(exval);
}

void verboseMessage(const char* message){
  if (verboseMode) printf("%s[verbose]: %s\n", PACKAGE, message);
}

void parseArgs(int argc, char *argv[]) {
  static struct option const long_options[] =
    {
      {"help",    no_argument, NULL, 'h'},
      {"version", no_argument, NULL, 'V'},
      {"verbose", no_argument, NULL, 'v'}
    };
  int opt;
  
  //If arg count is 1 because the first argument in argv is the command itself
  if(argc == 1) {
    fprintf(stderr, "This program needs arguments....\n\n");
    printHelp(1);
  }

  //Main loop that checks each toggle and argument
  while ((opt = getopt_long(argc, argv, "hVv", long_options, NULL)) != -1) {
    switch(opt) {
    case 'h':
      printHelp(0);
      break;
    case 'V':
      printf("%s %s\n\n", PACKAGE, VERSION); 
      exit(0);
      break;
    case 'v':
      verboseMode = true;
      printf("%s: Verbose option is set '%c'\n", PACKAGE, optopt);
      verboseMessage("Testing...");
      break;
    default:
    case '?':
      fprintf(stderr, "%s: Error - No such option: `%c'\n\n", PACKAGE, optopt);
      printHelp(1);
    }
  }
}