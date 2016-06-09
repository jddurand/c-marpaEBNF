#include "marpaEBNF.h"
#include "genericLogger.h"

int main(int argc, char **argv) {
  marpaWrapperGrammarOption_t marpaWrapperGrammarOption = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE),
							    0 /* warningIsErrorb */,
							    0 /* warningIsIgnoredb */
  };
  marpaEBNFOption_t marpaEBNFOption = { &marpaWrapperGrammarOption };

  marpaEBNF_t *marpaEBNFp = marpaEBNF_newp(&marpaEBNFOption);
  marpaEBNF_freev(marpaEBNFp);

  GENERICLOGGER_FREE(marpaWrapperGrammarOption.genericLoggerp)
}
