#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "config.h"
#include "genericStack.h"
#include "marpaWrapper.h"
#include "marpaEBNF.h"
#include "marpaEBNF/internal/logging.h"

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
  _GAP_FREE_SYMBOL_ALT_1,
  _GAP_FREE_SYMBOL_ALT_1_1,
  _GAP_FREE_SYMBOL_ALT_1_2,
  _KO_FIRST_QUOTE_SYMBOL,
  _KO_SECOND_QUOTE_SYMBOL,

  _FIRST_TERMINAL_CHARACTER_MANY,
  _SECOND_TERMINAL_CHARACTER_MANY,

  _FIRST_TERMINAL_CHARACTER_ALT_1,
  _FIRST_TERMINAL_CHARACTER_ALT_2,

  _SECOND_TERMINAL_CHARACTER_ALT_1,
  _SECOND_TERMINAL_CHARACTER_ALT_2,

  _GAP_SEPARATOR_ANY,
  _GAP_SYMBOL_UNIT,
  _GAP_SYMBOL_UNIT_MANY,

  _COMMENTLESS_SYMBOL_ALT_1,
  _COMMENTLESS_SYMBOL_ALT_1_1,
  _COMMENTLESS_SYMBOL_ALT_1_2,
  _KO_LETTER,
  _KO_DECIMAL_DIGIT,
  _KO_START_COMMENT_SYMBOL,
  _KO_END_COMMENT_SYMBOL,
  _KO_SPECIAL_SEQUENCE_SYMBOL,
  _KO_OTHER_CHARACTER,

  _META_IDENTIFIER_CHARACTER_ANY,

  _SPECIAL_SEQUENCE_CHARACTER_ANY,

  _SPECIAL_SEQUENCE_CHARACTER_ALT_1,
  _SPECIAL_SEQUENCE_CHARACTER_ALT_2,

  _COMMENT_SYMBOL_ANY,

  _BRACKETED_TEXTUAL_COMMENT_ANY,
  _COMMENTLESS_SYMBOL_UNIT,
  _COMMENTLESS_SYMBOL_UNIT_MANY,

  _DEFINITIONS_SEQUENCE,
} marpaEBNFSymbol_e;

typedef struct marpaEBNFSymbol {
  marpaEBNFSymbol_e                 symboli;
  short                             rejectedb;  /* Initialized to 0 at each lex lookup */
  int                               eventSeti;  /* Initialized to 0 at each lex lookup */
  int                               exceptioni;
  char                             *descriptions;
  marpaWrapperGrammarSymbolOption_t option;
} marpaEBNFSymbol_t;

typedef struct marpaEBNFRule {
  marpaWrapperGrammarRuleOption_t   option;
  int                               lhsSymboli;
  size_t                            rhsSymboll;
  int                               rhsSymbolip[20]; /* Flexible array and ANSI-C are not friends */
} marpaEBNFRule_t;

/* List of all symbols of the EBNF grammar as per ISO/IEC 14977:1996 */
static marpaEBNFSymbol_t marpaEBNFSymbolArray[] = {
  /* -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                  rejectedb, eventSeti, exceptioni, descriptions                         { terminalb, startb,                                eventSeti } }
  -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {LETTER,                          0,         0,         -1, "<letter>",                          {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {DECIMAL_DIGIT,                   0,         0,         -1, "<decimal digit>",                   {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {CONCATENATE_SYMBOL,              0,         0,         -1, "<concatenate symbol>",              {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {DEFINING_SYMBOL,                 0,         0,         -1, "<defining symbol>",                 {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {DEFINITION_SEPARATOR_SYMBOL,     0,         0,         -1, "<definition separator symbol>",     {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {END_COMMENT_SYMBOL,              0,         0,         -1, "<end comment symbol>",              {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {END_GROUP_SYMBOL,                0,         0,         -1, "<end group symbol>",                {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {END_OPTION_SYMBOL,               0,         0,         -1, "<end option symbol>",               {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {END_REPEAT_SYMBOL,               0,         0,         -1, "<end repeat symbol>",               {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {EXCEPT_SYMBOL,                   0,         0,         -1, "<except symbol>",                   {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {FIRST_QUOTE_SYMBOL,              0,         0,         -1, "<first quote symbol>",              {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {REPETITION_SYMBOL,               0,         0,         -1, "<repetition symbol>",               {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SECOND_QUOTE_SYMBOL,             0,         0,         -1, "<second quote symbol>",             {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SPECIAL_SEQUENCE_SYMBOL,         0,         0,         -1, "<special sequence symbol>",         {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {START_COMMENT_SYMBOL,            0,         0,         -1, "<start comment symbol>",            {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {START_GROUP_SYMBOL,              0,         0,         -1, "<start group symbol>",              {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {START_OPTION_SYMBOL,             0,         0,         -1, "<start option symbol>",             {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {START_REPEAT_SYMBOL,             0,         0,         -1, "<start repeat symbol>",             {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {TERMINATOR_SYMBOL,               0,         0,         -1, "<terminator symbol>",               {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {OTHER_CHARACTER,                 0,         0,         -1, "<other character>",                 {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SPACE_CHARACTER,                 0,         0,         -1, "<space character>",                 {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {HORIZONTAL_TABULATION_CHARACTER, 0,         0,         -1, "<horizontal tabulation character>", {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {NEW_LINE,                        0,         0,         -1, "<new line>",                        {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {VERTICAL_TABULATION_CHARACTER,   0,         0,         -1, "<vertical tabulation character>",   {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {FORM_FEED,                       0,         0,         -1, "<form feed>",                       {         1,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },

  /* -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                  rejectedb, eventSeti, exceptioni, descriptions                         { terminalb, startb,                                eventSeti } }
  -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {TERMINAL_CHARACTER,              0,         0,         -1, "<terminal character>",              {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {GAP_FREE_SYMBOL,                 0,         0,         -1, "<gap free symbol>",                 {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {TERMINAL_STRING,                 0,         0,         -1, "<terminal string>",                 {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {FIRST_TERMINAL_CHARACTER,        0,         0,         -1, "<first terminal character>",        {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SECOND_TERMINAL_CHARACTER,       0,         0,         -1, "<second terminal character>",       {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {GAP_SEPARATOR,                   0,         0,         -1, "<gap separator>",                   {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTAX,                          0,         0,         -1, "<syntax>",                          {         0,      1,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },  /* start symbol */
  {COMMENTLESS_SYMBOL,              0,         0,         -1, "<commentless symbol>",              {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {INTEGER,                         0,         0,         -1, "<integer>",                         {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {META_IDENTIFIER,                 0,         0,         -1, "<meta identifier>",                 {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {META_IDENTIFIER_CHARACTER,       0,         0,         -1, "<meta identifier character>",       {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SPECIAL_SEQUENCE,                0,         0,         -1, "<special sequence>",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } }, /* To detect end of special sequence */
  {SPECIAL_SEQUENCE_CHARACTER,      0,         0,         -1, "<special sequence character>",      {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {COMMENT_SYMBOL,                  0,         0,         -1, "<comment symbol>",                  {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {BRACKETED_TEXTUAL_COMMENT,       0,         0,         -1, "<bracketed textual comment>",       {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTAX_RULE,                     0,         0,         -1, "<syntax rule>",                     {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {DEFINITIONS_LIST,                0,         0,         -1, "<definitions list>",                {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SINGLE_DEFINITION,               0,         0,         -1, "<single definition>",               {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTACTIC_TERM,                  0,         0,         -1, "<syntactic term>",                  {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTACTIC_EXCEPTION,             0,         0,         -1, "<syntactic exception>",             {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } }, /* no meta identifier! */
  {SYNTACTIC_FACTOR,                0,         0,         -1, "<syntactic factor>",                {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTACTIC_PRIMARY,               0,         0,         -1, "<syntactic primary>",               {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {OPTIONAL_SEQUENCE,               0,         0,         -1, "<optional sequence>",               {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {REPEATED_SEQUENCE,               0,         0,         -1, "<repeated sequence>",               {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {GROUPED_SEQUENCE,                0,         0,         -1, "<grouped sequence>",                {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {EMPTY_SEQUENCE,                  0,         0,         -1, "<empty sequence>",                  {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {COMMENT,                         0,         0,         -1, "<comment>",                         {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
    Note: handling of the exception with standard BNF
    *
    * Given a rule (if B is composite, this has to be derived as many times as necessary)
    *   X = A - B
    *
    * it is rewriten as:
    *   X = A
    *   X = E
    *   E = B
    *
    * with a prediction event on E. We use the value exceptioni to know that B is targetted
    *
    */
  /* ---------------- */
  /* Internal symbols */
  /* ---------------- */
  /*
   * gap free symbol = terminal character - (first quote symbol | second quote symbol)
   *                   | terminal string
   *
   * is revisited to:
   *
   * gap free symbol         = gap free symbol alt 1
   *                         | terminal string
   *
   * gap free symbol alt 1   = gap free symbol alt 1.1
   *                         | gap free symbol alt 1.2
   * gap free symbol alt 1.1 = terminal character
   * gap free symbol alt 1.2 = <first quote symbol KO>
   *                         | <second quote symbol KO>
   * <first quote symbol> = <first quote symbol KO>
   * <second quote symbol> = <second quote symbol KO>
   */
  /* ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                  rejectedb, eventSeti, exceptioni, descriptions                         { terminalb, startb,                                eventSeti } }
  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_GAP_FREE_SYMBOL_ALT_1,          0,         0,         -1, "<gap free symbol alt 1>",           {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_GAP_FREE_SYMBOL_ALT_1_1,        0,         0,         -1, "<gap free symbol alt 1.1>",         {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_GAP_FREE_SYMBOL_ALT_1_2,        0,         0,         -1, "<gap free symbol alt 1.2>",         {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_KO_FIRST_QUOTE_SYMBOL,          0,         0,  FIRST_QUOTE_SYMBOL, "<first quote symbol KO>",  {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION } },
  {_KO_SECOND_QUOTE_SYMBOL,         0,         0,  SECOND_QUOTE_SYMBOL, "<second quote symbol KO>",{       0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION } },
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
  {symboli                  rejectedb, eventSeti, exceptioni, descriptions                         { terminalb, startb,                                eventSeti } }
  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_FIRST_TERMINAL_CHARACTER_MANY,  0,         0,         -1, "<first terminal character many>",   {         0,      0,      MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SECOND_TERMINAL_CHARACTER_MANY, 0,         0,         -1, "<second terminal character many>",  {         0,      0,      MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * first terminal character = terminal character - first quote symbol
   *
   * is revisited to:
   *
   * first terminal character = first terminal character alt 1
   * first terminal character = first terminal character alt 2
   * first terminal character alt 1 = terminal character
   * first terminal character alt 2 = <first quote symbol KO>
   *
   */
  /* ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                  rejectedb, eventSeti, exceptioni, descriptions                         { terminalb, startb,                                eventSeti } }
  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_FIRST_TERMINAL_CHARACTER_ALT_1, 0,         0,         -1, "<first terminal character alt 1>",  {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_FIRST_TERMINAL_CHARACTER_ALT_2, 0,         0,         -1, "<first terminal character alt 2>",  {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * second terminal character = terminal character - second quote symbol
   *
   * is revisited to:
   *
   * second terminal character = second terminal character alt 1
   * second terminal character = second terminal character alt 2
   * second terminal character alt 1 = terminal character
   * second terminal character alt 2 = <second quote symbol KO>
   *
   */
  /* ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                  rejectedb, eventSeti, exceptioni, descriptions                         { terminalb, startb,                                eventSeti } }
  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_SECOND_TERMINAL_CHARACTER_ALT_1,0,         0,         -1, "<second terminal character alt 1>", {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SECOND_TERMINAL_CHARACTER_ALT_2,0,         0,         -1, "<second terminal character alt 2>", {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * syntax = {gap separator}, gap free symbol, {gap separator}, {gap free symbol, {gap separator}}
   *
   * could revisited to:
   *
   * <gap separator any> = <gap separator>*
   * <gap symbol unit> = <gap free symbol> <gap separator any>
   * <gap symbol unit many> = <gap free symbol unit>+
   * <syntax> = <gap separator any> <gap symbol unit many>
   *
   * but take care, there is an ambiguity:
   * <gap separator> is a non-printing character, in particular space ' '
   * <gap free symbol> is a <terminal character> minus the quotes or a <terminal string>. But a <terminal character> can be an <other character>, itself being potentially a ' ' character
   *
   * Now: EBNF says that a non printing character, i.e. <gap separator>
   * has no formal effect on a syntax if the character is outside a <terminal string>,
   * or pair of characters forming a single <terminal character>.
   *
   * Since we manage <terminal characters> as a single entity, this is resumed to:
   * the ' ' character cannot be an <other character> if we are not in a <terminal string> context.
   * The consequence
   *
   *
   */
  /* ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                  rejectedb, eventSeti, exceptioni, descriptions                         { terminalb, startb,                                eventSeti } }
  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_GAP_SEPARATOR_ANY,              0,         0,         -1, "<gap separator any>",               {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_GAP_SYMBOL_UNIT,                0,         0,         -1, "<gap symbol unit>",                 {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_GAP_SYMBOL_UNIT_MANY,           0,         0,         -1, "<gap symbol unit many>",            {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
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
   * commentless symbol alt 1 = commentless symbol alt 1.1
   *                            commentless symbol alt 1.2
   * commentless symbol alt 1.1 = terminal character
   * commentless symbol alt 1.2 = <letter KO>
   *                             | <decimal digit KO>
   *                             | <first quote symbol KO>
   *                             | <second quote symbol KO>
   *                             | <start comment symbol KO>
   *                             | <end comment symbol KO>
   *                             | <special sequence symbol KO>
   *                             | <other character KO>)
   *
   */
  /* ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                  rejectedb, eventSeti, exceptioni, descriptions                         { terminalb, startb,                                eventSeti } }
  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_COMMENTLESS_SYMBOL_ALT_1,       0,         0,         -1, "<commentless symbol alt 1>",        {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_COMMENTLESS_SYMBOL_ALT_1_1,     0,         0,         -1, "<commentless symbol alt 1.1>",      {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_COMMENTLESS_SYMBOL_ALT_1_2,     0,         0,         -1, "<commentless symbol alt 1.2>",      {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_KO_LETTER,                      0,         0, LETTER, "<letter KO>",                           {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION } },
  {_KO_DECIMAL_DIGIT,               0,         0, DECIMAL_DIGIT, "<decimal digit KO>",             {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION } },
  {_KO_START_COMMENT_SYMBOL,        0,         0, START_COMMENT_SYMBOL, "<start comment symbol KO>",{        0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION } },
  {_KO_END_COMMENT_SYMBOL,          0,         0, END_COMMENT_SYMBOL, "<end comment symbol KO>",   {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION } },
  {_KO_SPECIAL_SEQUENCE_SYMBOL,     0,         0, SPECIAL_SEQUENCE_SYMBOL, "<special sequence symbol KO>", { 0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION } },
  {_KO_OTHER_CHARACTER,             0,         0, OTHER_CHARACTER, "<other charactero KO>",        {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION } },
  /*
   * integer = decimal digit, {decimal digit}
   *
   * is revisited to:
   *
   * <integer> = <decimal digit>+
   */
  /*
   * meta identifier = letter, {meta identifier character}
   *
   * is revisited to:
   *
   * <meta identifier>               = <letter> _META_IDENTIFIER_CHARACTER_ANY
   * _META_IDENTIFIER_CHARACTER_ANY  = <meta identifier character>*
   *
   */
  /* ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                  rejectedb, eventSeti, exceptioni, descriptions                         { terminalb, startb,                                eventSeti } }
  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_META_IDENTIFIER_CHARACTER_ANY,  0,         0,         -1, "<meta identifier character any>",   {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * special sequence = special sequence symbol, {special sequence character}, special sequence symbol
   *
   * is revisited to:
   *
   * <special sequence character any> = <special sequence character>*
   * <special sequence> = <special sequence symbol> <special sequence character any> <special sequence symbol>
   */
  /* ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                  rejectedb, eventSeti, exceptioni, descriptions                         { terminalb, startb,                                eventSeti } }
  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_SPECIAL_SEQUENCE_CHARACTER_ANY, 0,         0,         -1, "<special sequence character any>",  {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * special sequence character = terminal character - special sequence symbol
   *
   * is revisited to:
   *
   * special sequence character = special sequence character alt 1
   * special sequence character = special sequence character alt 2
   * special sequence character alt 1 = terminal character
   * special sequence character alt 2 = <special sequence symbol KO>
   *
   */
  /* ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                  rejectedb, eventSeti, exceptioni, descriptions                         { terminalb, startb,                                eventSeti } }
  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_SPECIAL_SEQUENCE_CHARACTER_ALT_1,0,        0,         -1, "<special sequence character alt 1>",{         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SPECIAL_SEQUENCE_CHARACTER_ALT_2,0,        0,         -1, "<special sequence character alt 2>",{         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * bracketed textual comment = start comment symbol, {comment symbol}, end comment symbol
   *
   * is revisited to:
   *
   * <comment symbol any> = <comment symbol> <comment symbol any>
   * <comment symbol any> =
   * <bracketed textual comment> = <start comment symbol> <comment symbol any> <end comment symbol>
   */
  /* ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                  rejectedb, eventSeti, exceptioni, descriptions                         { terminalb, startb,                                eventSeti } }
  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_COMMENT_SYMBOL_ANY,             0,         0,         -1, "<comment symbol any>",              {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * syntax = {bracketed textual comment}, commentless symbol, {bracketed textual comment}, {commentless symbol, {bracketed textual comment}}
   *
   * is revisited to:
   *
   * <bracketed textual comment any> = <bracketed textual comment>*
   * <commentless symbol unit> = <commentless symbol> <bracketed textual comment any>
   * <commentless symbol unit many> = <commentless symbol unit>+
   * <syntax> = <bracketed textual comment any> <commentless symbol unit many>
   */
  /* ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                  rejectedb, eventSeti, exceptioni, descriptions                         { terminalb, startb,                                eventSeti } }
  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_BRACKETED_TEXTUAL_COMMENT_ANY,  0,         0,         -1, "<bracketed textual comment any>",    {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_COMMENTLESS_SYMBOL_UNIT,        0,         0,         -1, "<commentless symbol unit>",          {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_COMMENTLESS_SYMBOL_UNIT_MANY,   0,         0,         -1, "<commentless symbol unit many>",     {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * syntax = syntax rule, {syntax rule}
   *
   * is revisited to:
   *
   * <syntax> = <syntax rule>+
   */
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
  /* ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  {symboli                  rejectedb, eventSeti, exceptioni, descriptions                         { terminalb, startb,                                eventSeti } }
  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  {_DEFINITIONS_SEQUENCE,           0,         0,         -1, "<definitions sequence>",             {         0,      0,       MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
};


static marpaEBNFRule_t marpaEBNFRuleArray[] = {
  /* ------------------------------------------------------------------------------------------------------------------------------- */
  /*{ marpaWrapperGrammarRuleOption},         lhsSymboli,      rhsSymboll, { rhsSymbolip }                                           */
  /*  ranki
      nullRanksHighb
      sequenceb
      separatorSymboli
      properb
      minimumi
  */
  /* ------------------------------------------------------------------------------------------------------------------------------- */
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
  { { 0, 0, 0,            -1, 0, 0 }, _GAP_FREE_SYMBOL_ALT_1,           1, { _GAP_FREE_SYMBOL_ALT_1_1 } },
  { { 0, 0, 0,            -1, 0, 0 }, _GAP_FREE_SYMBOL_ALT_1,           1, { _GAP_FREE_SYMBOL_ALT_1_2 } },
  { { 0, 0, 0,            -1, 0, 0 }, _GAP_FREE_SYMBOL_ALT_1_1,         1, { TERMINAL_CHARACTER } },
  { { 0, 0, 0,            -1, 0, 0 }, _GAP_FREE_SYMBOL_ALT_1_2,         1, { _KO_FIRST_QUOTE_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, _GAP_FREE_SYMBOL_ALT_1_2,         1, { _KO_SECOND_QUOTE_SYMBOL } },

  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_STRING,                  3, { FIRST_QUOTE_SYMBOL, _FIRST_TERMINAL_CHARACTER_MANY, FIRST_QUOTE_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_STRING,                  3, { SECOND_QUOTE_SYMBOL, _SECOND_TERMINAL_CHARACTER_MANY, SECOND_QUOTE_SYMBOL } },
  { { 0, 0, 1,            -1, 0, 1 }, _FIRST_TERMINAL_CHARACTER_MANY,   1, { FIRST_TERMINAL_CHARACTER } },
  { { 0, 0, 1,            -1, 0, 1 }, _SECOND_TERMINAL_CHARACTER_MANY,  1, { SECOND_TERMINAL_CHARACTER } },

  { { 0, 0, 0,            -1, 0, 0 }, FIRST_TERMINAL_CHARACTER,         1, { _FIRST_TERMINAL_CHARACTER_ALT_1 } },
  { { 0, 0, 0,            -1, 0, 0 }, FIRST_TERMINAL_CHARACTER,         1, { _FIRST_TERMINAL_CHARACTER_ALT_2 } },
  { { 0, 0, 0,            -1, 0, 0 }, _FIRST_TERMINAL_CHARACTER_ALT_1,  1, { TERMINAL_CHARACTER } },
  { { 0, 0, 0,            -1, 0, 0 }, _FIRST_TERMINAL_CHARACTER_ALT_2,  1, { _KO_FIRST_QUOTE_SYMBOL } },

  { { 0, 0, 0,           -1, 0, 0 }, SECOND_TERMINAL_CHARACTER,         1, { _SECOND_TERMINAL_CHARACTER_ALT_1 } },
  { { 0, 0, 0,           -1, 0, 0 }, SECOND_TERMINAL_CHARACTER,         1, { _SECOND_TERMINAL_CHARACTER_ALT_2 } },
  { { 0, 0, 0,           -1, 0, 0 }, _SECOND_TERMINAL_CHARACTER_ALT_1,  1, { TERMINAL_CHARACTER } },
  { { 0, 0, 0,           -1, 0, 0 }, _SECOND_TERMINAL_CHARACTER_ALT_2,  2, { _KO_SECOND_QUOTE_SYMBOL } },

  { { 0, 0, 0,            -1, 0, 0 }, GAP_SEPARATOR,                    1, { SPACE_CHARACTER } },
  { { 0, 0, 0,            -1, 0, 0 }, GAP_SEPARATOR,                    1, { HORIZONTAL_TABULATION_CHARACTER } },
  { { 0, 0, 0,            -1, 0, 0 }, GAP_SEPARATOR,                    1, { NEW_LINE } },
  { { 0, 0, 0,            -1, 0, 0 }, GAP_SEPARATOR,                    1, { VERTICAL_TABULATION_CHARACTER } },
  { { 0, 0, 0,            -1, 0, 0 }, GAP_SEPARATOR,                    1, { FORM_FEED } },

  { { 0, 0, 1,            -1, 0, 0 }, _GAP_SEPARATOR_ANY,               1, { GAP_SEPARATOR } },
  { { 0, 0, 0,            -1, 0, 0 }, _GAP_SYMBOL_UNIT,                 2, { GAP_FREE_SYMBOL, _GAP_SEPARATOR_ANY } },
  { { 0, 0, 1,            -1, 0, 1 }, _GAP_SYMBOL_UNIT_MANY,            1, { _GAP_SYMBOL_UNIT } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTAX,                           2, { _GAP_SEPARATOR_ANY, _GAP_SYMBOL_UNIT_MANY } },

  { { 0, 0, 0,            -1, 0, 0 }, COMMENTLESS_SYMBOL,               1, { _COMMENTLESS_SYMBOL_ALT_1 } },
  { { 0, 0, 0,            -1, 0, 0 }, COMMENTLESS_SYMBOL,               1, { META_IDENTIFIER } },
  { { 0, 0, 0,            -1, 0, 0 }, COMMENTLESS_SYMBOL,               1, { INTEGER } },
  { { 0, 0, 0,            -1, 0, 0 }, COMMENTLESS_SYMBOL,               1, { TERMINAL_STRING } },
  { { 0, 0, 0,            -1, 0, 0 }, COMMENTLESS_SYMBOL,               1, { SPECIAL_SEQUENCE } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1,        1, { _COMMENTLESS_SYMBOL_ALT_1_1 } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1,        1, { _COMMENTLESS_SYMBOL_ALT_1_2 } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_1,      1, { TERMINAL_CHARACTER } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,      1, { _KO_LETTER } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,      1, { _KO_DECIMAL_DIGIT } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,      1, { _KO_FIRST_QUOTE_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,      1, { _KO_SECOND_QUOTE_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,      1, { _KO_START_COMMENT_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,      1, { _KO_END_COMMENT_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,      1, { _KO_SPECIAL_SEQUENCE_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,      1, { _KO_OTHER_CHARACTER } },

  { { 0, 0, 1,            -1, 0, 1 }, INTEGER,                          1, { DECIMAL_DIGIT } },

  { { 0, 0, 0,            -1, 0, 0 }, META_IDENTIFIER,                  2, { LETTER, _META_IDENTIFIER_CHARACTER_ANY } },
  { { 0, 0, 1,            -1, 0, 0 }, _META_IDENTIFIER_CHARACTER_ANY,   1, { META_IDENTIFIER_CHARACTER } },

  { { 0, 0, 0,            -1, 0, 0 }, META_IDENTIFIER_CHARACTER,        1, { LETTER } },
  { { 0, 0, 0,            -1, 0, 0 }, META_IDENTIFIER_CHARACTER,        1, { DECIMAL_DIGIT } },

  { { 0, 0, 1,            -1, 0, 0 }, _SPECIAL_SEQUENCE_CHARACTER_ANY,  1, { SPECIAL_SEQUENCE_CHARACTER } },
  { { 0, 0, 0,            -1, 0, 0 }, SPECIAL_SEQUENCE,                 3, { SPECIAL_SEQUENCE_SYMBOL, _SPECIAL_SEQUENCE_CHARACTER_ANY, SPECIAL_SEQUENCE_SYMBOL } },

  { { 0, 0, 0,            -1, 0, 0 }, SPECIAL_SEQUENCE_CHARACTER,       1, { _SPECIAL_SEQUENCE_CHARACTER_ALT_1 } },
  { { 0, 0, 0,            -1, 0, 0 }, SPECIAL_SEQUENCE_CHARACTER,       1, { _SPECIAL_SEQUENCE_CHARACTER_ALT_2 } },
  { { 0, 0, 0,            -1, 0, 0 }, _SPECIAL_SEQUENCE_CHARACTER_ALT_1,1, { TERMINAL_CHARACTER } },
  { { 0, 0, 0,            -1, 0, 0 }, _SPECIAL_SEQUENCE_CHARACTER_ALT_2,1, { _KO_SPECIAL_SEQUENCE_SYMBOL } },

  { { 0, 0, 0,            -1, 0, 0 }, COMMENT_SYMBOL,                   1, { BRACKETED_TEXTUAL_COMMENT } },
  { { 0, 0, 0,            -1, 0, 0 }, COMMENT_SYMBOL,                   1, { OTHER_CHARACTER } },
  { { 0, 0, 0,            -1, 0, 0 }, COMMENT_SYMBOL,                   1, { COMMENTLESS_SYMBOL } },

  { { 1, 0, 0,            -1, 0, 0 }, _COMMENT_SYMBOL_ANY,              2, { COMMENT_SYMBOL, _COMMENT_SYMBOL_ANY } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENT_SYMBOL_ANY,              0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, BRACKETED_TEXTUAL_COMMENT,        3, { START_COMMENT_SYMBOL, _COMMENT_SYMBOL_ANY, END_COMMENT_SYMBOL } },

  { { 0, 0, 1,            -1, 0, 0 }, _BRACKETED_TEXTUAL_COMMENT_ANY,   1, { BRACKETED_TEXTUAL_COMMENT } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_UNIT,         2, { COMMENTLESS_SYMBOL, _BRACKETED_TEXTUAL_COMMENT_ANY } },
  { { 0, 0, 1,            -1, 0, 1 }, _COMMENTLESS_SYMBOL_UNIT_MANY,    1, { _COMMENTLESS_SYMBOL_UNIT } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTAX,                           2, { _BRACKETED_TEXTUAL_COMMENT_ANY, _COMMENTLESS_SYMBOL_UNIT_MANY } },

  { { 1, 0, 0,            -1, 0, 0 }, SYNTAX,                           2, { SYNTAX_RULE, SYNTAX } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTAX,                           1, { SYNTAX_RULE } },

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
  genericStack_t        *inputStackp;
  genericStack_t        *outputStackp;
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

static inline int   _marpaEBNFAlternativeCmpi(const void *p1, const void *p2);
static inline short _marpaEBNFvalueRuleCallback(void *userDatavp, int rulei, int arg0i, int argni, int resulti);
static inline short _marpaEBNFvalueSymbolCallback(void *userDatavp, int symboli, int argi, int resulti);
static inline void  _genericStackFree(void *p);

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
  if (marpaEBNFp->symbolArrayp == NULL) {
    MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "malloc error, %s", strerror(errno));
    goto err;
  }
  marpaEBNFp->ruleArrayp           = (marpaEBNFRule_t *) malloc(marpaEBNFRuleArraySizel);
  if (marpaEBNFp->ruleArrayp == NULL) {
    MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "malloc error, %s", strerror(errno));
    goto err;
  }

  marpaWrapperGrammarOption.genericLoggerp    = marpaEBNFp->marpaEBNFOption.genericLoggerp;
  marpaWrapperGrammarOption.warningIsErrorb   = 0;
  marpaWrapperGrammarOption.warningIsIgnoredb = 0;
  marpaWrapperGrammarOption.autorankb         = 0;

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
  const static char             funcs[]                  = "marpaEBNF_grammarb";
  int                            linel                   = 1;
  int                            columnl                 = 1;
  marpaWrapperRecognizer_t      *marpaWrapperRecognizerp = NULL;
  marpaEBNFAlternative_t        *alternativep            = NULL;
  marpaWrapperValue_t           *marpaWrapperValuep      = NULL;
  GENERICSTACK_DECL(inputStackp)                         = NULL;
  GENERICSTACK_DECL(outputStackp)                        = NULL;

  marpaWrapperRecognizerOption_t marpaWrapperRecognizerOption;
  size_t                         eventl;
  marpaWrapperGrammarEvent_t    *eventp;
  size_t                         i;
  size_t                         nSymboll;
  int                           *expectedSymbolArrayp;
  int                            symboli, rejectedSymboli;
  marpaEBNFSymbol_t             *marpaEBNFSymbolp;
  char                           c, initialc;
  char                          *p, *q;
  char                          *maxp;
  size_t                         grammarLengthl;
  short                          okb;
  int                            lengthl, tmplengthl;
  marpaWrapperGrammarEventType_t eventTypee;
  char                          *tokens;
  size_t                         alternativel = 0;
  size_t                         alternativeokl = 0;
  marpaWrapperValueOption_t      marpaWrapperValueOption;
  short                          doCompleteb = 0;
  short                          rci;

  if ((marpaEBNFp == NULL) || (grammars == NULL)) {
    errno = EINVAL;
    goto err;
  }

  /* An ISO EBNF grammar is always expressed in the 7-bit ASCII range; that is entirely */
  /* covered by the basic C default character set. We just have to take care to always  */
  /* compare with a char, not an integer value.                                         */
  grammarLengthl = strlen(grammars);

  /* Create input stack */
  GENERICSTACK_NEW(inputStackp);
  if (GENERICSTACK_ERROR(inputStackp)) {
    MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "GENERICSTACK_ERROR error, %s", strerror(errno));
    goto err;
  }
  /* Marpa does not like the indice 0 with the input stack, it means an unvalued symbol */
  GENERICSTACK_PUSH_ANY(inputStackp, NULL, NULL, NULL);
  if (GENERICSTACK_ERROR(inputStackp)) {
    MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "GENERICSTACK_PUSH_ANY error, %s", strerror(errno));
    goto err;
  }

  /* Parse external grammar using internal grammar, supposed to fit entirely in memory */
  marpaWrapperRecognizerOption.genericLoggerp = marpaEBNFp->marpaEBNFOption.genericLoggerp;

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
        MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Invalid sequence of characters outside a terminal string or a special sequence context: %c%c%c", p[0], p[1], p[2]);
        goto err;
      }
    }

    /* When p is == maxp, we only take care of the events */
    
    /* Lookup expected terminals */
    if (marpaWrapperRecognizer_expectedb(marpaWrapperRecognizerp, &nSymboll, &expectedSymbolArrayp) == 0) {
      goto err;
    }
    /* Make sure we have enough space in the internal alternativep array */
    if (alternativep == NULL) {
      alternativep = malloc(nSymboll * sizeof(marpaEBNFAlternative_t));
      if (alternativep == NULL) {
        MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "malloc error, %s", strerror(errno));
	goto err;
      }
      alternativel = nSymboll;
    } else if (alternativel < nSymboll) {
      marpaEBNFAlternative_t *tmp = (marpaEBNFAlternative_t *) realloc(alternativep, nSymboll * sizeof(marpaEBNFAlternative_t));
      if (tmp == NULL) {
        MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "realloc error, %s", strerror(errno));
	goto err;
      }
      alternativep = tmp;
      alternativel = nSymboll;
    }

    /* Reset rejectedb for all expected terminals and eventSeti for all surveyed symbols */
    if (nSymboll > 0) {
      for (i = 0; i < nSymboll; i++) {
	marpaEBNFp->symbolArrayp[expectedSymbolArrayp[i]].rejectedb  = 0;
      }
    }
    
    /* Get events */
    if (marpaWrapperGrammar_eventb(marpaEBNFp->marpaWrapperGrammarp, &eventl, &eventp, 0) == 0) {
      goto err;
    }

    /* Remember which symbols have an event, set rejectedb to 1 for the rejected in advance symbols */
    if (eventl > 0) {
      for (i = 0; i < eventl; i++) {

	switch (eventp[i].eventType) {
	case MARPAWRAPPERGRAMMAR_EVENT_COMPLETED:
	  eventTypee = MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION;
          MARPAEBNF_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, funcs, "Completion event on symbol: %s", _marpaEBNF_symbolDescription(marpaEBNFp, eventp[i].symboli));
	  break;
	case MARPAWRAPPERGRAMMAR_EVENT_NULLED:
	  eventTypee = MARPAWRAPPERGRAMMAR_EVENTTYPE_NULLED;
          MARPAEBNF_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, funcs, "Nulled event on symbol: %s", _marpaEBNF_symbolDescription(marpaEBNFp, eventp[i].symboli));
	  break;
	case MARPAWRAPPERGRAMMAR_EVENT_EXPECTED:
	  eventTypee = MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION;
          MARPAEBNF_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, funcs, "Predicted event on symbol: %s", _marpaEBNF_symbolDescription(marpaEBNFp, eventp[i].symboli));
	  marpaEBNFSymbolp = &(marpaEBNFp->symbolArrayp[eventp[i].symboli]);
          rejectedSymboli = marpaEBNFSymbolp->exceptioni;
          if (rejectedSymboli >= 0) {
	    MARPAEBNF_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, funcs, "Predicted rejection on symbol: %s", _marpaEBNF_symbolDescription(marpaEBNFp, rejectedSymboli));
	    marpaEBNFSymbolp = &(marpaEBNFp->symbolArrayp[rejectedSymboli]);
            marpaEBNFSymbolp->rejectedb  = 1;
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

    /* If rejectedb is a true value, this just mean that at least one rule is rejecting the symbol */
    /* We have to look if there is at one rule that is NOT rejecting it.                           */

    alternativeokl = 0;
    if (nSymboll > 0) {

      /* ----------------------------- */
      /* look tothe expected terminals */
      /* ----------------------------- */
      if (p < maxp) {
	for (i = 0; i < nSymboll; i++) {
	  symboli = expectedSymbolArrayp[i];
	  marpaEBNFSymbolp = &(marpaEBNFp->symbolArrayp[symboli]);

          if (marpaEBNFSymbolp->rejectedb != 0) {
            /* Rejected in advance symbol */
            continue;
          }

	  okb = 0;
	  c = initialc = *p;
	  q = p + 1;

          MARPAEBNF_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, funcs, "Expected terminal: %s", _marpaEBNF_symbolDescription(marpaEBNFp, symboli), p);
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
	      while ((q < maxp) && (*q == '\r')) { ++tmplengthl; ++q; }
	      if ((q < maxp) && (*q++ == '\n')) { ++linel; columnl = 1; okb = 1; ++tmplengthl; }
	      if (okb == 1) {
		/* Followed by a \n: look to remaining \r */
		while ((q < maxp) && (*q == '\r')) { ++tmplengthl; ++q; }
	      }
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
	      if ((marpaEBNFp->terminalStringContexti != 0) || (marpaEBNFp->specialSequenceContexti > 0)) {
		switch (symboli) {
		case START_COMMENT_SYMBOL:
		case END_COMMENT_SYMBOL:
		case START_REPEAT_SYMBOL:
		case END_REPEAT_SYMBOL:
		case START_OPTION_SYMBOL:
		case END_OPTION_SYMBOL:
                  MARPAEBNF_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, funcs, "Rejected %s (terminal string or special sequence context)", _marpaEBNF_symbolDescription(marpaEBNFp, symboli));
		  okb = 0;
		  break;
		default:
		  break;
		}
	      }
	    } else {
              /* Reject ' ' if outside of a terminal string */
              switch (symboli) {
              case OTHER_CHARACTER:
                if ((initialc == ' ') && (marpaEBNFp->terminalStringContexti == 0)) {
                  MARPAEBNF_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, funcs, "Rejected %s (outside of terminal string context)", _marpaEBNF_symbolDescription(marpaEBNFp, symboli));
                  okb = 0;
                }
                break;
              default:
                break;
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

    if (alternativeokl > 0) {
      /* Say we will have to call for lexeme completion */
      doCompleteb = 1;
      /* Sort by longest length */
      qsort(alternativep, alternativeokl, sizeof(marpaEBNFAlternative_t), _marpaEBNFAlternativeCmpi);
      /* Keep only the longests */
      lengthl = alternativep[0].lengthl;
      for (i = 0; i < alternativeokl; i++) {
	if (alternativep[i].lengthl < lengthl) {
	  break;
	}
	symboli = alternativep[i].symboli;
	/* Our input stack is a copy of what was matched */
	tokens = malloc(lengthl + 1);
	if (tokens == NULL) {
          MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "malloc error, %s", strerror(errno));
	  goto err;
	}
	strncpy(tokens, p, lengthl);
	tokens[lengthl] = '\0';
	GENERICSTACK_PUSH_ANY(inputStackp, tokens, NULL, _genericStackFree);
        if (GENERICSTACK_ERROR(inputStackp)) {
          MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "GENERICSTACK_PUSH_ANY error, %s", strerror(errno));
          goto err;
        }
        MARPAEBNF_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, funcs, "Accepted terminal %s of length %ld: \"%s\"", _marpaEBNF_symbolDescription(marpaEBNFp, symboli), (unsigned long) lengthl, tokens);
        switch (symboli) {
        case SPECIAL_SEQUENCE_SYMBOL:
          if (marpaEBNFp->terminalStringContexti == 0) {
            MARPAEBNF_TRACE(marpaEBNFp->marpaEBNFOption.genericLoggerp, funcs, "Special sequence context is starting");
            marpaEBNFp->specialSequenceContexti = 1;
          } else {
            MARPAEBNF_TRACE(marpaEBNFp->marpaEBNFOption.genericLoggerp, funcs, "Special sequence context is ending");
            marpaEBNFp->specialSequenceContexti = 0;
          }
          break;
        case FIRST_QUOTE_SYMBOL:
        case SECOND_QUOTE_SYMBOL:
          if (marpaEBNFp->terminalStringContexti == 0) {
            MARPAEBNF_TRACE(marpaEBNFp->marpaEBNFOption.genericLoggerp, funcs, "Terminal string context is starting");
            marpaEBNFp->terminalStringContexti = 1;
          } else {
            MARPAEBNF_TRACE(marpaEBNFp->marpaEBNFOption.genericLoggerp, funcs, "Terminal string context is ending");
            marpaEBNFp->terminalStringContexti = 0;
          }
          break;
        default:
          break;
        }
	/* Length is a number of earlemes from grammar point of view - we maintain internally the real pointer position */
	if (marpaWrapperRecognizer_alternativeb(marpaWrapperRecognizerp, symboli, GENERICSTACK_SIZE(inputStackp) - 1, 1) == 0) {
	  goto err;
	}
      }
    }

    if (doCompleteb != 0) {
      /* This will naturally fail if we pushed no alternative */
      if (marpaWrapperRecognizer_completeb(marpaWrapperRecognizerp) == 0) {
	goto err;
      }
    } else {
      /* This is an error if we are not at the end of the grammar */
      if (p < maxp) {
        MARPAEBNF_ERROR(marpaEBNFp->marpaEBNFOption.genericLoggerp, "No terminal nor discard detected");
	goto err;
      }
    }

    if (p < maxp) {
      /* Resume */   
      MARPAEBNF_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, funcs, "Moving forward by %ld", lengthl);
      p += lengthl;
    } else {
      break;
    }
  }

  /* Ask for the value. We require it to be explicitely unambigous. Since we have no rank, let's specify it. */
  marpaWrapperValueOption.genericLoggerp = marpaEBNFp->marpaEBNFOption.genericLoggerp;
  marpaWrapperValueOption.highRankOnlyb  = 0;
  marpaWrapperValueOption.orderByRankb   = 0;
  marpaWrapperValueOption.ambiguousb     = 1;
  marpaWrapperValueOption.nullb          = 0;
  marpaWrapperValuep = marpaWrapperValue_newp(marpaWrapperRecognizerp, &marpaWrapperValueOption);
  if (marpaWrapperValuep == NULL) {
    goto err;
  }

  rci = 0;
  /* Remember input stack */
  marpaEBNFp->inputStackp = inputStackp;

  /* Create and remember output stack */
  GENERICSTACK_NEW(outputStackp);
  if (GENERICSTACK_ERROR(outputStackp)) {
    MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "GENERICSTACK_ERROR error, %s", strerror(errno));
    goto err;
  }
  marpaEBNFp->outputStackp = outputStackp;

  /* No need for a loop: we required the parse tree to be unambiguous */
  while (marpaWrapperValue_valueb(marpaWrapperValuep,
			       marpaEBNFp,
			       _marpaEBNFvalueRuleCallback,
			       _marpaEBNFvalueSymbolCallback,
			       NULL) > 0) {
    MARPAEBNF_TRACE(marpaEBNFp->marpaEBNFOption.genericLoggerp, funcs, "---------- Got a value");
    if (++rci > 1) {
      goto err;
    }
  }

  rci = 1;
  goto done;

 err:
  rci = 0;

 done:
  if (inputStackp != NULL) {
    GENERICSTACK_FREE(inputStackp);
  }
  if (outputStackp != NULL) {
    GENERICSTACK_FREE(outputStackp);
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
      if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
        marpaWrapperRecognizer_progressLogb(marpaWrapperRecognizerp, -1, -1, GENERICLOGGER_LOGLEVEL_ERROR, marpaEBNFp, _marpaEBNF_symbolDescription);
      }
    }
    marpaWrapperRecognizer_freev(marpaWrapperRecognizerp);
  }
  return rci;
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

/****************************************************************************/
static inline short _marpaEBNFvalueRuleCallback(void *userDatavp, int rulei, int arg0i, int argni, int resulti)
/****************************************************************************/
{
  marpaEBNF_t       *marpaEBNFp = (marpaEBNF_t *) userDatavp;
  marpaEBNFRule_t   *rulep      = &(marpaEBNFp->ruleArrayp[rulei]);
  marpaEBNFSymbol_t *lhsp       = &(marpaEBNFp->symbolArrayp[rulep->lhsSymboli]);
  size_t             i;

  if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
    if (rulep->rhsSymboll > 0) {
      if (rulep->rhsSymboll == 1) {
        if (marpaEBNFp->ruleArrayp[rulei].option.sequenceb != 0) {
          MARPAEBNF_INFOF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Rule No %2d value callback: %s = %s%s ;", rulei, lhsp->descriptions, marpaEBNFp->symbolArrayp[rulep->rhsSymbolip[0]].descriptions, (marpaEBNFp->ruleArrayp[rulei].option.minimumi == 0) ? "*" : "+");
        } else {
          MARPAEBNF_INFOF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Rule No %2d value callback: %s = %s ;", rulei, lhsp->descriptions, marpaEBNFp->symbolArrayp[rulep->rhsSymbolip[0]].descriptions);
        }
      } else {
	MARPAEBNF_INFOF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Rule No %2d value callback: %s = %s", rulei, lhsp->descriptions, marpaEBNFp->symbolArrayp[rulep->rhsSymbolip[0]].descriptions);
	for (i = 1; i < rulep->rhsSymboll; i++) {
	  if (i == rulep->rhsSymboll - 1) {
	    MARPAEBNF_INFOF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "\t%s ;", marpaEBNFp->symbolArrayp[rulep->rhsSymbolip[i]].descriptions);
	  } else {
	    MARPAEBNF_INFOF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "\t%s", marpaEBNFp->symbolArrayp[rulep->rhsSymbolip[i]].descriptions);
	  }
	}
      }
    } else {
      MARPAEBNF_INFOF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Rule No %2d value callback: %s ;", rulei, lhsp->descriptions);
    }
  }

  return 1;
}

/****************************************************************************/
static inline short _marpaEBNFvalueSymbolCallback(void *userDatavp, int symboli, int argi, int resulti)
/****************************************************************************/
{
  marpaEBNF_t       *marpaEBNFp  = (marpaEBNF_t *) userDatavp;
  marpaEBNFSymbol_t *symbolp     = &(marpaEBNFp->symbolArrayp[symboli]);
  genericStack_t    *inputStackp  = marpaEBNFp->inputStackp;
  genericStack_t    *outputStackp = marpaEBNFp->outputStackp;
  char              *inputs;
  char              *outputs;

  inputs = GENERICSTACK_GET_PTR(inputStackp, argi);

  if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
    if (inputs != NULL) {
      MARPAEBNF_INFOF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "  Symbol No %2d value callback: %s, input: \"%s\"", symboli, symbolp->descriptions, inputs);
    } else {
      MARPAEBNF_INFOF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "  Symbol No %2d value callback: %s, input: NULL", symboli, symbolp->descriptions);
    }
  }

  if (inputs == NULL) {
    /* This is legal and happen only with our fake terminals */
    GENERICSTACK_SET_ANY(outputStackp, NULL, NULL, NULL, argi);
  } else {
    outputs = strdup(inputs);
    if (outputs == NULL) {
      MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "strdup error, %s", strerror(errno));
      goto err;
    }
    GENERICSTACK_SET_ANY(outputStackp, outputs, NULL, _genericStackFree, argi);
  }

  if (GENERICSTACK_ERROR(outputStackp)) {
    MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "GENERICSTACK_SET_ANY error, %s", strerror(errno));
    goto err;
  }

  return 1;

 err:
  return 0;
}

/****************************************************************************/
static inline void  _genericStackFree(void *p)
/****************************************************************************/
{
  free(p);
}
