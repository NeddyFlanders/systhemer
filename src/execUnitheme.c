#include "execUnitheme.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

void execUnitheme(programDefs *prgs) {
  /*
    Main loop : cycle through all program definitions {

      If file exists :
        - Allocate list of compiled regexprs
        - loop : cycle through regexprs and compile them {}
        - Initialize and open cfg_file
        loop : cycle through lines of cfg_file {
          loop : cycle through all compiled regexprs {
            If compiled regexpr matches :
              - Handle match
              - Increment match count
            Else :
              - Handle no-match
            Endif
          }
        }
        - Free allocated memory
      Endif

    }

   */
  FILE *cfg_file;
  char *buff;

  int err_code = 0;
  size_t err_offset = 0;
  PCRE2_UCHAR *err_msg = NULL;

  pcre2_code **tok_reg_comped;
  pcre2_match_data *match_data = NULL;

  size_t num_matches = 0;

  /* Main loop cycles through all programs */
  for (size_t i=0; i < prgs->used; i++) {
    if (verboseMode) {
      printf(KDEFAULT PACKAGE ": " BKBLU "ABOUT TO EXECUTE: %s SCRIPT\n"
             KDEFAULT "=======================================\n", prgs->progs[i]->name);
    }

    /* if file exists */
    /* TODO: use a more platform independant alternative to access() */
    if (prgs->progs[i]->path != NULL && strcmp("", prgs->progs[i]->path) != 0 && access(prgs->progs[i]->path, F_OK) != -1) {

      /* init and compile regular expressions (tokens) */
      /* allocate list of compiled regexprs */
      tok_reg_comped = calloc(prgs->progs[i]->tokens->used +1, sizeof(pcre2_code *));
      /* cycle through regexprs and compile them */
      for (size_t ii=0; ii < prgs->progs[i]->tokens->used; ii++) {
        VERBOSE_PRINT_VALUE(%s, prgs->progs[i]->tokens->items[ii]);

        tok_reg_comped[ii] = NULL;

        tok_reg_comped[ii] = pcre2_compile((PCRE2_SPTR)prgs->progs[i]->tokens->items[ii], PCRE2_ZERO_TERMINATED, 0, &err_code, &err_offset, NULL);
        if (tok_reg_comped[ii] == NULL) {
          fprintf(stderr, BKRED "REGEX COMPILATION FAILED!!\n" KDEFAULT);
          err_msg = calloc(256, sizeof(PCRE2_UCHAR));
          pcre2_get_error_message(err_code, err_msg, 256);
          fprintf(stderr, BKRED "%s\n" KDEFAULT, err_msg);
          free(err_msg);
        }

      } /* End of init and compile regular expressions (tokens) */

      /* init and open cfg_file */
      cfg_file = fopen(prgs->progs[i]->path, "r");
      buff = malloc(256);

      /* Cycles through lines of cfg_file */
      while (fgets(buff, 256, cfg_file) != NULL) {
        if (buff[0] == '\n') {
          continue;
        } else {
          for (size_t ii=0; ii < prgs->progs[i]->tokens->used; ii++) {                   /* Cycle through all regexs */
            match_data = pcre2_match_data_create_from_pattern(tok_reg_comped[ii], NULL); /* Create match data */
            err_code = pcre2_match(tok_reg_comped[ii],
                                   (PCRE2_SPTR)buff,
                                   strlen(buff),
                                   0,
                                   0,
                                   match_data,
                                   NULL);                                               /* Test for match */
            pcre2_match_data_free(match_data);                                          /* Dispose of the match data right away */
            if (err_code > 0) {
              WARNING_PRINT("FOUND MATCH!!!!!");

              /* Handle match found */

              num_matches++;

            } else if (err_code != -1){                                                 /* Error code -1 means just no match */
              VERBOSE_PRINT_VALUE(%d, err_code);
              err_msg = malloc(256);
              pcre2_get_error_message(err_code, err_msg, 256);
              fprintf(stderr, BKRED "%s\n" KDEFAULT, err_msg);
              free(err_msg);
            }
          }
        }
      } /* end of cfg_file loop */

      if (verboseMode)
        printf(PACKAGE ": " BKBLU "%lu matches found\n" KDEFAULT, (unsigned long)num_matches);

      /* free allocated heap*/
      for (size_t ii=0; ii < prgs->progs[i]->tokens->used; ii++)
        pcre2_code_free(tok_reg_comped[ii]);
      free(tok_reg_comped);

      tok_reg_comped = NULL;
      free(buff);
      buff = NULL;
      fclose(cfg_file);
      cfg_file = NULL;
    } else {
      fprintf(stderr, BKRED "File %s does not exist!\n" KDEFAULT, prgs->progs[i]->path);
    }


  } /* End of main loop */

}

