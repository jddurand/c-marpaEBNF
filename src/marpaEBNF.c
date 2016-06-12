#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "config.h"
#include "genericStack.h"
#include "marpaWrapper.h"
#include "marpaEBNF.h"

#define MARPAEBNF_LENGTH_ARRAY(x) (sizeof(x)/sizeof((x)[0]))

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
  COMMENT,
  /* Internal symbols */
  _SPECIAL_SEQUENCE_NULLED,
  _TERMINAL_STRING_NULLED,

  _GAP_FREE_SYMBOL_ALT_1,
  _GAP_FREE_SYMBOL_ALT_1_1,
  _GAP_FREE_SYMBOL_ALT_1_2,
  _GAP_FREE_SYMBOL_ALT_1_1_MARKER,
  _GAP_FREE_SYMBOL_ALT_1_2_MARKER,

  _FIRST_TERMINAL_CHARACTER_MANY,
  _SECOND_TERMINAL_CHARACTER_MANY,

  _FIRST_TERMINAL_CHARACTER_ALT_1,
  _FIRST_TERMINAL_CHARACTER_ALT_1_MARKER,
  _FIRST_TERMINAL_CHARACTER_ALT_2,
  _FIRST_TERMINAL_CHARACTER_ALT_2_MARKER,

  _SECOND_TERMINAL_CHARACTER_ALT_1,
  _SECOND_TERMINAL_CHARACTER_ALT_1_MARKER,
  _SECOND_TERMINAL_CHARACTER_ALT_2,
  _SECOND_TERMINAL_CHARACTER_ALT_2_MARKER,

  _GAP_SEPARATOR_ANY,
  _GAP_FREE_SYMBOL_MANY,

  _COMMENTLESS_SYMBOL_ALT_1,
  _COMMENTLESS_SYMBOL_ALT_1_1,
  _COMMENTLESS_SYMBOL_ALT_1_2,
  _COMMENTLESS_SYMBOL_ALT_1_1_MARKER,
  _COMMENTLESS_SYMBOL_ALT_1_2_MARKER,

  _DECIMAL_DIGIT_MANY,

  _META_IDENTIFIER_CHARACTER_ANY,

  _SPECIAL_SEQUENCE_CHARACTER_ANY,

  _SPECIAL_SEQUENCE_CHARACTER_ALT_1,
  _SPECIAL_SEQUENCE_CHARACTER_ALT_1_MARKER,
  _SPECIAL_SEQUENCE_CHARACTER_ALT_2,
  _SPECIAL_SEQUENCE_CHARACTER_ALT_2_MARKER,

  _COMMENT_SYMBOL_ANY,

  _BRACKETED_TEXTUAL_COMMENT_ANY,
  _SYNTAX_UNIT,
  _SYNTAX_UNIT_MANY,
  _SYNTAX_01,

  _SYNTAX_RULE_MANY,
  _SYNTAX_02,

  _DEFINITIONS_SEQUENCE,
} marpaEBNFSymbol_e;

typedef struct marpaEBNFSymbol {
  marpaEBNFSymbol_e                 symboli;
  short                             markerb;
  int                               markedi;
  short                             matchedb;   /* Initialized to 0 at each lex lookup */
  int                               eventSeti;  /* Initialized to 0 at each lex lookup */
  short                             exceptionb;
  char                             *descriptions;
  marpaWrapperGrammarSymbolOption_t option;
} marpaEBNFSymbol_t;

#define MARPAEBNF_SYMBOL_IS_MARKER(marpaEBNFSymbolp) ((marpaEBNFSymbolp)->markerb != 0)
#define MARPAEBNF_SYMBOL_IS_MARKER_OK(marpaEBNFSymbolp) (MARPAEBNF_SYMBOL_IS_MARKER(marpaEBNFSymbolp) && ((marpaEBNFSymbolp)->exceptionb == 0))
#define MARPAEBNF_SYMBOL_IS_MARKER_KO(marpaEBNFSymbolp) (MARPAEBNF_SYMBOL_IS_MARKER(marpaEBNFSymbolp) && ((marpaEBNFSymbolp)->exceptionb != 0))

typedef struct marpaEBNFRule {
  marpaWrapperGrammarRuleOption_t   option;
  int                               lhsSymboli;
  size_t                            rhsSymboll;
  int                               rhsSymbolip[20]; /* Flexible array and ANSI-C are not friends */
} marpaEBNFRule_t;

/* List of all symbols of the EBNF grammar as per ISO/IEC 14977:1996 */
static marpaEBNFSymbol_t marpaEBNFSymbolArray[] = {
  /* -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                    markerb, markedi, matchedb, eventSeti, exceptionb, descriptions                          { terminalb, startb,                          eventSeti } }
  -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {LETTER,                          0,       0,        0,         0,          0, "<letter>",                          {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {DECIMAL_DIGIT,                   0,       0,        0,         0,          0, "<decimal digit>",                   {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {CONCATENATE_SYMBOL,              0,       0,        0,         0,          0, "<concatenate symbol>",              {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {DEFINING_SYMBOL,                 0,       0,        0,         0,          0, "<defining symbol>",                 {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {DEFINITION_SEPARATOR_SYMBOL,     0,       0,        0,         0,          0, "<definition separator symbol>",     {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {END_COMMENT_SYMBOL,              0,       0,        0,         0,          0, "<end comment symbol>",              {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {END_GROUP_SYMBOL,                0,       0,        0,         0,          0, "<end group symbol>",                {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {END_OPTION_SYMBOL,               0,       0,        0,         0,          0, "<end option symbol>",               {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {END_REPEAT_SYMBOL,               0,       0,        0,         0,          0, "<end repeat symbol>",               {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {EXCEPT_SYMBOL,                   0,       0,        0,         0,          0, "<except symbol>",                   {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } }, /* To detect meta identifier */
  {FIRST_QUOTE_SYMBOL,              0,       0,        0,         0,          0, "<first quote symbol>",              {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {REPETITION_SYMBOL,               0,       0,        0,         0,          0, "<repetition symbol>",               {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SECOND_QUOTE_SYMBOL,             0,       0,        0,         0,          0, "<second quote symbol>",             {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SPECIAL_SEQUENCE_SYMBOL,         0,       0,        0,         0,          0, "<special sequence symbol>",         {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {START_COMMENT_SYMBOL,            0,       0,        0,         0,          0, "<start comment symbol>",            {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {START_GROUP_SYMBOL,              0,       0,        0,         0,          0, "<start group symbol>",              {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {START_OPTION_SYMBOL,             0,       0,        0,         0,          0, "<start option symbol>",             {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {START_REPEAT_SYMBOL,             0,       0,        0,         0,          0, "<start repeat symbol>",             {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {TERMINATOR_SYMBOL,               0,       0,        0,         0,          0, "<terminator symbol>",               {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {OTHER_CHARACTER,                 0,       0,        0,         0,          0, "<other character>",                 {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SPACE_CHARACTER,                 0,       0,        0,         0,          0, "<space character>",                 {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {HORIZONTAL_TABULATION_CHARACTER, 0,       0,        0,         0,          0, "<horizontal tabulation character>", {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {NEW_LINE,                        0,       0,        0,         0,          0, "<new line>",                        {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {VERTICAL_TABULATION_CHARACTER,   0,       0,        0,         0,          0, "<vertical tabulation character>",   {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {FORM_FEED,                       0,       0,        0,         0,          0, "<form feed>",                       {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },

  /* -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                    markerb, markedi, matchedb, eventSeti, exceptionb, descriptions                          { terminalb, startb,                          eventSeti } }
  -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {TERMINAL_CHARACTER,              0,       0,        0,         0,          0, "<terminal character>",              {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {GAP_FREE_SYMBOL,                 0,       0,        0,         0,          0, "<gap free symbol>",                 {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {TERMINAL_STRING,                 0,       0,        0,         0,          0, "<terminal string>",                 {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } }, /* To detect end of terminal string */
  {FIRST_TERMINAL_CHARACTER,        0,       0,        0,         0,          0, "<first terminal character>",        {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SECOND_TERMINAL_CHARACTER,       0,       0,        0,         0,          0, "<second terminal character>",       {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {GAP_SEPARATOR,                   0,       0,        0,         0,          0, "<gap separator>",                   {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTAX,                          0,       0,        0,         0,          0, "<syntax>",                          {         0,      1, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },  /* START */
  {COMMENTLESS_SYMBOL,              0,       0,        0,         0,          0, "<commentless symbol>",              {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {INTEGER,                         0,       0,        0,         0,          0, "<integer>",                         {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {META_IDENTIFIER,                 0,       0,        0,         0,          0, "<meta identifier>",                 {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {META_IDENTIFIER_CHARACTER,       0,       0,        0,         0,          0, "<meta identifier character>",       {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SPECIAL_SEQUENCE,                0,       0,        0,         0,          0, "<special sequence>",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } }, /* To detect end of special sequence */
  {SPECIAL_SEQUENCE_CHARACTER,      0,       0,        0,         0,          0, "<special sequence character>",      {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {COMMENT_SYMBOL,                  0,       0,        0,         0,          0, "<comment symbol>",                  {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {BRACKETED_TEXTUAL_COMMENT,       0,       0,        0,         0,          0, "<bracketed textual comment>",       {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTAX_RULE,                     0,       0,        0,         0,          0, "<syntax rule>",                     {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {DEFINITIONS_LIST,                0,       0,        0,         0,          0, "<definitions list>",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SINGLE_DEFINITION,               0,       0,        0,         0,          0, "<single definition>",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTACTIC_TERM,                  0,       0,        0,         0,          0, "<syntactic term>",                  {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTACTIC_EXCEPTION,             0,       0,        0,         0,          0, "<syntactic exception>",             {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } }, /* no meta identifier! */
  {SYNTACTIC_FACTOR,                0,       0,        0,         0,          0, "<syntactic factor>",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTACTIC_PRIMARY,               0,       0,        0,         0,          0, "<syntactic primary>",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {OPTIONAL_SEQUENCE,               0,       0,        0,         0,          0, "<optional sequence>",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {REPEATED_SEQUENCE,               0,       0,        0,         0,          0, "<repeated sequence>",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {GROUPED_SEQUENCE,                0,       0,        0,         0,          0, "<grouped sequence>",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {EMPTY_SEQUENCE,                  0,       0,        0,         0,          0, "<empty sequence>",                  {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {COMMENT,                         0,       0,        0,         0,          0, "<comment>",                         {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
    Note: handling of the exception with standard BNF
    *
    * Given a rule
    *   X = A - B
    *
    * it is rewriten as:
    *   X = A MARKER_A    # With a completion or nulled event on A
    *   X = B MARKER_B    # With a completion or nulled event on B
    *
    * During lexing phase:
    * IF (MARKER_A was predicted) THEN
    *   IF (there is a nulled event on A) THEN
    *     we insert MARKER_A
    *   ELSE IF (there is a completion event on A) THEN
    *     IF (there is a prediction on A) THEN
    *       IF (A was not matched) THEN
    *         we insert MARKER_A
    *       ENDIF
    *     ELSE
    *       we insert MARKER_A
    *     ENDIF
    *   ENDIF
    * ENDIF
    */

  /* ---------------- */
  /* Internal symbols */
  /* ---------------- */
  /*
   * special sequence = special sequence symbol, {special sequence character}, special sequence symbol;
   *
   * is revisited to:
   *
   * special sequence = special sequence symbol, _SPECIAL_SEQUENCE_NULLED, {special sequence character}, special sequence symbol;
   *
   * to detect special sequence context. Plus a completion symbol on special sequence.
   */
  /* ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                   markerb,                   markedi, matchedb, eventSeti, exceptionb, descriptions                         { terminalb, startb,                          eventSeti } }
  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_SPECIAL_SEQUENCE_NULLED,       0,                         0,        0,         0,          0, "<terminal string nulled>",          {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NULLED } },
  /*
   * terminal string = first quote symbol, first terminal character, {first terminal character}, first quote symbol
   *                 | second quote symbol, second terminal character, {second terminal character}, second quote symbol;
   *
   * is revisited to:
   *
   * terminal string = first quote symbol, _TERMINAL_STRING_NULLED, first terminal character, {first terminal character}, first quote symbol
   *                 | second quote symbol, _TERMINAL_STRING_NULLED, second terminal character, {second terminal character}, second quote symbol;
   *
   * to detect terminal string context. Plus a completion symbol on terminal string.
   */
  /* ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                   markerb,                   markedi, matchedb, eventSeti, exceptionb, descriptions                         { terminalb, startb,                          eventSeti } }
  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_TERMINAL_STRING_NULLED,        0,                         0,        0,         0,          0, "<terminal string nulled>",          {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NULLED } },
  /*
   * gap free symbol = terminal character - (first quote symbol | second quote symbol)
   *                   | terminal string
   *
   * is revisited to:
   *
   * gap free symbol         = gap free symbol alt 1
   *                         | terminal string
   *
   * gap free symbol alt 1   = gap free symbol alt 1.1 _GAP_FREE_SYMBOL_ALT_1_1_MARKER
   *                         | gap free symbol alt 1.2 _GAP_FREE_SYMBOL_ALT_1_2_MARKER
   *
   * + COMPLETION EVENT ON gap free symbol alt 1.1
   * gap free symbol alt 1.1 = terminal character
   *
   * + COMPLETION EVENT ON <gap free symbol alt 1.2>
   * gap free symbol alt 1.2 = first quote symbol
   *                         | second quote symbol
   */
  /* ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                   markerb,                   markedi, matchedb, eventSeti, exceptionb, descriptions                         { terminalb, startb,                          eventSeti } }
  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_GAP_FREE_SYMBOL_ALT_1,          0,                        0,        0,         0,          0, "<gap free symbol alt 1>",           {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_GAP_FREE_SYMBOL_ALT_1_1,        0,                        0,        0,         0,          0, "<gap free symbol alt 1.1>",         {         0,      0, -1 /* All Events */                } },
  {_GAP_FREE_SYMBOL_ALT_1_2,        0,                        0,        0,         0,          0, "<gap free symbol alt 1.2>",         {         0,      0, -1 /* All Events */                } },
  {_GAP_FREE_SYMBOL_ALT_1_1_MARKER, 1, _GAP_FREE_SYMBOL_ALT_1_1,        0,         0,          0, "<gap free symbol alt 1.1 marker>",  {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_GAP_FREE_SYMBOL_ALT_1_2_MARKER, 1, _GAP_FREE_SYMBOL_ALT_1_2,        0,         0,          1, "<gap free symbol alt 1.2 marker>",  {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * terminal string = first quote symbol, first terminal character, {first terminal character}, first quote symbol
   *                 | second quote symbol, second terminal character, {second terminal character}, second quote symbol
   *
   * is revisited to:
   *
   * terminal string = first quote symbol, _FIRST_TERMINAL_CHARACTER_MANY, first quote symbol
   *                 | second quote symbol, _SECOND_TERMINAL_CHARACTER_MANY, second quote symbol
   *
   * _FIRST_TERMINAL_CHARACTER_MANY = <first terminal character>+
   * _SECOND_TERMINAL_CHARACTER_MANY = <second terminal character>+
   *
   */
  /* ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                   markerb,                   markedi, matchedb, eventSeti, exceptionb, descriptions                         { terminalb, startb,                          eventSeti } }
  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_FIRST_TERMINAL_CHARACTER_MANY,  0,                        0,        0,         0,          0, "<first terminal character +>",      {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SECOND_TERMINAL_CHARACTER_MANY, 0,                        0,        0,         0,          0, "<second terminal character +>",     {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * first terminal character = terminal character - first quote symbol
   *
   * is revisited to:
   *
   * first terminal character = first terminal character alt 1 _FIRST_TERMINAL_CHARACTER_ALT_1_MARKER
   * first terminal character = first terminal character alt 2 _FIRST_TERMINAL_CHARACTER_ALT_2_MARKER
   * first terminal character alt 1 = terminal character
   * first terminal character alt 2 = first quote symbol
   */
  /* -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                           markerb,                         markedi, matchedb, eventSeti, exceptionb, descriptions                               { terminalb, startb,                          eventSeti } }
  -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_FIRST_TERMINAL_CHARACTER_ALT_1,        0,                               0,        0,         0,          0, "<first terminal character alt 1>",        {         0,      0, -1 /* All Events */                } },
  {_FIRST_TERMINAL_CHARACTER_ALT_1_MARKER, 1, _FIRST_TERMINAL_CHARACTER_ALT_1,        0,         0,          0, "<first terminal character alt 1 marker>", {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_FIRST_TERMINAL_CHARACTER_ALT_2,        0,                               0,        0,         0,          0, "<first terminal character alt 2>",        {         0,      0, -1 /* All Events */ } },
  {_FIRST_TERMINAL_CHARACTER_ALT_2_MARKER, 1, _FIRST_TERMINAL_CHARACTER_ALT_2,        0,         0,          1, "<first terminal character alt 2 marker>", {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * second terminal character = terminal character - second quote symbol
   *
   * is revisited to:
   *
   * second terminal character = second terminal character alt 1 _SECOND_TERMINAL_CHARACTER_ALT_1_MARKER
   * second terminal character = second terminal character alt 2 _SECOND_TERMINAL_CHARACTER_ALT_2_MARKER
   * second terminal character alt 1 = terminal character
   * second terminal character alt 2 = second quote symbol
   */
  /* --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                           markerb,                           markedi, matchedb, eventSeti, exceptionb, descriptions                                { terminalb, startb,                          eventSeti } }
  ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_SECOND_TERMINAL_CHARACTER_ALT_1,        0,                                0,        0,         0,          0, "<second terminal character alt 1>",        {         0,      0, -1 /* All Events */                } },
  {_SECOND_TERMINAL_CHARACTER_ALT_1_MARKER, 1, _SECOND_TERMINAL_CHARACTER_ALT_1,        0,         0,          0, "<second terminal character alt 1 marker>", {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SECOND_TERMINAL_CHARACTER_ALT_2,        0,                                0,        0,         0,          0, "<second terminal character alt 2>",        {         0,      0, -1 /* All Events */                } },
  {_SECOND_TERMINAL_CHARACTER_ALT_2_MARKER, 1, _SECOND_TERMINAL_CHARACTER_ALT_2,        0,         0,          1, "<second terminal character alt 2 marker>", {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * syntax = {gap separator}, gap free symbol, {gap separator}, {gap free symbol, {gap separator}}
   *
   * is revisited to:
   *
   * <gap separator any> = <gap separator>*
   * <gap free symbol many> = <gap free symbol>+ separator => <gap separator> proper => 0
   * <syntax> = <gap separator any> <gap free symbol many>
   */
  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                   markerb, markedi, matchedb, eventSeti, exceptionb, descriptions                          { terminalb, startb,                          eventSeti } }
  ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_GAP_SEPARATOR_ANY,              0,       0,       0,         0,          0, "<gap separator *>",                  {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_GAP_FREE_SYMBOL_MANY,           0,       0,       0,         0,          0, "<gap free symbol +>",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * commentless symbol = terminal character
   *                       - (letter
   *                         | decimal digit
   *                         | first quote symbol
   *                         | second quote symbol
   *                         | start comment symbol
   *                         | end comment symbol
   *                         | special sequence symbol
   *                         | other character)
   *                     | meta identifier
   *                     | integer
   *                     | terminal string
   *                     | special sequence
   *
   * is revisited to:
   *
   * commentless symbol = commentless symbol alt 1
   *                     | meta identifier
   *                     | integer
   *                     | terminal string
   *                     | special sequence
   * commentless symbol alt 1 = commentless symbol alt 1.1 _COMMENTLESS_SYMBOL_ALT_1_1_MARKER
   *                             commentless symbol alt 1.2 _COMMENTLESS_SYMBOL_ALT_1_2_MARKER
   * commentless symbol alt 1.1 = terminal character
   * commentless symbol alt 1.2 = letter
   *                             | decimal digit
   *                             | first quote symbol
   *                             | second quote symbol
   *                             | start comment symbol
   *                             | end comment symbol
   *                             | special sequence symbol
   *                             | other character)
   *
   */
  /* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                       markerb,                     markedi, matchedb, eventSeti, exceptionb, descriptions                            { terminalb, startb,                          eventSeti } }
  --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_COMMENTLESS_SYMBOL_ALT_1,          0,                           0,        0,         0,          0, "<commentless symbol alt 1>",           {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_COMMENTLESS_SYMBOL_ALT_1_1,        0,                           0,        0,         0,          0, "<commentless symbol alt 1.1>",         {         0,      0, -1 /* All Events */                } },
  {_COMMENTLESS_SYMBOL_ALT_1_2,        0,                           0,        0,         0,          0, "<commentless symbol alt 1.2>",         {         0,      0, -1 /* All Events */                } },
  {_COMMENTLESS_SYMBOL_ALT_1_1_MARKER, 1, _COMMENTLESS_SYMBOL_ALT_1_1,        0,         0,          0, "<commentless symbol alt 1.1 marker>",  {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_COMMENTLESS_SYMBOL_ALT_1_2_MARKER, 1, _COMMENTLESS_SYMBOL_ALT_1_2,        0,         0,          1, "<commentless symbol alt 1.2 marker>",  {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * integer = decimal digit, {decimal digit}
   *
   * is revisited to:
   *
   * <integer> = <decimal digit many>
   * <decimal digit many> = <decimal digit>+
   */
  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                   markerb, markedi, matchedb, eventSeti, exceptionb, descriptions                          { terminalb, startb,                          eventSeti } }
  ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_DECIMAL_DIGIT_MANY,            0,       0,        0,         0,          0, "<decimal digit +>",                  {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * meta identifier = letter, {meta identifier character}
   *
   * is revisited to:
   *
   * <meta identifier> = <letter> <meta identifier character any>
   * <meta identifier character any> = <meta identifier character>*
   */
  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                   markerb, markedi, matchedb, eventSeti, exceptionb, descriptions                          { terminalb, startb,                          eventSeti } }
  ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_META_IDENTIFIER_CHARACTER_ANY, 0,       0,        0,         0,          0, "<meta identifier character *>",      {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * special sequence = special sequence symbol, {special sequence character}, special sequence symbol
   *
   * is revisited to:
   *
   * <special sequence character any> = <special sequence character>*
   * <special sequence> = <special sequence symbol> <special sequence character any> <special sequence symbol>
   */
  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                   markerb, markedi, matchedb, eventSeti, exceptionb, descriptions                          { terminalb, startb,                          eventSeti } }
  ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_SPECIAL_SEQUENCE_CHARACTER_ANY,0,       0,        0,         0,          0, "<special sequence character *>",     {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * special sequence character = terminal character - special sequence symbol
   *
   * is revisited to:
   *
   * special sequence character = special sequence character alt 1 _SPECIAL_SEQUENCE_CHARACTER_ALT_1_MARKER
   * special sequence character = special sequence character alt 2 _SPECIAL_SEQUENCE_CHARACTER_ALT_2_MARKER
   * special sequence character alt 1 = terminal character
   * special sequence character alt 2 = special sequence symbol
   */
  /* -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                             markerb,                           markedi, matchedb, eventSeti, exceptionb, descriptions                                 { terminalb, startb,                          eventSeti } }
  -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_SPECIAL_SEQUENCE_CHARACTER_ALT_1,        0,                                 0,        0,         0,          0, "<special sequence character alt 1>",        {         0,      0, -1 /* All Events */                } },
  {_SPECIAL_SEQUENCE_CHARACTER_ALT_1_MARKER, 1, _SPECIAL_SEQUENCE_CHARACTER_ALT_1,        0,         0,          0, "<special sequence character alt 1 marker>", {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SPECIAL_SEQUENCE_CHARACTER_ALT_2,        0,                                 0,        0,         0,          0, "<special sequence character alt 2>",        {         0,      0, -1 /* All Events */                } },
  {_SPECIAL_SEQUENCE_CHARACTER_ALT_2_MARKER, 1, _SPECIAL_SEQUENCE_CHARACTER_ALT_2,        0,         0,          1, "<special sequence character alt 2 marker>", {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * bracketed textual comment = start comment symbol, {comment symbol}, end comment symbol
   *
   * is revisited to:
   *
   * <comment symbol any> = <comment symbol>*
   * <bracketed textual comment> = <start comment symbol> <comment symbol any> <end comment symbol>
   */
  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                   markerb, markedi, matchedb, eventSeti, exceptionb, descriptions                          { terminalb, startb,                          eventSeti } }
  ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_COMMENT_SYMBOL_ANY,            0,       0,        0,         0,          0, "<comment symbol *>",                 {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * syntax = {bracketed textual comment}, commentless symbol, {bracketed textual comment}, {commentless symbol, {bracketed textual comment}}
   *
   * is revisited to:
   *
   * <bracketed textual comment any> = <bracketed textual comment>*
   * <syntax unit> =  <commentless symbol> <bracketed textual comment any>
   * <syntax unit many> =  <syntax unit>+
   * <syntax 01> = <bracketed textual comment any> <syntax unit many>
   * <syntax> = <syntax 01>
   */
  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                   markerb, markedi, matchedb, eventSeti, exceptionb, descriptions                          { terminalb, startb,                          eventSeti } }
  ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_BRACKETED_TEXTUAL_COMMENT_ANY, 0,       0,        0,         0,          0, "<bracketed textual comment *>",      {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SYNTAX_UNIT,                   0,       0,        0,         0,          0, "<syntax unit>",                      {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SYNTAX_UNIT_MANY,              0,       0,        0,         0,          0, "<syntax unit +>",                    {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SYNTAX_01,                     0,       0,        0,         0,          0, "<syntax 01>",                        {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * syntax = syntax rule, {syntax rule}
   *
   * is revisited to:
   *
   * <syntax rule many>> = <syntax rule>+
   * <syntax 02> = <syntax rule many>
   * <syntax> = <syntax 01>
   */
  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                   markerb, markedi, matchedb, eventSeti, exceptionb, descriptions                          { terminalb, startb,                          eventSeti } }
  ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_SYNTAX_RULE_MANY,              0,       0,        0,         0,          0, "<syntax rule +>",                    {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SYNTAX_02,                     0,       0,        0,         0,          0, "<syntax 02>",                        {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
    Marpa does not like counted nullables at all. EMPTY_SEQUENCE is a (and the only) nullable and its parents are:

    definitions list = single definition, {definition separator symbol, single definition}
    single definition = syntactic term, {concatenate symbol, syntactic term}
    syntactic term = syntactic factor [except symbol, syntactic exception]
    syntactic factor = [integer, repetition symbol], syntactic primary
    syntactic primary = empty sequence

    This is revisited to:

    definitions list = _DEFINITIONS_SEQUENCE
    definitions list = empty sequence
    _DEFINITIONS_SEQUENCE = single definition, {definition separator symbol, single definition}
    single definition = syntactic term, {concatenate symbol, syntactic term}
    syntactic term = syntactic factor [except symbol, syntactic exception]
    syntactic factor = [integer, repetition symbol], syntactic primary
  */
  /* ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                   markerb, markedi, matchedb, eventSeti, exceptionb, descriptions                          { terminalb, startb,                          eventSeti } }
  ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_DEFINITIONS_SEQUENCE,          0,       0,        0,         0,          0, "<definitions sequence>",             {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
};


static marpaEBNFRule_t marpaEBNFRuleArray[] = {
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { LETTER } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { DECIMAL_DIGIT } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { CONCATENATE_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { DEFINING_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { DEFINITION_SEPARATOR_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { END_COMMENT_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { END_GROUP_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { END_OPTION_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { END_REPEAT_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { EXCEPT_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { FIRST_QUOTE_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { REPETITION_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { SECOND_QUOTE_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { SPECIAL_SEQUENCE_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { START_COMMENT_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { START_GROUP_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { START_OPTION_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { START_REPEAT_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { TERMINATOR_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { OTHER_CHARACTER } },

  { { 0, 0, 0,            -1, 0, 0 }, GAP_FREE_SYMBOL,                  1, { _GAP_FREE_SYMBOL_ALT_1 } },
  { { 0, 0, 0,            -1, 0, 0 }, GAP_FREE_SYMBOL,                  1, { TERMINAL_STRING } },
  { { 0, 0, 0,            -1, 0, 0 }, _GAP_FREE_SYMBOL_ALT_1,           2, { _GAP_FREE_SYMBOL_ALT_1_1, _GAP_FREE_SYMBOL_ALT_1_1_MARKER } },
  { { 0, 0, 0,            -1, 0, 0 }, _GAP_FREE_SYMBOL_ALT_1,           2, { _GAP_FREE_SYMBOL_ALT_1_2, _GAP_FREE_SYMBOL_ALT_1_2_MARKER } },
  { { 0, 0, 0,            -1, 0, 0 }, _GAP_FREE_SYMBOL_ALT_1_1,         1, { TERMINAL_CHARACTER } },
  { { 0, 0, 0,            -1, 0, 0 }, _GAP_FREE_SYMBOL_ALT_1_2,         1, { FIRST_QUOTE_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, _GAP_FREE_SYMBOL_ALT_1_2,         1, { SECOND_QUOTE_SYMBOL } },

  { { 0, 0, 0,         -1, 0, 0 }, _TERMINAL_STRING_NULLED,          0, { -1 } }, /* Some compilers like cl does not like an empty [] */
  { { 0, 0, 0,         -1, 0, 0 }, TERMINAL_STRING,                  4, { FIRST_QUOTE_SYMBOL, _TERMINAL_STRING_NULLED, _FIRST_TERMINAL_CHARACTER_MANY, FIRST_QUOTE_SYMBOL } },
  { { 0, 0, 0,         -1, 0, 0 }, TERMINAL_STRING,                  4, { SECOND_QUOTE_SYMBOL, _TERMINAL_STRING_NULLED, _SECOND_TERMINAL_CHARACTER_MANY, SECOND_QUOTE_SYMBOL } },
  { { 0, 0, 1,         -1, 0, 1 }, _FIRST_TERMINAL_CHARACTER_MANY,   1, { FIRST_TERMINAL_CHARACTER } },
  { { 0, 0, 1,         -1, 0, 1 }, _SECOND_TERMINAL_CHARACTER_MANY,  1, { SECOND_TERMINAL_CHARACTER } },

  { { 0, 0, 0,         -1, 0, 0 }, FIRST_TERMINAL_CHARACTER,         2, { _FIRST_TERMINAL_CHARACTER_ALT_1, _FIRST_TERMINAL_CHARACTER_ALT_1_MARKER } },
  { { 0, 0, 0,         -1, 0, 0 }, FIRST_TERMINAL_CHARACTER,         2, { _FIRST_TERMINAL_CHARACTER_ALT_2, _FIRST_TERMINAL_CHARACTER_ALT_2_MARKER } },
  { { 0, 0, 0,         -1, 0, 0 }, _FIRST_TERMINAL_CHARACTER_ALT_1,  1, { TERMINAL_CHARACTER } },
  { { 0, 0, 0,         -1, 0, 0 }, _FIRST_TERMINAL_CHARACTER_ALT_2,  1, { FIRST_QUOTE_SYMBOL } },

  { { 0, 0, 0,        -1, 0, 0 }, SECOND_TERMINAL_CHARACTER,         2, { _SECOND_TERMINAL_CHARACTER_ALT_1, _SECOND_TERMINAL_CHARACTER_ALT_1_MARKER } },
  { { 0, 0, 0,        -1, 0, 0 }, SECOND_TERMINAL_CHARACTER,         2, { _SECOND_TERMINAL_CHARACTER_ALT_2, _SECOND_TERMINAL_CHARACTER_ALT_2_MARKER } },
  { { 0, 0, 0,        -1, 0, 0 }, _SECOND_TERMINAL_CHARACTER_ALT_1,  1, { TERMINAL_CHARACTER } },
  { { 0, 0, 0,        -1, 0, 0 }, _SECOND_TERMINAL_CHARACTER_ALT_2,  1, { SECOND_QUOTE_SYMBOL } },

  { { 0, 0, 0,            -1, 0, 0 }, GAP_SEPARATOR,                    1, { SPACE_CHARACTER } },
  { { 0, 0, 0,            -1, 0, 0 }, GAP_SEPARATOR,                    1, { HORIZONTAL_TABULATION_CHARACTER } },
  { { 0, 0, 0,            -1, 0, 0 }, GAP_SEPARATOR,                    1, { NEW_LINE } },
  { { 0, 0, 0,            -1, 0, 0 }, GAP_SEPARATOR,                    1, { VERTICAL_TABULATION_CHARACTER } },
  { { 0, 0, 0,            -1, 0, 0 }, GAP_SEPARATOR,                    1, { FORM_FEED } },

  { { 0, 0, 1,            -1, 0, 0 }, _GAP_SEPARATOR_ANY,               1, { GAP_SEPARATOR } },
  { { 0, 0, 1, GAP_SEPARATOR, 0, 1 }, _GAP_FREE_SYMBOL_MANY,            1, { GAP_SEPARATOR } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTAX,                           2, { _GAP_SEPARATOR_ANY, _GAP_FREE_SYMBOL_MANY } },

  { { 0, 0, 0,            -1, 0, 0 }, COMMENTLESS_SYMBOL,              1, { _COMMENTLESS_SYMBOL_ALT_1 } },
  { { 0, 0, 0,            -1, 0, 0 }, COMMENTLESS_SYMBOL,              1, { META_IDENTIFIER } },
  { { 0, 0, 0,            -1, 0, 0 }, COMMENTLESS_SYMBOL,              1, { INTEGER } },
  { { 0, 0, 0,            -1, 0, 0 }, COMMENTLESS_SYMBOL,              1, { TERMINAL_STRING } },
  { { 0, 0, 0,            -1, 0, 0 }, COMMENTLESS_SYMBOL,              1, { SPECIAL_SEQUENCE } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1,       2, { _COMMENTLESS_SYMBOL_ALT_1_1, _COMMENTLESS_SYMBOL_ALT_1_1_MARKER } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1,       2, { _COMMENTLESS_SYMBOL_ALT_1_2, _COMMENTLESS_SYMBOL_ALT_1_2_MARKER } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_1,     1, { TERMINAL_CHARACTER } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,     1, { LETTER } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,     1, { DECIMAL_DIGIT } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,     1, { FIRST_QUOTE_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,     1, { SECOND_QUOTE_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,     1, { START_COMMENT_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,     1, { END_COMMENT_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,     1, { SPECIAL_SEQUENCE_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,     1, { OTHER_CHARACTER } },

  { { 0, 0, 1,            -1, 0, 1 }, _DECIMAL_DIGIT_MANY,              1, { DECIMAL_DIGIT } },
  { { 0, 0, 0,            -1, 0, 0 }, INTEGER,                          1, { _DECIMAL_DIGIT_MANY } },

  { { 0, 0, 1,          -1, 0, 0 }, _META_IDENTIFIER_CHARACTER_ANY,   1, { META_IDENTIFIER_CHARACTER } },
  { { 0, 0, 0,            -1, 0, 0 }, META_IDENTIFIER,                  2, { LETTER, _META_IDENTIFIER_CHARACTER_ANY } },

  { { 0, 0, 0,            -1, 0, 0 }, META_IDENTIFIER_CHARACTER,        1, { LETTER } },
  { { 0, 0, 0,            -1, 0, 0 }, META_IDENTIFIER_CHARACTER,        1, { DECIMAL_DIGIT } },

  { { 0, 0, 1,         -1, 0, 0 }, _SPECIAL_SEQUENCE_CHARACTER_ANY,  1, { SPECIAL_SEQUENCE_CHARACTER } },
  { { 0, 0, 0,         -1, 0, 0 }, _SPECIAL_SEQUENCE_NULLED,          0, { -1 } }, /* Some compilers like cl does not like an empty [] */
  { { 0, 0, 0,            -1, 0, 0 }, SPECIAL_SEQUENCE,                 4, { SPECIAL_SEQUENCE_SYMBOL, _SPECIAL_SEQUENCE_NULLED, _SPECIAL_SEQUENCE_CHARACTER_ANY, SPECIAL_SEQUENCE_SYMBOL } },

  { { 0, 0, 0,       -1, 0, 0 }, SPECIAL_SEQUENCE_CHARACTER,         2, { _SPECIAL_SEQUENCE_CHARACTER_ALT_1, _SPECIAL_SEQUENCE_CHARACTER_ALT_1_MARKER } },
  { { 0, 0, 0,       -1, 0, 0 }, SPECIAL_SEQUENCE_CHARACTER,         2, { _SPECIAL_SEQUENCE_CHARACTER_ALT_2, _SPECIAL_SEQUENCE_CHARACTER_ALT_2_MARKER } },
  { { 0, 0, 0,       -1, 0, 0 }, _SPECIAL_SEQUENCE_CHARACTER_ALT_1,  1, { TERMINAL_CHARACTER } },
  { { 0, 0, 0,       -1, 0, 0 }, _SPECIAL_SEQUENCE_CHARACTER_ALT_2,  1, { SPECIAL_SEQUENCE_SYMBOL } },

  { { 0, 0, 0,            -1, 0, 0 }, COMMENT_SYMBOL,                   1, { BRACKETED_TEXTUAL_COMMENT } },
  { { 0, 0, 0,            -1, 0, 0 }, COMMENT_SYMBOL,                   1, { OTHER_CHARACTER } },
  { { 0, 0, 0,            -1, 0, 0 }, COMMENT_SYMBOL,                   1, { COMMENTLESS_SYMBOL } },

  { { 0, 0, 1,            -1, 0, 0 }, _COMMENT_SYMBOL_ANY,              1, { COMMENT_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, BRACKETED_TEXTUAL_COMMENT,        3, { START_COMMENT_SYMBOL, _COMMENT_SYMBOL_ANY, END_COMMENT_SYMBOL } },

  { { 0, 0, 1,          -1, 0, 0 }, _BRACKETED_TEXTUAL_COMMENT_ANY,   1, { BRACKETED_TEXTUAL_COMMENT } },
  { { 0, 0, 0,            -1, 0, 0 }, _SYNTAX_UNIT,                     2, { COMMENTLESS_SYMBOL, _BRACKETED_TEXTUAL_COMMENT_ANY } },
  { { 0, 0, 1,            -1, 0, 1 }, _SYNTAX_UNIT_MANY,                1, { _SYNTAX_UNIT } },
  { { 0, 0, 0,            -1, 0, 0 }, _SYNTAX_01,                       2, { _BRACKETED_TEXTUAL_COMMENT_ANY, _SYNTAX_UNIT_MANY } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTAX,                           1, { _SYNTAX_01 } },

  { { 0, 0, 1,            -1, 0, 1 }, _SYNTAX_RULE_MANY,                1, { SYNTAX_RULE } },
  { { 0, 0, 0,            -1, 0, 0 }, _SYNTAX_02,                       1, { _SYNTAX_RULE_MANY } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTAX,                           1, { _SYNTAX_02 } },

  { { 0, 0, 0,            -1, 0, 0 }, SYNTAX_RULE,                      4, { META_IDENTIFIER, DEFINING_SYMBOL, DEFINITIONS_LIST, TERMINATOR_SYMBOL } },

  { { 0, 0, 1, DEFINITION_SEPARATOR_SYMBOL, 0, 1 }, _DEFINITIONS_SEQUENCE, 1, { SINGLE_DEFINITION } },
  { { 0, 0, 0,            -1, 0, 0 }, DEFINITIONS_LIST,                 1, { _DEFINITIONS_SEQUENCE } },
  { { 0, 0, 0,            -1, 0, 0 }, DEFINITIONS_LIST,                 1, { EMPTY_SEQUENCE } },
  { { 0, 0, 1, CONCATENATE_SYMBOL,          0, 1 }, SINGLE_DEFINITION,            1, { SYNTACTIC_TERM } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_TERM,                   1, { SYNTACTIC_FACTOR } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_TERM,                   3, { SYNTACTIC_FACTOR, EXCEPT_SYMBOL, SYNTACTIC_EXCEPTION } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_FACTOR,                 3, { INTEGER, REPETITION_SYMBOL, SYNTACTIC_PRIMARY } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_FACTOR,                 1, { SYNTACTIC_PRIMARY } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { OPTIONAL_SEQUENCE } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { REPEATED_SEQUENCE } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { GROUPED_SEQUENCE } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { META_IDENTIFIER } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { TERMINAL_STRING } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { SPECIAL_SEQUENCE } },
  /*
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { EMPTY_SEQUENCE } },
  */
  { { 0, 0, 0,            -1, 0, 0 }, OPTIONAL_SEQUENCE,                3, { START_OPTION_SYMBOL, DEFINITIONS_LIST, END_OPTION_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, REPEATED_SEQUENCE,                3, { START_REPEAT_SYMBOL, DEFINITIONS_LIST, END_REPEAT_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, GROUPED_SEQUENCE,                 3, { START_GROUP_SYMBOL, DEFINITIONS_LIST, END_GROUP_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, EMPTY_SEQUENCE,                   0, { -1 } }, /* Some compilers like cl does not like an empty [] */
};

/* Internally, EBNF is nothing else but an instance of marpaWrapperGrammar_t along */
/* with symbols and rules book-keeping.                                            */
struct marpaEBNF {
  marpaEBNFOption_t      marpaEBNFOption;      /* Option */
  marpaWrapperGrammar_t *grammarp;             /* External grammar */
  marpaWrapperGrammar_t *marpaWrapperGrammarp; /* Internal grammar */
  marpaEBNFSymbol_t     *symbolArrayp;         /* Copy of marpaEBNFSymbolArray: it has lexer dependent volatile data */
  marpaEBNFRule_t       *ruleArrayp;           /* Copy of marpaEBNFRuleArray: it has lexer dependent volatile data */
  int                    terminalStringContexti;    /* EBNF is not ambiguous if we take care of some symbols */
  int                    specialSequenceContexti;   /* that are not allowed.                                 */
};

static inline short _marpaEBNF_internalGrammarb(marpaEBNF_t *marpaEBNFp);
static inline char *_marpaEBNF_symbolDescription(void *userDatavp, int symboli);

static marpaEBNFOption_t marpaEBNFOptionDefault = {
  NULL /* genericLoggerp */
};

typedef struct marpaEBNFAlternative {
  int symboli;
  int lengthl;
} marpaEBNFAlternative_t;

static inline int _marpaEBNFAlternativeCmpi(const void *p1, const void *p2);

/****************************************************************************/
marpaEBNF_t *marpaEBNF_newp(marpaEBNFOption_t *marpaEBNFOptionp)
/****************************************************************************/
{
  marpaEBNF_t                *marpaEBNFp;
  marpaWrapperGrammarOption_t marpaWrapperGrammarOption;
  size_t                      marpaEBNFSymbolArraySizel = MARPAEBNF_LENGTH_ARRAY(marpaEBNFSymbolArray) * sizeof(marpaEBNFSymbol_t);
  size_t                      marpaEBNFRuleArraySizel = MARPAEBNF_LENGTH_ARRAY(marpaEBNFRuleArray) * sizeof(marpaEBNFRule_t);

  if (marpaEBNFOptionp == NULL) {
    marpaEBNFOptionp = &marpaEBNFOptionDefault;
  }

  marpaEBNFp = malloc(sizeof(marpaEBNF_t));
  if (marpaEBNFp == NULL) {
    goto err;
  }

  /* Initialization */
  marpaEBNFp->marpaEBNFOption      = *marpaEBNFOptionp;
  marpaEBNFp->grammarp             = NULL;
  marpaEBNFp->symbolArrayp         = (marpaEBNFSymbol_t *) malloc(marpaEBNFSymbolArraySizel);
  marpaEBNFp->ruleArrayp           = (marpaEBNFRule_t *) malloc(marpaEBNFRuleArraySizel);

  marpaWrapperGrammarOption.genericLoggerp    = marpaEBNFp->marpaEBNFOption.genericLoggerp;
  marpaWrapperGrammarOption.warningIsErrorb   = 0;
  marpaWrapperGrammarOption.warningIsIgnoredb = 0;

  marpaEBNFp->marpaWrapperGrammarp = marpaWrapperGrammar_newp(&marpaWrapperGrammarOption);

  if ((marpaEBNFp->symbolArrayp         == NULL) ||
      (marpaEBNFp->ruleArrayp           == NULL) ||
      (marpaEBNFp->marpaWrapperGrammarp == NULL)) {
    goto err;
  }

  /* Take care: marpaEBNFp->symbolArrayp and marpaEBNFp->ruleArrayp will contain pointers    */
  /* to strings that are shared with the static marpaEBNFSymbolArray and marpaEBNFRuleArray, */
  /* respectively.                                                                           */
  memcpy(marpaEBNFp->symbolArrayp, marpaEBNFSymbolArray, marpaEBNFSymbolArraySizel);
  memcpy(marpaEBNFp->ruleArrayp, marpaEBNFRuleArray, marpaEBNFRuleArraySizel);

  /* Compute internal grammar */
  if (_marpaEBNF_internalGrammarb(marpaEBNFp) == 0) {
    goto err;
  }

  return marpaEBNFp;

 err:
  marpaEBNF_freev(marpaEBNFp);
  return NULL;
}

/****************************************************************************/
short marpaEBNF_grammarb(marpaEBNF_t *marpaEBNFp, char *grammars)
/****************************************************************************/
{
  int                            linel                   = 1;
  int                            columnl                 = 1;
  marpaWrapperRecognizer_t      *marpaWrapperRecognizerp = NULL;
  marpaEBNFAlternative_t        *alternativep            = NULL;
  marpaWrapperValue_t           *marpaWrapperValuep      = NULL;

  marpaWrapperRecognizerOption_t marpaWrapperRecognizerOption;
  size_t                         eventl;
  marpaWrapperGrammarEvent_t    *eventp;
  size_t                         i;
  size_t                         nSymboll;
  int                           *symbolArrayp;
  int                            symboli, markedSymboli;
  marpaEBNFSymbol_t             *marpaEBNFSymbolp;
  char                           c;
  char                          *p, *q;
  char                          *maxp;
  size_t                         grammarLengthl;
  short                          okb;
  unsigned long                  lengthl, tmplengthl;
  marpaWrapperGrammarEventType_t eventTypee;
  char                          *tokens;
  size_t                         alternativel = 0;
  size_t                         alternativeokl = 0;
  marpaWrapperValueOption_t      marpaWrapperValueOption;
  short                          doCompleteb = 0;
  short                          rci;
  short                          discardb;
  GENERICSTACK_DECL(inputStack);

  if ((marpaEBNFp == NULL) || (grammars == NULL)) {
    errno = EINVAL;
    goto err;
  }

  /* An ISO EBNF grammar is always expressed in the 7-bit ASCII range; that is entirely */
  /* covered by the basic C default character set. We just have to take care to always  */
  /* compare with a char, not an integer value.                                         */
  grammarLengthl = strlen(grammars);

  /* We know that our input stack will never exceed the number of characters in grammars */
  GENERICSTACK_NEW(inputStack);
  if (GENERICSTACK_ERROR(inputStack)) {
    goto err;
  }

  /* Parse external grammar using internal grammar, supposed to fit entirely in memory */
  /* We manage ourself the LATM mode in the sense that all terminals have a length 1:  */
  /* In the grammar terminology, length is a number of earlemes, not length in the     */
  /* input stream.                                                                     */
  marpaWrapperRecognizerOption.genericLoggerp = marpaEBNFp->marpaEBNFOption.genericLoggerp;
  marpaWrapperRecognizerOption.latmb          = 0;

  marpaWrapperRecognizerp = marpaWrapperRecognizer_newp(marpaEBNFp->marpaWrapperGrammarp, &marpaWrapperRecognizerOption);
  if (marpaWrapperRecognizerp == NULL) {
    goto err;
  }

  /* Keep track of context */
  marpaEBNFp->terminalStringContexti = 0;
  marpaEBNFp->specialSequenceContexti = 0;
  
  /* Loop until it is consumed or an error */
  p = grammars;
  maxp = p + grammarLengthl;
  while (p <= maxp) {

    if ((marpaEBNFp->terminalStringContexti == 0) && (marpaEBNFp->specialSequenceContexti == 0) && (p < maxp - 2)) {
      if ((strncmp(p, "(*)", 3) == 0) || (strncmp(p, "(:)", 3) == 0) || (strncmp(p, "(/)", 3) == 0)) {
	if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
	  GENERICLOGGER_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Invalid sequence of characters outside a terminal string or a special sequence context: %c%c%c", p[0], p[1], p[2]);
	  goto err;
	}
      }
    }

    /* When p is == maxp, we only take care of the events */
    
    /* Lookup expected terminals */
    if (marpaWrapperRecognizer_expectedb(marpaWrapperRecognizerp, &nSymboll, &symbolArrayp) == 0) {
      goto err;
    }
    /* Make sure we have enough space in the internal alternativep array */
    if (alternativep == NULL) {
      alternativep = malloc(nSymboll * sizeof(marpaEBNFAlternative_t));
      if (alternativep == NULL) {
	goto err;
      }
      alternativel = nSymboll;
    } else if (alternativel < nSymboll) {
      marpaEBNFAlternative_t *tmp = (marpaEBNFAlternative_t *) realloc(alternativep, nSymboll * sizeof(marpaEBNFAlternative_t));
      if (tmp == NULL) {
	goto err;
      }
      alternativep = tmp;
      alternativel = nSymboll;
    }

    /* Reset matchedb for all expected terminals and eventSeti for all surveyed symbols */
    if (nSymboll > 0) {
      for (i = 0; i < nSymboll; i++) {
	symboli = symbolArrayp[i];
	marpaEBNFp->symbolArrayp[symboli].matchedb  = 0;
	/* If this terminal is an internal marker, reset */
	/* also the event data for the marked symbol.    */
	if (MARPAEBNF_SYMBOL_IS_MARKER(&(marpaEBNFp->symbolArrayp[symboli]))) {
	  symboli = marpaEBNFp->symbolArrayp[symboli].markedi;
	  marpaEBNFp->symbolArrayp[symboli].eventSeti = 0;
	}
      }
    }
    
    /* Get events */
    if (marpaWrapperGrammar_eventb(marpaEBNFp->marpaWrapperGrammarp, &eventl, &eventp, 0) == 0) {
      goto err;
    }

    /* Remember which symbols have an event */
    if (eventl > 0) {
      for (i = 0; i < eventl; i++) {

	switch (eventp[i].eventType) {
	case MARPAWRAPPERGRAMMAR_EVENT_COMPLETED:
	  eventTypee = MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION;
	  if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
	    GENERICLOGGER_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Completion event on symbol: %s", _marpaEBNF_symbolDescription(marpaEBNFp, eventp[i].symboli));
	  }
	  switch (eventp[i].symboli) {
	  case TERMINAL_STRING:
	    if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
	      GENERICLOGGER_TRACE(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Terminal string context is ending");
	    }
	    marpaEBNFp->terminalStringContexti--;
	    break;
	  case SPECIAL_SEQUENCE:
	    if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
	      GENERICLOGGER_TRACE(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Special sequence context is ending");
	    }
	    marpaEBNFp->specialSequenceContexti--;
	    break;
	  default:
	    break;
	  }
	  break;
	case MARPAWRAPPERGRAMMAR_EVENT_NULLED:
	  eventTypee = MARPAWRAPPERGRAMMAR_EVENTTYPE_NULLED;
	  if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
	    GENERICLOGGER_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Nulled event on symbol: %s", _marpaEBNF_symbolDescription(marpaEBNFp, eventp[i].symboli));
	  }
	  switch (eventp[i].symboli) {
	  case _TERMINAL_STRING_NULLED:
	    if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
	      GENERICLOGGER_TRACE(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Terminal string context is starting");
	    }
	    marpaEBNFp->terminalStringContexti++;
	    break;
	  case _SPECIAL_SEQUENCE_NULLED:
	    if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
	      GENERICLOGGER_TRACE(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Special sequence context is starting");
	    }
	    marpaEBNFp->specialSequenceContexti++;
	    break;
	  default:
	    break;
	  }
	  break;
	case MARPAWRAPPERGRAMMAR_EVENT_EXPECTED:
	  eventTypee = MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION;
	  if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
	    GENERICLOGGER_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Predicted event on symbol: %s", _marpaEBNF_symbolDescription(marpaEBNFp, eventp[i].symboli));
	  }
	  break;
	default:
	  eventTypee = MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE;
	  /* Should never happen and is anyway a no-op */
	  break;
	}

	if (eventTypee != MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE) {
	  symboli = eventp[i].symboli;
	  /* We do not mind if we are using a variable not resetted upper: */
	  /* such internal variable will not be used anyway.               */
	  marpaEBNFp->symbolArrayp[symboli].eventSeti |= eventTypee;
	}
      }
    }

    alternativeokl = 0;
    if (nSymboll > 0) {

      /* --------------------------------------------------------------------------- */
      /* First look ONLY to the real expected terminals (markers are fake terminals) */
      /* Real terminals can only occur before the end of the stream.                 */
      /* --------------------------------------------------------------------------- */
      if (p < maxp) {
	for (i = 0; i < nSymboll; i++) {
	  symboli = symbolArrayp[i];
	  marpaEBNFSymbolp = &(marpaEBNFp->symbolArrayp[symboli]);
	  if (MARPAEBNF_SYMBOL_IS_MARKER(marpaEBNFSymbolp)) {
	    continue;
	  }

	  if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
	    GENERICLOGGER_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Expected terminal: %s", _marpaEBNF_symbolDescription(marpaEBNFp, symboli));
	  }
	  okb = 0;
	  c = *p;
	  q = p + 1;
	  /* Process only terminals that we know about */
	  switch (symboli) {
	  case LETTER:
	    if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) { okb = 1; }
	    break;
	  case DECIMAL_DIGIT:
	    if ((c >= '0') && (c <= '9')) { okb = 1; }
	    break;
	  case CONCATENATE_SYMBOL:
	    if (c == ',') { okb = 1; }
	    break;
	  case DEFINING_SYMBOL:
	    if (c == '=') { okb = 1; }
	    break;
	  case DEFINITION_SEPARATOR_SYMBOL:
	    if ((c == '|') || (c == '/') || (c == '!')) { okb = 1; }
	    break;
	  case END_COMMENT_SYMBOL:
	    if ((c == '*') && (q < maxp) && (*q++ == ')')) { okb = 1; }
	    break;
	  case END_GROUP_SYMBOL:
	    if (c == ')') { okb = 1; }
	    break;
	  case END_OPTION_SYMBOL:
	    if (c == ']')                                       { okb = 1; }
	    else if ((c == '/') && (q < maxp) && (*q++ == ')')) { okb = 1; }
	    break;
	  case END_REPEAT_SYMBOL:
	    if (c == '}')                                       { okb = 1; }
	    else if ((c == ':') && (q < maxp) && (*q++ == ')')) { okb = 1; }
	    break;
	  case EXCEPT_SYMBOL:
	    if (c == '-') { okb = 1; }
	    break;
	  case FIRST_QUOTE_SYMBOL:
	    if (c == '\'') { okb = 1; }
	    break;
	  case REPETITION_SYMBOL:
	    if (c == '*') { okb = 1; }
	    break;
	  case SECOND_QUOTE_SYMBOL:
	    if (c == '"') { okb = 1; }
	    break;
	  case SPECIAL_SEQUENCE_SYMBOL:
	    if (c == '?') { okb = 1; }
	    break;
	  case START_COMMENT_SYMBOL:
	    if ((c == '(') && (q < maxp) && (*q++ == '*')) { okb = 1; }
	    break;
	  case START_GROUP_SYMBOL:
	    if (c == '(') { okb = 1; }
	    break;
	  case START_OPTION_SYMBOL:
	    if (c == '[')                                       { okb = 1; }
	    else if ((c == '(') && (q < maxp) && (*q++ == '/')) { okb = 1; }
	    break;
	  case START_REPEAT_SYMBOL:
	    if (c == '{')                                       { okb = 1; }
	    else if ((c == '(') && (q < maxp) && (*q++ == ':')) { okb = 1; }
	    break;
	  case TERMINATOR_SYMBOL:
	    if (c == ';' || c == '.') { okb = 1; }
	    break;
	  case OTHER_CHARACTER:
	    if ((c == ' ') || (c == ':') || (c == '+') || (c == '_') || (c == '%') || (c == '@')  ||
		(c == '&') || (c == '#') || (c == '$') || (c == '<') || (c == '>') || (c == '\\') ||
		(c == '^') || (c == '`') || (c == '~')
		) { okb = 1; }
	    break;
	  case SPACE_CHARACTER:
	    if (c == ' ') { okb = 1; }
	    break;
	  case HORIZONTAL_TABULATION_CHARACTER:
	    if (c == '\t') { okb = 1; }
	    break;
	  case NEW_LINE:
	    {
	      tmplengthl = 0;
	      q = p;
	      while ((q < maxp) && (*q++ == '\r')) { ++tmplengthl; }
	      if    ((q < maxp) && (*q++ == '\n')) { ++linel; columnl = 1; okb = 1; ++tmplengthl; }
	      while ((q < maxp) && (*q++ == '\r')) { ++tmplengthl; }
	    }
	    break;
	  case VERTICAL_TABULATION_CHARACTER:
	    if (c == '\v') { okb = 1; }
	    break;
	  case FORM_FEED:
	    if (c == '\f') { okb = 1; }
	    break;
	  default:
	    break;
	  }
	  if (okb) {
	    /* Reject some character pairs terminals depending on context */
	    lengthl = (int) (q - p);
	    if (lengthl > 1) {
	      if ((marpaEBNFp->terminalStringContexti > 0) || (marpaEBNFp->specialSequenceContexti > 0)) {
		switch (symboli) {
		case START_COMMENT_SYMBOL:
		case END_COMMENT_SYMBOL:
		case START_REPEAT_SYMBOL:
		case END_REPEAT_SYMBOL:
		case START_OPTION_SYMBOL:
		case END_OPTION_SYMBOL:
		  if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
		    GENERICLOGGER_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Rejected %s (terminal string or special sequence context)", _marpaEBNF_symbolDescription(marpaEBNFp, symboli));
		    GENERICLOGGER_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Grammar was at: %s", p);
		  }
		  okb = 0;
		  break;
		default:
		  break;
		}
	      }
	    }
	  }
	  if (okb) {
	    alternativep[alternativeokl].symboli = symboli;
	    alternativep[alternativeokl].lengthl = (int) (q - p);
	    alternativeokl++;
	  }
	}
      }
    }

    lengthl = 0;
    doCompleteb = 0;
    discardb = 0;

    if (alternativeokl > 0) {
      /* Say we will have to call for lexeme completion */
      doCompleteb = 1;
      /* Sort by longest length */
      qsort(alternativep, alternativeokl, sizeof(marpaEBNFAlternative_t), _marpaEBNFAlternativeCmpi);
      /* Keep only the longests */
      lengthl = alternativep[0].lengthl;
      for (i = 0; i < alternativeokl; i++) {
	/* If we managed correctly the context, we should never have more than one terminal */
	if (alternativep[i].lengthl < lengthl) {
	  break;
	}
	if (i > 0) {
	  if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
	    GENERICLOGGER_ERROR(marpaEBNFp->marpaEBNFOption.genericLoggerp, "More than one terminal detected");
	    goto err;
	  }
	}
	symboli = alternativep[i].symboli;
	/* Remember we matched it */
	marpaEBNFSymbolp = &(marpaEBNFp->symbolArrayp[symboli]);
	marpaEBNFSymbolp->matchedb = 1;
	/* Our input stack is a copy of what was matched */
	tokens = malloc(lengthl + 1);
	if (tokens == NULL) {
	  goto err;
	}
	strncpy(tokens, p, lengthl);
	tokens[lengthl] = '\0';
	GENERICSTACK_PUSH_PTR(inputStack, tokens);
	if (GENERICSTACK_ERROR(inputStack)) {
	  goto err;
	}
	if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
	  GENERICLOGGER_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Accepted terminal %s of length %ld: \"%s\"", _marpaEBNF_symbolDescription(marpaEBNFp, symboli), (unsigned long) lengthl, tokens);
	}
	/* Length is a number of earlemes from grammar point of view - we maintain internally the real pointer position */
	if (marpaWrapperRecognizer_alternativeb(marpaWrapperRecognizerp, symboli, GENERICSTACK_SIZE(inputStack), 1) == 0) {
	  goto err;
	}
      }
    }

    if (doCompleteb == 0) {
      /* ------------------------------------------- */
      /* No true terminal: look discarded characters */
      /* ------------------------------------------- */
      if (p < maxp) {
	switch (*p) {
	case '\n':
	  ++linel;
	  columnl = 1;
	  /* No break intentionnaly */
	case '\t':
	case '\f':
	case '\r':
	case '\v':
	case ' ':
	  if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
	    GENERICLOGGER_INFO(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Discarded a space character");
	  }
	  discardb = 1;
	  lengthl = 1;
	default:
	  break;
	}
      }
    }

    if ((doCompleteb == 0) && (discardb == 0)) {
      /* -------------------------------------------------------------- */
      /* No true terminal and nothing discarded: look to fake terminals */
      /* -------------------------------------------------------------- */
      for (i = 0; i < nSymboll; i++) {
	okb = 0;
        symboli = symbolArrayp[i];
	marpaEBNFSymbolp = &(marpaEBNFp->symbolArrayp[symboli]);
	/* Note that we restrict this part only to markers that are NOT an exception */
	if (! MARPAEBNF_SYMBOL_IS_MARKER_OK(marpaEBNFSymbolp)) {
	  continue;
	}
	if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
	  GENERICLOGGER_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Expected fake terminal: %s", _marpaEBNF_symbolDescription(marpaEBNFp, symboli));
	}

	markedSymboli = marpaEBNFSymbolp->markedi;
	marpaEBNFSymbolp = &(marpaEBNFp->symbolArrayp[markedSymboli]);
	if ((marpaEBNFSymbolp->eventSeti & MARPAWRAPPERGRAMMAR_EVENTTYPE_NULLED) == MARPAWRAPPERGRAMMAR_EVENTTYPE_NULLED) {
	  okb = 1;
	} else if ((marpaEBNFSymbolp->eventSeti & MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION) == MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION) {
	  if ((marpaEBNFSymbolp->eventSeti & MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION) == MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION) {
	    if (marpaEBNFSymbolp->matchedb == 0) {
	      okb = 1;
	    }
	  } else {
	    okb = 1;
	  }
	}
        if (okb) {
	  /* Remember we matched it */
	  marpaEBNFSymbolp = &(marpaEBNFp->symbolArrayp[symboli]);
	  marpaEBNFSymbolp->matchedb = 1;
	  /* Our input stack is the current pointer: a fake terminal has not real value */
          GENERICSTACK_PUSH_PTR(inputStack, NULL);
	  if (GENERICSTACK_ERROR(inputStack)) {
	    goto err;
	  }
	  if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
	    GENERICLOGGER_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Accepted fake terminal: %s", _marpaEBNF_symbolDescription(marpaEBNFp, symboli));
	  }
	  /* Length of a fake terminal is 1 and it does not advance the pointer */
          if (marpaWrapperRecognizer_alternativeb(marpaWrapperRecognizerp, symboli, GENERICSTACK_SIZE(inputStack), 1) == 0) {
            goto err;
          }
	  /* Remember we have to call for lexeme completion */
	  doCompleteb = 1;
        }
      }
    }

    if (doCompleteb != 0) {
      /* This will naturally fail if we pushed no alternative */
      if (marpaWrapperRecognizer_completeb(marpaWrapperRecognizerp) == 0) {
	goto err;
      }
    } else if (discardb == 0) {
      /* This is an error if we are not at the end of the grammar */
      if (p < maxp) {
	if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
	  GENERICLOGGER_ERROR(marpaEBNFp->marpaEBNFOption.genericLoggerp, "No terminal nor discard detected");
	}
	goto err;
      }
    }

    if (p < maxp) {
      /* Resume */   
      if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
	GENERICLOGGER_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Moving forward by %ld", lengthl);
      }
      p += lengthl;
    } else {
      break;
    }
  }

  /* Ask for the value, we REQUIRE it to be unambigous. Sine we have no rank, let's specify it. */
  marpaWrapperValueOption.genericLoggerp = marpaEBNFp->marpaEBNFOption.genericLoggerp;
  marpaWrapperValueOption.highRankOnlyb  = 0;
  marpaWrapperValueOption.orderByRankb   = 0;
  marpaWrapperValueOption.ambiguousb     = 0;
  marpaWrapperValueOption.nullb          = 0;
  marpaWrapperValuep = marpaWrapperValue_newp(marpaWrapperRecognizerp, &marpaWrapperValueOption);

  if (marpaWrapperValue_valueb(marpaWrapperValuep, NULL, NULL, NULL, NULL) == 0) {
    goto err;
  }
  
  rci = 1;
  goto done;

 err:
  rci = 0;

 done:
  if (inputStack != NULL) {
    for (i = 0; i < GENERICSTACK_SIZE(inputStack); i++) {
      tokens = GENERICSTACK_GET_PTR(inputStack, i);
      if (tokens != NULL) {
	free(tokens);
      }
    }
    GENERICSTACK_FREE(inputStack);
  }
  if (alternativep != NULL) {
    free(alternativep);
  }
  if (marpaWrapperValuep != NULL) {
    marpaWrapperValue_freev(marpaWrapperValuep);
  }
  if (marpaWrapperRecognizerp != NULL) {
    if (rci == 0) {
      /* Log current state of the recognizer */
      marpaWrapperRecognizer_progressLogb(marpaWrapperRecognizerp, -1, -1, GENERICLOGGER_LOGLEVEL_ERROR, marpaEBNFp, _marpaEBNF_symbolDescription);
    }
    marpaWrapperRecognizer_freev(marpaWrapperRecognizerp);
  }
  return 0;
}

/****************************************************************************/
void marpaEBNF_freev(marpaEBNF_t *marpaEBNFp)
/****************************************************************************/
{
  if (marpaEBNFp != NULL) {
    if (marpaEBNFp->symbolArrayp != NULL) {
      free(marpaEBNFp->symbolArrayp);
    }
    if (marpaEBNFp->ruleArrayp != NULL) {
      free(marpaEBNFp->ruleArrayp);
    }
    if (marpaEBNFp->marpaWrapperGrammarp != NULL) {
      marpaWrapperGrammar_freev(marpaEBNFp->marpaWrapperGrammarp);
    }
    if (marpaEBNFp->grammarp != NULL) {
      marpaWrapperGrammar_freev(marpaEBNFp->grammarp);
    }
    free(marpaEBNFp);
  }
}

/****************************************************************************/
static inline short _marpaEBNF_internalGrammarb(marpaEBNF_t *marpaEBNFp)
/****************************************************************************/
{
  int i;
  int symboli;
  int rulei;

  /* Declare all the symbols */
  for (i = 0; i < MARPAEBNF_LENGTH_ARRAY(marpaEBNFSymbolArray); i++) {
    symboli = marpaWrapperGrammar_newSymboli(marpaEBNFp->marpaWrapperGrammarp,
					     &(marpaEBNFSymbolArray[i].option));
    /* We take advantage of the fact that symbols always start at 0 with marpa */
    if (symboli != marpaEBNFSymbolArray[i].symboli) {
      goto err;
    }
  }

  /* Declare all the rules */
  for (i = 0; i < MARPAEBNF_LENGTH_ARRAY(marpaEBNFRuleArray); i++) {
    rulei = marpaWrapperGrammar_newRulei(marpaEBNFp->marpaWrapperGrammarp,
					 &(marpaEBNFRuleArray[i].option),
					 marpaEBNFRuleArray[i].lhsSymboli,
					 marpaEBNFRuleArray[i].rhsSymboll,
					 marpaEBNFRuleArray[i].rhsSymbolip);
    if (rulei < 0) {
      goto err;
    }
  }

  /* Precompute grammar */
  if (marpaWrapperGrammar_precomputeb(marpaEBNFp->marpaWrapperGrammarp) == 0) {
    goto err;
  }

  return 1;

 err:
  return 0;
}

/****************************************************************************/
static inline char *_marpaEBNF_symbolDescription(void *userDatavp, int symboli)
/****************************************************************************/
{
  marpaEBNF_t *marpaEBNFp = (marpaEBNF_t *) userDatavp;

  return marpaEBNFSymbolArray[symboli].descriptions;
}

/****************************************************************************/
static inline int _marpaEBNFAlternativeCmpi(const void *p1, const void *p2)
/****************************************************************************/
{
  marpaEBNFAlternative_t *p = (marpaEBNFAlternative_t *) p1;
  marpaEBNFAlternative_t *q = (marpaEBNFAlternative_t *) p2;

  return (p->lengthl < q->lengthl) ? 1 : ((p->lengthl == q->lengthl) ? 0 : -1);
}
