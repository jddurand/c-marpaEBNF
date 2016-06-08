#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "config.h"
#include "marpaWrapper.h"
#include "marpaEBNF.h"

typedef struct marpaEBNFSymbol {
  char                             *descriptions;
  marpaWrapperGrammarSymbolOption_t option;
} marpaEBNFSymbol_t;

#define MARPAEBNF_SIZEOF_ARRAY(x) (sizeof(x)/sizeof((x)[0]))

/* List of all symbols of the EBNF grammar as per ISO/IEC 14977:1996 */
static marpaEBNFSymbol_t marpaEBNFSymbolArray[] = {
  /* ------------------------------------------------------------------------------------------
    descriptions                      { terminalb, startb,                          eventSeti }
    -------------------------------------------------------------------------------------------
  */
  /*
   *                                TERMINAL CHARACTERS
   */
  {"letter",                          {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"decimal digit",                   {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"concatenate symbol",              {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"defining symbol",                 {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"definition separator symbol",     {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"end comment symbol",              {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"end group symbol",                {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"end option symbol",               {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"end repeat symbol",               {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"except symbol",                   {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"first quote symbol",              {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"repetition symbol",               {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"second quote symbol",             {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"special sequence symbol",         {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"start comment symbol",            {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"start group symbol",              {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"start option symbol",             {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"start repeat symbol",             {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"terminator symbol",               {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"other character",                 {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"space character",                 {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"horizontal tabulation character", {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"new line",                        {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"vertical tabulation character",   {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"form feed",                       {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   *                              NON-TERMINAL CHARACTERS
   */
  {"terminal character",              {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"gap free symbol",                 {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"terminal string",                 {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"first terminal character",        {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"second terminal character",       {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"gap separator",                   {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"syntax",                          {         0,      1, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },  /* START */
  {"commentless symbol",              {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"integer",                         {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"meta identifier",                 {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"meta identifier character",       {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"special sequence",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"special sequence character",      {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"comment symbol",                  {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"bracketed textual comment",       {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"syntax rule",                     {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"definitions list",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"single definition",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"syntactic term",                  {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"syntactic exception",             {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"syntactic factor",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"syntactic primary",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"optional sequence",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"repeated sequence",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"grouped sequence",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"empty sequence",                  {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {"comment",                         {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
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
    if (symboli != i) {
      goto err;
    }
  }

  return 1;

 err:
  return 0;
}
