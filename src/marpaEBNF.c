#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "config.h"
#include "genericstack.h"
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
  COMMENT,
  /* Internal symbols */
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
  short                             exceptionb;
  char                             *descriptions;
  marpaWrapperGrammarSymbolOption_t option;
} marpaEBNFSymbol_t;

#define MARPAEBNF_SYMBOL_IS_MARKER(marpaEBNFSymbolp) ((marpaEBNFSymbolp)->markerb != 0)
#define MARPAEBNF_SYMBOL_IS_MARKER_OK(marpaEBNFSymbolp) (MARPAEBNF_SYMBOL_IS_MARKER(marpaEBNFSymbolp) && ((marpaEBNFSymbolp)->exceptionb == 0))
#define MARPAEBNF_SYMBOL_IS_MARKER_KO(marpaEBNFSymbolp) (MARPAEBNF_SYMBOL_IS_MARKER(marpaEBNFSymbolp) && ((marpaEBNFSymbolp)->exceptionb != 0))

typedef struct marpaEBNFRule {
  char                             *descriptions;
  marpaWrapperGrammarRuleOption_t   option;
  int                               lhsSymboli;
  size_t                            rhsSymboll;
  int                               rhsSymbolip[20]; /* Flexible array and ANSI-C are not friends */
} marpaEBNFRule_t;

/* List of all symbols of the EBNF grammar as per ISO/IEC 14977:1996 */
static marpaEBNFSymbol_t marpaEBNFSymbolArray[] = {
  /* -----------------------------------------------------------------------------------------------------------------------------------
    symboli                   markerb, exceptionb, descriptions                { terminalb, startb,                          eventSeti }
    ------------------------------------------------------------------------------------------------------------------------------------
  */
  /*
   *                                TERMINAL CHARACTERS
   */
  {LETTER,                          0, 0, "<letter>",                          {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {DECIMAL_DIGIT,                   0, 0, "<decimal digit>",                   {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {CONCATENATE_SYMBOL,              0, 0, "<concatenate symbol>",              {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {DEFINING_SYMBOL,                 0, 0, "<defining symbol>",                 {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {DEFINITION_SEPARATOR_SYMBOL,     0, 0, "<definition separator symbol>",     {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {END_COMMENT_SYMBOL,              0, 0, "<end comment symbol>",              {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {END_GROUP_SYMBOL,                0, 0, "<end group symbol>",                {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {END_OPTION_SYMBOL,               0, 0, "<end option symbol>",               {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {END_REPEAT_SYMBOL,               0, 0, "<end repeat symbol>",               {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {EXCEPT_SYMBOL,                   0, 0, "<except symbol>",                   {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } }, /* To detect meta identifier */
  {FIRST_QUOTE_SYMBOL,              0, 0, "<first quote symbol>",              {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {REPETITION_SYMBOL,               0, 0, "<repetition symbol>",               {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SECOND_QUOTE_SYMBOL,             0, 0, "<second quote symbol>",             {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SPECIAL_SEQUENCE_SYMBOL,         0, 0, "<special sequence symbol>",         {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {START_COMMENT_SYMBOL,            0, 0, "<start comment symbol>",            {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {START_GROUP_SYMBOL,              0, 0, "<start group symbol>",              {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {START_OPTION_SYMBOL,             0, 0, "<start option symbol>",             {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {START_REPEAT_SYMBOL,             0, 0, "<start repeat symbol>",             {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {TERMINATOR_SYMBOL,               0, 0, "<terminator symbol>",               {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {OTHER_CHARACTER,                 0, 0, "<other character>",                 {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SPACE_CHARACTER,                 0, 0, "<space character>",                 {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {HORIZONTAL_TABULATION_CHARACTER, 0, 0, "<horizontal tabulation character>", {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {NEW_LINE,                        0, 0, "<new line>",                        {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {VERTICAL_TABULATION_CHARACTER,   0, 0, "<vertical tabulation character>",   {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {FORM_FEED,                       0, 0, "<form feed>",                       {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   *                              NON-TERMINAL CHARACTERS
   */
  {TERMINAL_CHARACTER,              0, 0, "<terminal character>",              {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {GAP_FREE_SYMBOL,                 0, 0, "<gap free symbol>",                 {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {TERMINAL_STRING,                 0, 0, "<terminal string>",                 {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {FIRST_TERMINAL_CHARACTER,        0, 0, "<first terminal character>",        {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SECOND_TERMINAL_CHARACTER,       0, 0, "<second terminal character>",       {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {GAP_SEPARATOR,                   0, 0, "<gap separator>",                   {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTAX,                          0, 0, "<syntax>",                          {         0,      1, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },  /* START */
  {COMMENTLESS_SYMBOL,              0, 0, "<commentless symbol>",              {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {INTEGER,                         0, 0, "<integer>",                         {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {META_IDENTIFIER,                 0, 0, "<meta identifier>",                 {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {META_IDENTIFIER_CHARACTER,       0, 0, "<meta identifier character>",       {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SPECIAL_SEQUENCE,                0, 0, "<special sequence>",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SPECIAL_SEQUENCE_CHARACTER,      0, 0, "<special sequence character>",      {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {COMMENT_SYMBOL,                  0, 0, "<comment symbol>",                  {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {BRACKETED_TEXTUAL_COMMENT,       0, 0, "<bracketed textual comment>",       {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTAX_RULE,                     0, 0, "<syntax rule>",                     {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {DEFINITIONS_LIST,                0, 0, "<definitions list>",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SINGLE_DEFINITION,               0, 0, "<single definition>",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTACTIC_TERM,                  0, 0, "<syntactic term>",                  {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTACTIC_EXCEPTION,             0, 0, "<syntactic exception>",             {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } }, /* no meta identifier! */
  {SYNTACTIC_FACTOR,                0, 0, "<syntactic factor>",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTACTIC_PRIMARY,               0, 0, "<syntactic primary>",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {OPTIONAL_SEQUENCE,               0, 0, "<optional sequence>",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {REPEATED_SEQUENCE,               0, 0, "<repeated sequence>",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {GROUPED_SEQUENCE,                0, 0, "<grouped sequence>",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {EMPTY_SEQUENCE,                  0, 0, "<empty sequence>",                  {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {COMMENT,                         0, 0, "<comment>",                         {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
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
    * 1. If the context is the X rule
    *   a. If there is an event on A we insert MARKER_A
    *   b. If there is an event on B we do nothing
    * 2. Else
    *   a. we do nothing
    *
    * For performance reasons, the above algorithm is done in reverse way:
    * 1. If there is an event on A
    *   a. If the context is rule X we insert MARKER_A
    *   b. Else we do nothing
    * 2. Else
    *   a. we do nothing
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
  {_GAP_FREE_SYMBOL_ALT_1,          0, 0, "<gap free symbol alt 1>",           {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_GAP_FREE_SYMBOL_ALT_1_1,        0, 0, "<gap free symbol alt 1.1>",         {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } },
  {_GAP_FREE_SYMBOL_ALT_1_2,        0, 0, "<gap free symbol alt 1.2>",         {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } },
  {_GAP_FREE_SYMBOL_ALT_1_1_MARKER, 1, 0, "<gap free symbol alt 1.1 marker>",  {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_GAP_FREE_SYMBOL_ALT_1_2_MARKER, 1, 1, "<gap free symbol alt 1.2 marker>",  {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
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
  {_FIRST_TERMINAL_CHARACTER_MANY,  0, 0, "<first terminal character +>",    {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SECOND_TERMINAL_CHARACTER_MANY, 0, 0, "<second terminal character +>",   {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
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
  {_FIRST_TERMINAL_CHARACTER_ALT_1,        0, 0, "<first terminal character alt 1>",        {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } },
  {_FIRST_TERMINAL_CHARACTER_ALT_1_MARKER, 1, 0, "<first terminal character alt 1 marker>", {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_FIRST_TERMINAL_CHARACTER_ALT_2,        0, 0, "<first terminal character alt 2>",        {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } },
  {_FIRST_TERMINAL_CHARACTER_ALT_2_MARKER, 1, 1, "<first terminal character alt 2 marker>", {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
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
  {_SECOND_TERMINAL_CHARACTER_ALT_1,        0, 0, "<second terminal character alt 1>",        {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } },
  {_SECOND_TERMINAL_CHARACTER_ALT_1_MARKER, 1, 0, "<second terminal character alt 1 marker>", {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SECOND_TERMINAL_CHARACTER_ALT_2,        0, 0, "<second terminal character alt 2>",        {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } },
  {_SECOND_TERMINAL_CHARACTER_ALT_2_MARKER, 1, 1, "<second terminal character alt 2 marker>", {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * syntax = {gap separator}, gap free symbol, {gap separator}, {gap free symbol, {gap separator}}
   *
   * is revisited to:
   *
   * <gap separator any> = <gap separator>*
   * <gap free symbol many> = <gap free symbol>+ separator => <gap separator> proper => 0
   * <syntax> = <gap separator any> <gap free symbol many>
   */
  {_GAP_SEPARATOR_ANY,              0, 0, "<gap separator *>",              {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_GAP_FREE_SYMBOL_MANY,           0, 0, "<gap free symbol +>",            {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
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
  {_COMMENTLESS_SYMBOL_ALT_1,          0, 0, "<commentless symbol alt 1>",           {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_COMMENTLESS_SYMBOL_ALT_1_1,        0, 0, "<commentless symbol alt 1.1>",         {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } },
  {_COMMENTLESS_SYMBOL_ALT_1_2,        0, 0, "<commentless symbol alt 1.2>",         {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } },
  {_COMMENTLESS_SYMBOL_ALT_1_1_MARKER, 1, 0, "<commentless symbol alt 1.1 marker>",  {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_COMMENTLESS_SYMBOL_ALT_1_2_MARKER, 1, 1, "<commentless symbol alt 1.2 marker>",  {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * integer = decimal digit, {decimal digit}
   *
   * is revisited to:
   *
   * <integer> = <decimal digit many>
   * <decimal digit many> = <decimal digit>+
   */
  {_DECIMAL_DIGIT_MANY,             0, 0, "<decimal digit +>",                         {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * meta identifier = letter, {meta identifier character}
   *
   * is revisited to:
   *
   * <meta identifier> = <letter> <meta identifier character any>
   * <meta identifier character any> = <meta identifier character>*
   */
  {_META_IDENTIFIER_CHARACTER_ANY,   0, 0, "<meta identifier character *>",            {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * special sequence = special sequence symbol, {special sequence character}, special sequence symbol
   *
   * is revisited to:
   *
   * <special sequence character any> = <special sequence character>*
   * <special sequence> = <special sequence symbol> <special sequence character any> <special sequence symbol>
   */
  {_SPECIAL_SEQUENCE_CHARACTER_ANY,   0, 0, "<special sequence character *>",          {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
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
  {_SPECIAL_SEQUENCE_CHARACTER_ALT_1,        0, 0, "<special sequence character alt 1>",        {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } },
  {_SPECIAL_SEQUENCE_CHARACTER_ALT_1_MARKER, 1, 0, "<special sequence character alt 1 marker>", {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SPECIAL_SEQUENCE_CHARACTER_ALT_2,        0, 0, "<special sequence character alt 2>",        {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } },
  {_SPECIAL_SEQUENCE_CHARACTER_ALT_2_MARKER, 1, 1, "<special sequence character alt 2 marker>", {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * bracketed textual comment = start comment symbol, {comment symbol}, end comment symbol
   *
   * is revisited to:
   *
   * <comment symbol any> = <comment symbol>*
   * <bracketed textual comment> = <start comment symbol> <comment symbol any> <end comment symbol>
   */
  {_COMMENT_SYMBOL_ANY,   0, 0, "<comment symbol *>",                                           {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
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
  {_BRACKETED_TEXTUAL_COMMENT_ANY, 0, 0, "<bracketed textual comment *>", { 0, 0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SYNTAX_UNIT,                   0, 0, "<syntax unit>",                 { 0, 0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SYNTAX_UNIT_MANY,              0, 0, "<syntax unit +>",               { 0, 0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SYNTAX_01,                     0, 0, "<syntax 01>",                   { 0, 0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * syntax = syntax rule, {syntax rule}
   *
   * is revisited to:
   *
   * <syntax rule many>> = <syntax rule>+
   * <syntax 02> = <syntax rule many>
   * <syntax> = <syntax 01>
   */
  {_SYNTAX_RULE_MANY,              0, 0, "<syntax rule +>",              { 0, 0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SYNTAX_02,                     0, 0, "<syntax 02>",                  { 0, 0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
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
  {_DEFINITIONS_SEQUENCE,          0, 0, "<definitions sequence>",       { 0, 0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
};


static marpaEBNFRule_t marpaEBNFRuleArray[] = {
  { "<terminal character>",          { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { LETTER } },
  { "<terminal character>",          { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { DECIMAL_DIGIT } },
  { "<terminal character>",          { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { CONCATENATE_SYMBOL } },
  { "<terminal character>",          { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { DEFINING_SYMBOL } },
  { "<terminal character>",          { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { DEFINITION_SEPARATOR_SYMBOL } },
  { "<terminal character>",          { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { END_COMMENT_SYMBOL } },
  { "<terminal character>",          { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { END_GROUP_SYMBOL } },
  { "<terminal character>",          { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { END_OPTION_SYMBOL } },
  { "<terminal character>",          { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { END_REPEAT_SYMBOL } },
  { "<terminal character>",          { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { EXCEPT_SYMBOL } },
  { "<terminal character>",          { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { FIRST_QUOTE_SYMBOL } },
  { "<terminal character>",          { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { REPETITION_SYMBOL } },
  { "<terminal character>",          { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { SECOND_QUOTE_SYMBOL } },
  { "<terminal character>",          { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { SPECIAL_SEQUENCE_SYMBOL } },
  { "<terminal character>",          { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { START_COMMENT_SYMBOL } },
  { "<terminal character>",          { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { START_GROUP_SYMBOL } },
  { "<terminal character>",          { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { START_OPTION_SYMBOL } },
  { "<terminal character>",          { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { START_REPEAT_SYMBOL } },
  { "<terminal character>",          { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { TERMINATOR_SYMBOL } },
  { "<terminal character>",          { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { OTHER_CHARACTER } },

  { "<gap free symbol>",             { 0, 0, 0,            -1, 0, 0 }, GAP_FREE_SYMBOL,                  1, { _GAP_FREE_SYMBOL_ALT_1 } },
  { "<gap free symbol>",             { 0, 0, 0,            -1, 0, 0 }, GAP_FREE_SYMBOL,                  1, { TERMINAL_STRING } },
  { "<gap free symbol alt 1>",       { 0, 0, 0,            -1, 0, 0 }, _GAP_FREE_SYMBOL_ALT_1,           2, { _GAP_FREE_SYMBOL_ALT_1_1, _GAP_FREE_SYMBOL_ALT_1_1_MARKER } },
  { "<gap free symbol alt 1>",       { 0, 0, 0,            -1, 0, 0 }, _GAP_FREE_SYMBOL_ALT_1,           2, { _GAP_FREE_SYMBOL_ALT_1_2, _GAP_FREE_SYMBOL_ALT_1_2_MARKER } },
  { "<gap free symbol alt 1.1>",     { 0, 0, 0,            -1, 0, 0 }, _GAP_FREE_SYMBOL_ALT_1_1,         1, { TERMINAL_CHARACTER } },
  { "<gap free symbol alt 1.2>",     { 0, 0, 0,            -1, 0, 0 }, _GAP_FREE_SYMBOL_ALT_1_2,         1, { FIRST_QUOTE_SYMBOL } },
  { "<gap free symbol alt 1.2>",     { 0, 0, 0,            -1, 0, 0 }, _GAP_FREE_SYMBOL_ALT_1_2,         1, { SECOND_QUOTE_SYMBOL } },

  { "<terminal string>",                { 0, 0, 0,         -1, 0, 0 }, TERMINAL_STRING,                  3, { FIRST_QUOTE_SYMBOL, _FIRST_TERMINAL_CHARACTER_MANY, FIRST_QUOTE_SYMBOL } },
  { "<terminal string>",                { 0, 0, 0,         -1, 0, 0 }, TERMINAL_STRING,                  3, { SECOND_QUOTE_SYMBOL, _SECOND_TERMINAL_CHARACTER_MANY, SECOND_QUOTE_SYMBOL } },
  { "<first terminal character many>",  { 0, 0, 1,         -1, 0, 1 }, _FIRST_TERMINAL_CHARACTER_MANY,   1, { FIRST_TERMINAL_CHARACTER } },
  { "<second terminal character many>", { 0, 0, 1,         -1, 0, 1 }, _SECOND_TERMINAL_CHARACTER_MANY,  1, { SECOND_TERMINAL_CHARACTER } },

  { "<first terminal character>",       { 0, 0, 0,         -1, 0, 0 }, FIRST_TERMINAL_CHARACTER,         2, { _FIRST_TERMINAL_CHARACTER_ALT_1, _FIRST_TERMINAL_CHARACTER_ALT_1_MARKER } },
  { "<first terminal character>",       { 0, 0, 0,         -1, 0, 0 }, FIRST_TERMINAL_CHARACTER,         2, { _FIRST_TERMINAL_CHARACTER_ALT_2, _FIRST_TERMINAL_CHARACTER_ALT_2_MARKER } },
  { "<first terminal character alt 1>", { 0, 0, 0,         -1, 0, 0 }, _FIRST_TERMINAL_CHARACTER_ALT_1,  1, { TERMINAL_CHARACTER } },
  { "<first terminal character alt 2>", { 0, 0, 0,         -1, 0, 0 }, _FIRST_TERMINAL_CHARACTER_ALT_2,  1, { FIRST_QUOTE_SYMBOL } },

  { "<second terminal character>",       { 0, 0, 0,        -1, 0, 0 }, SECOND_TERMINAL_CHARACTER,         2, { _SECOND_TERMINAL_CHARACTER_ALT_1, _SECOND_TERMINAL_CHARACTER_ALT_1_MARKER } },
  { "<second terminal character>",       { 0, 0, 0,        -1, 0, 0 }, SECOND_TERMINAL_CHARACTER,         2, { _SECOND_TERMINAL_CHARACTER_ALT_2, _SECOND_TERMINAL_CHARACTER_ALT_2_MARKER } },
  { "<second terminal character alt 1>", { 0, 0, 0,        -1, 0, 0 }, _SECOND_TERMINAL_CHARACTER_ALT_1,  1, { TERMINAL_CHARACTER } },
  { "<second terminal character alt 2>", { 0, 0, 0,        -1, 0, 0 }, _SECOND_TERMINAL_CHARACTER_ALT_2,  1, { SECOND_QUOTE_SYMBOL } },

  { "<gap separator>",               { 0, 0, 0,            -1, 0, 0 }, GAP_SEPARATOR,                    1, { SPACE_CHARACTER } },
  { "<gap separator>",               { 0, 0, 0,            -1, 0, 0 }, GAP_SEPARATOR,                    1, { HORIZONTAL_TABULATION_CHARACTER } },
  { "<gap separator>",               { 0, 0, 0,            -1, 0, 0 }, GAP_SEPARATOR,                    1, { NEW_LINE } },
  { "<gap separator>",               { 0, 0, 0,            -1, 0, 0 }, GAP_SEPARATOR,                    1, { VERTICAL_TABULATION_CHARACTER } },
  { "<gap separator>",               { 0, 0, 0,            -1, 0, 0 }, GAP_SEPARATOR,                    1, { FORM_FEED } },

  { "<gap separator any>",           { 0, 0, 1,            -1, 0, 0 }, _GAP_SEPARATOR_ANY,               1, { GAP_SEPARATOR } },
  { "<gap free symbol many>",        { 0, 0, 1, GAP_SEPARATOR, 0, 1 }, _GAP_FREE_SYMBOL_MANY,            1, { GAP_SEPARATOR } },
  { "<syntax>",                      { 0, 0, 0,            -1, 0, 0 }, SYNTAX,                           2, { _GAP_SEPARATOR_ANY, _GAP_FREE_SYMBOL_MANY } },

  { "<commentless symbol>",         { 0, 0, 0,            -1, 0, 0 }, COMMENTLESS_SYMBOL,              1, { _COMMENTLESS_SYMBOL_ALT_1 } },
  { "<commentless symbol>",         { 0, 0, 0,            -1, 0, 0 }, COMMENTLESS_SYMBOL,              1, { META_IDENTIFIER } },
  { "<commentless symbol>",         { 0, 0, 0,            -1, 0, 0 }, COMMENTLESS_SYMBOL,              1, { INTEGER } },
  { "<commentless symbol>",         { 0, 0, 0,            -1, 0, 0 }, COMMENTLESS_SYMBOL,              1, { TERMINAL_STRING } },
  { "<commentless symbol>",         { 0, 0, 0,            -1, 0, 0 }, COMMENTLESS_SYMBOL,              1, { SPECIAL_SEQUENCE } },
  { "<commentless symbol alt 1>",   { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1,       2, { _COMMENTLESS_SYMBOL_ALT_1_1, _COMMENTLESS_SYMBOL_ALT_1_1_MARKER } },
  { "<commentless symbol alt 1>",   { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1,       2, { _COMMENTLESS_SYMBOL_ALT_1_2, _COMMENTLESS_SYMBOL_ALT_1_2_MARKER } },
  { "<commentless symbol alt 1.1>", { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_1,     1, { TERMINAL_CHARACTER } },
  { "<commentless symbol alt 1.2>", { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,     1, { LETTER } },
  { "<commentless symbol alt 1.2>", { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,     1, { DECIMAL_DIGIT } },
  { "<commentless symbol alt 1.2>", { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,     1, { FIRST_QUOTE_SYMBOL } },
  { "<commentless symbol alt 1.2>", { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,     1, { SECOND_QUOTE_SYMBOL } },
  { "<commentless symbol alt 1.2>", { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,     1, { START_COMMENT_SYMBOL } },
  { "<commentless symbol alt 1.2>", { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,     1, { END_COMMENT_SYMBOL } },
  { "<commentless symbol alt 1.2>", { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,     1, { SPECIAL_SEQUENCE_SYMBOL } },
  { "<commentless symbol alt 1.2>", { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_ALT_1_2,     1, { OTHER_CHARACTER } },

  { "<decimal digit many>",          { 0, 0, 1,            -1, 0, 1 }, _DECIMAL_DIGIT_MANY,              1, { DECIMAL_DIGIT } },
  { "<integer>",                     { 0, 0, 0,            -1, 0, 0 }, INTEGER,                          1, { _DECIMAL_DIGIT_MANY } },

  { "<meta identifier character any>", { 0, 0, 1,          -1, 0, 0 }, _META_IDENTIFIER_CHARACTER_ANY,   1, { META_IDENTIFIER_CHARACTER } },
  { "<meta identifier>",             { 0, 0, 0,            -1, 0, 0 }, META_IDENTIFIER,                  2, { LETTER, _META_IDENTIFIER_CHARACTER_ANY } },

  { "<meta identifier character>",   { 0, 0, 0,            -1, 0, 0 }, META_IDENTIFIER_CHARACTER,        1, { LETTER } },
  { "<meta identifier character>",   { 0, 0, 0,            -1, 0, 0 }, META_IDENTIFIER_CHARACTER,        1, { DECIMAL_DIGIT } },

  { "<special sequence character any>", { 0, 0, 1,         -1, 0, 0 }, _SPECIAL_SEQUENCE_CHARACTER_ANY,  1, { SPECIAL_SEQUENCE_CHARACTER } },
  { "<special sequence>",            { 0, 0, 0,            -1, 0, 0 }, SPECIAL_SEQUENCE,                 3, { SPECIAL_SEQUENCE_SYMBOL, _SPECIAL_SEQUENCE_CHARACTER_ANY, SPECIAL_SEQUENCE_SYMBOL } },

  { "<special sequence character>",       { 0, 0, 0,       -1, 0, 0 }, SPECIAL_SEQUENCE_CHARACTER,         2, { _SPECIAL_SEQUENCE_CHARACTER_ALT_1, _SPECIAL_SEQUENCE_CHARACTER_ALT_1_MARKER } },
  { "<special sequence character>",       { 0, 0, 0,       -1, 0, 0 }, SPECIAL_SEQUENCE_CHARACTER,         2, { _SPECIAL_SEQUENCE_CHARACTER_ALT_2, _SPECIAL_SEQUENCE_CHARACTER_ALT_2_MARKER } },
  { "<special sequence character alt 1>", { 0, 0, 0,       -1, 0, 0 }, _SPECIAL_SEQUENCE_CHARACTER_ALT_1,  1, { TERMINAL_CHARACTER } },
  { "<special sequence character alt 2>", { 0, 0, 0,       -1, 0, 0 }, _SPECIAL_SEQUENCE_CHARACTER_ALT_2,  1, { SPECIAL_SEQUENCE_SYMBOL } },

  { "<comment symbol>",              { 0, 0, 0,            -1, 0, 0 }, COMMENT_SYMBOL,                   1, { BRACKETED_TEXTUAL_COMMENT } },
  { "<comment symbol>",              { 0, 0, 0,            -1, 0, 0 }, COMMENT_SYMBOL,                   1, { OTHER_CHARACTER } },
  { "<comment symbol>",              { 0, 0, 0,            -1, 0, 0 }, COMMENT_SYMBOL,                   1, { COMMENTLESS_SYMBOL } },

  { "<comment symbol any>",          { 0, 0, 1,            -1, 0, 0 }, _COMMENT_SYMBOL_ANY,              1, { COMMENT_SYMBOL } },
  { "<bracketed textual comment>",   { 0, 0, 0,            -1, 0, 0 }, BRACKETED_TEXTUAL_COMMENT,        3, { START_COMMENT_SYMBOL, _COMMENT_SYMBOL_ANY, END_COMMENT_SYMBOL } },

  { "<bracketed textual comment any>", { 0, 0, 1,          -1, 0, 0 }, _BRACKETED_TEXTUAL_COMMENT_ANY,   1, { BRACKETED_TEXTUAL_COMMENT } },
  { "<syntax unit>",                 { 0, 0, 0,            -1, 0, 0 }, _SYNTAX_UNIT,                     2, { COMMENTLESS_SYMBOL, _BRACKETED_TEXTUAL_COMMENT_ANY } },
  { "<syntax unit many>",            { 0, 0, 1,            -1, 0, 1 }, _SYNTAX_UNIT_MANY,                1, { _SYNTAX_UNIT } },
  { "<syntax 01>",                   { 0, 0, 0,            -1, 0, 0 }, _SYNTAX_01,                       2, { _BRACKETED_TEXTUAL_COMMENT_ANY, _SYNTAX_UNIT_MANY } },
  { "<syntax>",                      { 0, 0, 0,            -1, 0, 0 }, SYNTAX,                           1, { _SYNTAX_01 } },

  { "<syntax rule many>",            { 0, 0, 1,            -1, 0, 1 }, _SYNTAX_RULE_MANY,                1, { SYNTAX_RULE } },
  { "<syntax 02>",                   { 0, 0, 0,            -1, 0, 0 }, _SYNTAX_02,                       1, { _SYNTAX_RULE_MANY } },
  { "<syntax>",                      { 0, 0, 0,            -1, 0, 0 }, SYNTAX,                           1, { _SYNTAX_02 } },

  { "<syntax rule>",                 { 0, 0, 0,            -1, 0, 0 }, SYNTAX_RULE,                      4, { META_IDENTIFIER, DEFINING_SYMBOL, DEFINITIONS_LIST, TERMINATOR_SYMBOL } },

  { "<definitions sequence>",      { 0, 0, 1, DEFINITION_SEPARATOR_SYMBOL, 0, 1 }, _DEFINITIONS_SEQUENCE, 1, { SINGLE_DEFINITION } },
  { "<definitions list>",            { 0, 0, 0,            -1, 0, 0 }, DEFINITIONS_LIST,                 1, { _DEFINITIONS_SEQUENCE } },
  { "<definitions list>",            { 0, 0, 0,            -1, 0, 0 }, DEFINITIONS_LIST,                 1, { EMPTY_SEQUENCE } },
  { "<single definition>", { 0, 0, 1, CONCATENATE_SYMBOL,          0, 1 }, SINGLE_DEFINITION,            1, { SYNTACTIC_TERM } },
  { "<syntactic term>",              { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_TERM,                   1, { SYNTACTIC_FACTOR } },
  { "<syntactic term>",              { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_TERM,                   3, { SYNTACTIC_FACTOR, EXCEPT_SYMBOL, SYNTACTIC_EXCEPTION } },
  { "<syntactic factor>",            { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_FACTOR,                 3, { INTEGER, REPETITION_SYMBOL, SYNTACTIC_PRIMARY } },
  { "<syntactic factor>",            { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_FACTOR,                 1, { SYNTACTIC_PRIMARY } },
  { "<syntactic primary>",           { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { OPTIONAL_SEQUENCE } },
  { "<syntactic primary>",           { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { REPEATED_SEQUENCE } },
  { "<syntactic primary>",           { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { GROUPED_SEQUENCE } },
  { "<syntactic primary>",           { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { META_IDENTIFIER } },
  { "<syntactic primary>",           { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { TERMINAL_STRING } },
  { "<syntactic primary>",           { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { SPECIAL_SEQUENCE } },
  /*
  { "<syntactic primary>",           { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { EMPTY_SEQUENCE } },
  */
  { "<optional sequence>",           { 0, 0, 0,            -1, 0, 0 }, OPTIONAL_SEQUENCE,                3, { START_OPTION_SYMBOL, DEFINITIONS_LIST, END_OPTION_SYMBOL } },
  { "<repeated sequence>",           { 0, 0, 0,            -1, 0, 0 }, REPEATED_SEQUENCE,                3, { START_REPEAT_SYMBOL, DEFINITIONS_LIST, END_REPEAT_SYMBOL } },
  { "<grouped sequence>",            { 0, 0, 0,            -1, 0, 0 }, GROUPED_SEQUENCE,                 3, { START_GROUP_SYMBOL, DEFINITIONS_LIST, END_GROUP_SYMBOL } },
  { "<empty sequence>",              { 0, 0, 0,            -1, 0, 0 }, EMPTY_SEQUENCE,                   0, { -1 } }, /* Some compilers like cl does not like an empty [] */
};

/* Internally, EBNF is nothing else but an instance of marpaWrapperGrammar_t along */
/* with symbols and rules book-keeping.                                            */
struct marpaEBNF {
  marpaEBNFOption_t      marpaEBNFOption;      /* Option */
  marpaWrapperGrammar_t *grammarp;             /* External grammar */
  marpaWrapperGrammar_t *marpaWrapperGrammarp; /* Internal grammar */
};

static inline short _marpaEBNF_internalGrammarb(marpaEBNF_t *marpaEBNFp);
static inline char *_marpaEBNF_symbolDescription(void *userDatavp, int symboli);

static marpaEBNFOption_t marpaEBNFOptionDefault = {
  NULL /* genericLoggerp */
};

/****************************************************************************/
marpaEBNF_t *marpaEBNF_newp(marpaEBNFOption_t *marpaEBNFOptionp)
/****************************************************************************/
{
  marpaEBNF_t                *marpaEBNFp;
  marpaWrapperGrammarOption_t marpaWrapperGrammarOption;

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
  marpaWrapperGrammarOption.genericLoggerp = marpaEBNFp->marpaEBNFOption.genericLoggerp;
  marpaWrapperGrammarOption.warningIsErrorb = 0;
  marpaWrapperGrammarOption.warningIsIgnoredb = 0;
  marpaEBNFp->marpaWrapperGrammarp = marpaWrapperGrammar_newp(&marpaWrapperGrammarOption);

  if (marpaEBNFp->marpaWrapperGrammarp == NULL) {
    goto err;
  }
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
  size_t                         linel   = 1;   /* Current line number */
  size_t                         columnl = 1;   /* Current column number */
  size_t                         pos = 0;       /* Current position in buffer */
  size_t                         maxpos;
  marpaWrapperRecognizer_t      *marpaWrapperRecognizerp;
  marpaWrapperRecognizerOption_t marpaWrapperRecognizerOption;
  size_t                         eventl;
  marpaWrapperGrammarEvent_t    *eventp;
  size_t                         i;
  size_t                         nSymboll;
  int                           *symbolArrayp;
  int                            symboli;
  marpaEBNFSymbol_t              marpaEBNFSymbol;
  char                           c;
  int                            nAlternativei;
  GENERICSTACK_DECL(inputStack);

  GENERICSTACK_NEW(inputStack);

  if ((marpaEBNFp == NULL) || (grammars == NULL)) {
    errno = EINVAL;
    goto err;
  }

  /* An ISO EBNF grammar is always expressed in the 7-bit ASCII range; that is entirely */
  /* covered by the basic C default character set. We just have to take care to always  */
  /* compare with a char, not an integer value.                                         */
  maxpos = strlen(grammars) - 1;

  /* Parse external grammar using internal grammar, supposed to fit entirely in memory */
  marpaWrapperRecognizerOption.genericLoggerp = marpaEBNFp->marpaEBNFOption.genericLoggerp;
  marpaWrapperRecognizerOption.latmb          = 1;

  marpaWrapperRecognizerp = marpaWrapperRecognizer_newp(marpaEBNFp->marpaWrapperGrammarp, &marpaWrapperRecognizerOption);
  if (marpaWrapperRecognizerp == NULL) {
    goto err;
  }
  /* Loop until it is consumed or an error */
  while (pos <= maxpos) {
    /* Events */
    if (marpaWrapperGrammar_eventb(marpaEBNFp->marpaWrapperGrammarp, &eventl, &eventp, 0) == 0) {
      goto err;
    }
    if (eventl > 0) {
      for (i = 0; i < eventl; i++) {
	switch (eventp[i].eventType) {
	case MARPAWRAPPERGRAMMAR_EVENT_COMPLETED:
	  break;
	case MARPAWRAPPERGRAMMAR_EVENT_NULLED:
	  break;
	case MARPAWRAPPERGRAMMAR_EVENT_EXPECTED:
	  break;
	default:
	  /* Should never happen and is anyway a no-op */
	  break;
	}
      }
    }
    /* Log current state */
    marpaWrapperRecognizer_progressLogb(marpaWrapperRecognizerp, -1, 1, GENERICLOGGER_LOGLEVEL_TRACE, marpaEBNFp, _marpaEBNF_symbolDescription);
    /* Lookup only expected terminals */
    if (marpaWrapperRecognizer_expectedb(marpaWrapperRecognizerp, &nSymboll, &symbolArrayp) == 0) {
      goto err;
    }
    if (nSymboll > 0) {
      short okb;
      int lengthi;

      nAlternativei = 0;
      for (i = 0; i < nSymboll; i++) {
        symboli = symbolArrayp[i];
	if (marpaEBNFp->marpaEBNFOption.genericLoggerp != NULL) {
	  GENERICLOGGER_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "Expected terminal: %s", _marpaEBNF_symbolDescription(marpaEBNFp, symboli));
	}

        marpaEBNFSymbol = marpaEBNFSymbolArray[symboli];
        okb = 0;
        lengthi = 1;
        c = grammars[pos];
        /* Process only terminals that we know about */
        switch (symboli) {
        case LETTER:
          if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) { okb = 1; }
          break;
        case DECIMAL_DIGIT:
          if (c >= '0' && c <= '9') { okb = 1; }
          break;
        case CONCATENATE_SYMBOL:
          if (c == ',') { okb = 1; }
          break;
        case DEFINING_SYMBOL:
          if (c == ',') { okb = 1; }
          break;
        case DEFINITION_SEPARATOR_SYMBOL:
          if (c == '|' || c == '/' || c == '!') { okb = 1; }
          break;
        case END_COMMENT_SYMBOL:
          if (c == '*' && pos < maxpos && grammars[pos+1] == ')') { okb = 1; }
          break;
        case END_GROUP_SYMBOL:
          if (c == ')') { okb = 1; }
          break;
        case END_OPTION_SYMBOL:
          if (c == ']' || (pos < maxpos && c == '/' && grammars[pos+1] == ')')) { okb = 1; lengthi = 2; }
          break;
        case END_REPEAT_SYMBOL:
          if (c == '}' || (pos < maxpos && c == ':' && grammars[pos+1] == ')')) { okb = 1; lengthi = 2;}
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
          if (c == '(' && pos < maxpos && grammars[pos+1] == '*') { okb = 1; lengthi = 2; }
          break;
        default:
          break;
        }
        if (okb) {
          GENERICSTACK_PUSH_CHAR(inputStack, c);
          if (marpaWrapperRecognizer_alternativeb(marpaWrapperRecognizerp, symboli, GENERICSTACK_SIZE(inputStack), 1) == 0) {
            goto err;
          }
          ++nAlternativei;
        }
      }
      if (nAlternativei > 0) {
        marpaWrapperRecognizer_completeb(marpaWrapperRecognizerp);
      } else {
        errno = EINVAL;
        goto err;
      }
    }

    /* Resume */   
    break;

  }

  marpaWrapperRecognizer_freev(marpaWrapperRecognizerp);
  return 1; 

 err:
  if (marpaWrapperRecognizerp != NULL) {
    marpaWrapperRecognizer_freev(marpaWrapperRecognizerp);
  }
  return 0;
}

/****************************************************************************/
void marpaEBNF_freev(marpaEBNF_t *marpaEBNFp)
/****************************************************************************/
{
  if (marpaEBNFp != NULL) {
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
  for (i = 0; i < MARPAEBNF_SIZEOF_ARRAY(marpaEBNFSymbolArray); i++) {
    symboli = marpaWrapperGrammar_newSymboli(marpaEBNFp->marpaWrapperGrammarp,
					     &(marpaEBNFSymbolArray[i].option));
    /* We take advantage of the fact that symbols always start at 0 with marpa */
    if (symboli != marpaEBNFSymbolArray[i].symboli) {
      goto err;
    }
  }

  /* Declare all the rules */
  for (i = 0; i < MARPAEBNF_SIZEOF_ARRAY(marpaEBNFRuleArray); i++) {
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
