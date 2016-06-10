#include "marpaEBNF.h"
#include "genericLogger.h"

int main(int argc, char **argv) {
  int rci = 0;
  genericLogger_t  *genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE);
  marpaEBNFOption_t marpaEBNFOption = { genericLoggerp };
  marpaEBNF_t *marpaEBNFp;

  marpaEBNFp = marpaEBNF_newp(&marpaEBNFOption);
  if (marpaEBNFp == NULL) {
    rci = 1;
    goto end;
  }

  /* Use EBNF to parse itself */
  if (marpaEBNF_grammarb(marpaEBNFp, "x") == 0) {
    rci = 1;
    goto end;
  }

 end:
  if (marpaEBNFp != NULL) {
    marpaEBNF_freev(marpaEBNFp);
  }

  GENERICLOGGER_FREE(genericLoggerp);
}
