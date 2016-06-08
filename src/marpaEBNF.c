#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "config.h"
#include "marpaWrapper.h"
#include "marpaEBNF.h"

#define MARPAEBNF_SIZEOF_ARRAY(x) (sizeof(x)/sizeof((x)[0]))

/* Convenience enum, also used to make sure that symbol id is the enum -; */
typedef enum marpaEBNFSymbolEnum {
  LETTER,
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
  TERMINATOR_SYMBOL,
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
  OPTIONAL_SEQUENCE,
  REPEATED_SEQUENCE,
  GROUPED_SEQUENCE,
  EMPTY_SEQUENCE,
  COMMENT
} marpaEBNFSymbol_e;

typedef struct marpaEBNFSymbol {
  marpaEBNFSymbol_e                 symboli;
  char                             *descriptions;
  marpaWrapperGrammarSymbolOption_t option;
} marpaEBNFSymbol_t;

/* List of all symbols of the EBNF grammar as per ISO/IEC 14977:1996 */
static marpaEBNFSymbol_t marpaEBNFSymbolArray[] = {
  /* ------------------------------------------------------------------------------------------
    descriptions                      { terminalb, startb,                          eventSeti }
    -------------------------------------------------------------------------------------------
  */
  /*
   *                                TERMINAL CHARACTERS
   */
  {LETTER,                          "letter",                          {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {DECIMAL_DIGIT,                   "decimal digit",                   {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {CONCATENATE_SYMBOL,              "concatenate symbol",              {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {DEFINING_SYMBOL,                 "defining symbol",                 {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {DEFINITION_SEPARATOR_SYMBOL,     "definition separator symbol",     {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {END_COMMENT_SYMBOL,              "end comment symbol",              {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {END_GROUP_SYMBOL,                "end group symbol",                {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {END_OPTION_SYMBOL,               "end option symbol",               {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {END_REPEAT_SYMBOL,               "end repeat symbol",               {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {EXCEPT_SYMBOL,                   "except symbol",                   {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {FIRST_QUOTE_SYMBOL,              "first quote symbol",              {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {REPETITION_SYMBOL,               "repetition symbol",               {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SECOND_QUOTE_SYMBOL,             "second quote symbol",             {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SPECIAL_SEQUENCE_SYMBOL,         "special sequence symbol",         {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {START_COMMENT_SYMBOL,            "start comment symbol",            {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {START_GROUP_SYMBOL,              "start group symbol",              {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {START_OPTION_SYMBOL,             "start option symbol",             {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {START_REPEAT_SYMBOL,             "start repeat symbol",             {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {TERMINATOR_SYMBOL,               "terminator symbol",               {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {OTHER_CHARACTER,                 "other character",                 {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SPACE_CHARACTER,                 "space character",                 {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {HORIZONTAL_TABULATION_CHARACTER, "horizontal tabulation character", {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {NEW_LINE,                        "new line",                        {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {VERTICAL_TABULATION_CHARACTER,   "vertical tabulation character",   {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {FORM_FEED,                       "form feed",                       {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   *                              NON-TERMINAL CHARACTERS
   */
  {TERMINAL_CHARACTER,              "terminal character",              {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {GAP_FREE_SYMBOL,                 "gap free symbol",                 {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {TERMINAL_STRING,                 "terminal string",                 {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {FIRST_TERMINAL_CHARACTER,        "first terminal character",        {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SECOND_TERMINAL_CHARACTER,       "second terminal character",       {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {GAP_SEPARATOR,                   "gap separator",                   {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTAX,                          "syntax",                          {         0,      1, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },  /* START */
  {COMMENTLESS_SYMBOL,              "commentless symbol",              {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {INTEGER,                         "integer",                         {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {META_IDENTIFIER,                 "meta identifier",                 {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {META_IDENTIFIER_CHARACTER,       "meta identifier character",       {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SPECIAL_SEQUENCE,                "special sequence",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SPECIAL_SEQUENCE_CHARACTER,      "special sequence character",      {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {COMMENT_SYMBOL,                  "comment symbol",                  {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {BRACKETED_TEXTUAL_COMMENT,       "bracketed textual comment",       {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTAX_RULE,                     "syntax rule",                     {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {DEFINITIONS_LIST,                "definitions list",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SINGLE_DEFINITION,               "single definition",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTACTIC_TERM,                  "syntactic term",                  {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTACTIC_EXCEPTION,             "syntactic exception",             {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTACTIC_FACTOR,                "syntactic factor",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTACTIC_PRIMARY,               "syntactic primary",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {OPTIONAL_SEQUENCE,               "optional sequence",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {REPEATED_SEQUENCE,               "repeated sequence",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {GROUPED_SEQUENCE,                "grouped sequence",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {EMPTY_SEQUENCE,                  "empty sequence",                  {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {COMMENT,                         "comment",                         {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
};

/* Internally, EBNF is nothing else but an instance of marpaWrapperGrammar_t along */
/* with symbols and rules book-keeping.                                            */
struct marpaEBNF {
  marpaWrapperGrammar_t *marpaWrapperGrammarp;
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
  if ( _marpaEBNF_internalGrammarb(marpaEBNFp) == 0) {
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
  }
}

/****************************************************************************/
static inline short _marpaEBNF_internalGrammarb(marpaEBNF_t *marpaEBNFp)
/****************************************************************************/
{
  int i, symboli;

  /* Declare all the symbols */
  for (i = 0; i < (sizeof(marpaEBNFSymbolArray) / sizeof(marpaEBNFSymbolArray[0])); i++) {
    symboli = marpaWrapperGrammar_newSymboli(marpaEBNFp->marpaWrapperGrammarp, &(marpaEBNFSymbolArray[i].option));
    /* We take advantage of the fact that symbols always start at 0 with marpa */
    if (symboli != marpaEBNFSymbolArray[i].symboli) {
      goto err;
    }
  }

  return 1;

 err:
  return 0;
}
