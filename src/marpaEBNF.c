#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "config.h"
#include "marpaWrapper.h"
#include "marpaEBNF.h"

/* List of all symbols of the EBNF grammar as per ISO/IEC 14977:1996 */
typedef enum marpaEBNFSymbolIndice {
  LETTER = 0,
  DECIMAL_DIGIT,
  CONCATENATE_SYMBOL,
  DEFINING_SYMBOL,
  DEFINITION_SEPARATOR_SYMBOL,
  END_COMMENT_SYMBOL,
  END_GROUP_SYMBOL,
  END_OPTION_SYMBOL,
  END_REPEAT_SYMBOL,
  EXCEPT_SYMBOL,
  FIRST_QUOTE_SYMBOL,
  REPETITION_SYMBOL,
  SECOND_QUOTE_SYMBOL,
  SPECIAL_SEQUENCE_SYMBOL,
  START_COMMENT_SYMBOL,
  START_GROUP_SYMBOL,
  START_OPTION_SYMBOL,
  START_REPEAT_SYMBOL,
  TERMINAL_SYMBOL,
  OTHER_CHARACTER,
  SPACE_CHARACTER,
  HORIZONTAL_TABULATION_CHARACTER,
  NEW_LINE,
  VERTICAL_TABULATION_CHARACTER,
  FORM_FEED,
  TERMINAL_CHARACTER,
  GAP_FREE_SYMBOL,
  TERMINAL_STRING,
  FIRST_TERMINAL_CHARACTER,
  SECOND_TERMINAL_CHARACTER,
  GAP_SEPARATOR,
  SYNTAX,
  COMMENTLESS_SYMBOL,
  OPTIONAL_SEQUENCE,
  REPEATED_SEQUENCE,
  GROUPED_SEQUENCE,
  EMPTY_SEQUENCE,
  INTEGER,
  META_IDENTIFIER,
  META_IDENTIFIER_CHARACTER,
  SPECIAL_SEQUENCE,
  SPECIAL_SEQUENCE_CHARACTER,
  COMMENT_SYMBOL,
  BRACKETED_TEXTUAL_COMMENT,
  SYNTAX_RULE,
  DEFINITIONS_LIST,
  SINGLE_DEFINITION,
  SYNTACTIC_TERM,
  SYNTACTIC_EXCEPTION,
  SYNTACTIC_FACTOR,
  SYNTACTIC_PRIMARY,
  COMMENT,
  _marpaEBNFSymbolIndice_MAX
} marpaEBNFSymbolIndice_e;

/* Internally, EBNF is nothing else but an instance of marpaWrapperGrammar_t along */
/* with symbols and rules book-keeping.                                            */
struct marpaEBNF {
  marpaWrapperGrammar_t *marpaWrapperGrammarp;
  int                   symbolip[_marpaEBNFSymbolIndice_MAX];
  int                   *ruleip;
};

static inline short _marpaEBNF_internalGrammarb(marpaEBNF_t *marpaEBNFp);

static marpaEBNFOption_t marpaEBNFOptionDefault = {
  NULL /* marpaWrapperGrammarOptionp */
};

/****************************************************************************/
marpaEBNF_t *marpaEBNF_newp(marpaEBNFOption_t *marpaEBNFOptionp)
/****************************************************************************/
{
  marpaEBNF_t                *marpaEBNFp;

  if (marpaEBNFOptionp == NULL) {
    marpaEBNFOptionp = &marpaEBNFOptionDefault;
  }

  marpaEBNFp = malloc(sizeof(marpaEBNF_t));
  if (marpaEBNFp == NULL) {
    goto err;
  }

  marpaEBNFp->marpaWrapperGrammarp = marpaWrapperGrammar_newp(marpaEBNFOptionp->marpaWrapperGrammarOptionp);
  if (marpaEBNFp->marpaWrapperGrammarp == NULL) {
    goto err;
  }

  return marpaEBNFp;

 err:
  marpaEBNF_freev(marpaEBNFp);
  return NULL;
}

/****************************************************************************/
void marpaEBNF_freev(marpaEBNF_t *marpaEBNFp)
/****************************************************************************/
{
  if (marpaEBNFp != NULL) {
    if (marpaEBNFp->marpaWrapperGrammarp != NULL) {
      marpaWrapperGrammar_freev(marpaEBNFp->marpaWrapperGrammarp);
    }
    if (marpaEBNFp->symbolip != NULL) {
      free(marpaEBNFp->symbolip);
    }
    if (marpaEBNFp->ruleip != NULL) {
      free(marpaEBNFp->ruleip);
    }
  }
}

/****************************************************************************/
static inline short _marpaEBNF_internalGrammarb(marpaEBNF_t *marpaEBNFp)
/****************************************************************************/
{
  int i;

  for (i = 0; i < _marpaEBNFSymbolIndice_MAX; i++) {
    marpaEBNFp->symbolip[i] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaEBNFp->marpaWrapperGrammarp);
    if (marpaEBNFp->symbolip[i] < 0) {
      goto err;
    }
  }

  return 1;

 err:
  return 0;
}
