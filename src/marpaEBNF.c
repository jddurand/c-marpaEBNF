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
  _SYNTAX_02
} marpaEBNFSymbol_e;

typedef struct marpaEBNFSymbol {
  marpaEBNFSymbol_e                 symboli;
  char                             *descriptions;
  marpaWrapperGrammarSymbolOption_t option;
} marpaEBNFSymbol_t;

typedef struct marpaEBNFRule {
  char                             *descriptions;
  marpaWrapperGrammarRuleOption_t   option;
  int                               lhsSymboli;
  size_t                            rhsSymboll;
  int                               rhsSymbolip[20]; /* Flexible array and ANSI-C are not friends */
} marpaEBNFRule_t;

/* List of all symbols of the EBNF grammar as per ISO/IEC 14977:1996 */
static marpaEBNFSymbol_t marpaEBNFSymbolArray[] = {
  /* ------------------------------------------------------------------------------------------
    symboli                          descriptions                      { terminalb, startb,                          eventSeti }
    -------------------------------------------------------------------------------------------
  */
  /*
   *                                TERMINAL CHARACTERS
   */
  {LETTER,                          "<letter>",                          {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {DECIMAL_DIGIT,                   "<decimal digit>",                   {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {CONCATENATE_SYMBOL,              "<concatenate symbol>",              {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {DEFINING_SYMBOL,                 "<defining symbol>",                 {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {DEFINITION_SEPARATOR_SYMBOL,     "<definition separator symbol>",     {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {END_COMMENT_SYMBOL,              "<end comment symbol>",              {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {END_GROUP_SYMBOL,                "<end group symbol>",                {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {END_OPTION_SYMBOL,               "<end option symbol>",               {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {END_REPEAT_SYMBOL,               "<end repeat symbol>",               {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {EXCEPT_SYMBOL,                   "<except symbol>",                   {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {FIRST_QUOTE_SYMBOL,              "<first quote symbol>",              {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {REPETITION_SYMBOL,               "<repetition symbol>",               {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SECOND_QUOTE_SYMBOL,             "<second quote symbol>",             {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SPECIAL_SEQUENCE_SYMBOL,         "<special sequence symbol>",         {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {START_COMMENT_SYMBOL,            "<start comment symbol>",            {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {START_GROUP_SYMBOL,              "<start group symbol>",              {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {START_OPTION_SYMBOL,             "<start option symbol>",             {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {START_REPEAT_SYMBOL,             "<start repeat symbol>",             {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {TERMINATOR_SYMBOL,               "<terminator symbol>",               {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {OTHER_CHARACTER,                 "<other character>",                 {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SPACE_CHARACTER,                 "<space character>",                 {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {HORIZONTAL_TABULATION_CHARACTER, "<horizontal tabulation character>", {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {NEW_LINE,                        "<new line>",                        {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {VERTICAL_TABULATION_CHARACTER,   "<vertical tabulation character>",   {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {FORM_FEED,                       "<form feed>",                       {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   *                              NON-TERMINAL CHARACTERS
   */
  {TERMINAL_CHARACTER,              "<terminal character>",              {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {GAP_FREE_SYMBOL,                 "<gap free symbol>",                 {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {TERMINAL_STRING,                 "<terminal string>",                 {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {FIRST_TERMINAL_CHARACTER,        "<first terminal character>",        {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SECOND_TERMINAL_CHARACTER,       "<second terminal character>",       {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {GAP_SEPARATOR,                   "<gap separator>",                   {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTAX,                          "<syntax>",                          {         0,      1, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },  /* START */
  {COMMENTLESS_SYMBOL,             "<commentless symbol>",             {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {INTEGER,                         "<integer>",                         {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {META_IDENTIFIER,                 "<meta identifier>",                 {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {META_IDENTIFIER_CHARACTER,       "<meta identifier character>",       {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SPECIAL_SEQUENCE,                "<special sequence>",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SPECIAL_SEQUENCE_CHARACTER,      "<special sequence character>",      {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {COMMENT_SYMBOL,                  "<comment symbol>",                  {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {BRACKETED_TEXTUAL_COMMENT,       "<bracketed textual comment>",       {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTAX_RULE,                     "<syntax rule>",                     {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {DEFINITIONS_LIST,                "<definitions list>",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SINGLE_DEFINITION,               "<single definition>",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTACTIC_TERM,                  "<syntactic term>",                  {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTACTIC_EXCEPTION,             "<syntactic exception>",             {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTACTIC_FACTOR,                "<syntactic factor>",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {SYNTACTIC_PRIMARY,               "<syntactic primary>",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {OPTIONAL_SEQUENCE,               "<optional sequence>",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {REPEATED_SEQUENCE,               "<repeated sequence>",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {GROUPED_SEQUENCE,                "<grouped sequence>",                {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {EMPTY_SEQUENCE,                  "<empty sequence>",                  {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {COMMENT,                         "<comment>",                         {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
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
  {_GAP_FREE_SYMBOL_ALT_1,          "<gap free symbol alt 1>",           {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_GAP_FREE_SYMBOL_ALT_1_1,        "<gap free symbol alt 1.1>",         {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } },
  {_GAP_FREE_SYMBOL_ALT_1_2,        "<gap free symbol alt 1.2>",         {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } },
  {_GAP_FREE_SYMBOL_ALT_1_1_MARKER, "<gap free symbol alt 1.1 marker>",  {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_GAP_FREE_SYMBOL_ALT_1_2_MARKER, "<gap free symbol alt 1.2 marker>",  {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
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
  {_FIRST_TERMINAL_CHARACTER_MANY,  "<first terminal character>+",     {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SECOND_TERMINAL_CHARACTER_MANY, "<second terminal character>+",    {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
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
  {_FIRST_TERMINAL_CHARACTER_ALT_1,        "<first terminal character alt 1>",        {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } },
  {_FIRST_TERMINAL_CHARACTER_ALT_1_MARKER, "<first terminal character alt 1 marker>", {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_FIRST_TERMINAL_CHARACTER_ALT_2,        "<first terminal character alt 2>",        {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } },
  {_FIRST_TERMINAL_CHARACTER_ALT_2_MARKER, "<first terminal character alt 2 marker>", {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
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
  {_SECOND_TERMINAL_CHARACTER_ALT_1,        "<second terminal character alt 1>",        {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } },
  {_SECOND_TERMINAL_CHARACTER_ALT_1_MARKER, "<second terminal character alt 1 marker>", {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SECOND_TERMINAL_CHARACTER_ALT_2,        "<second terminal character alt 2>",        {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } },
  {_SECOND_TERMINAL_CHARACTER_ALT_2_MARKER, "<second terminal character alt 2 marker>", {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * syntax = {gap separator}, gap free symbol, {gap separator}, {gap free symbol, {gap separator}}
   *
   * is revisited to:
   *
   * <gap separator any> = <gap separator>*
   * <gap free symbol many> = <gap free symbol>+ separator => <gap separator> proper => 0
   * <syntax> = <gap separator any> <gap free symbol many>
   */
  {_GAP_SEPARATOR_ANY,              "<gap separator>*",               {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_GAP_FREE_SYMBOL_MANY,           "<gap free symbol>+",             {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
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
  {_COMMENTLESS_SYMBOL_ALT_1,          "<commentless symbol alt 1>",           {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_COMMENTLESS_SYMBOL_ALT_1_1,        "<commentless symbol alt 1.1>",         {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } },
  {_COMMENTLESS_SYMBOL_ALT_1_2,        "<commentless symbol alt 1.2>",         {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } },
  {_COMMENTLESS_SYMBOL_ALT_1_1_MARKER, "<commentless symbol alt 1.1 marker>",  {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_COMMENTLESS_SYMBOL_ALT_1_2_MARKER, "<commentless symbol alt 1.2 marker>",  {         1,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * integer = decimal digit, {decimal digit}
   *
   * is revisited to:
   *
   * <integer> = <decimal digit many>
   * <decimal digit many> = <decimal digit>+
   */
  {_DECIMAL_DIGIT_MANY,             "<decimal digit>+",                          {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * meta identifier = letter, {meta identifier character}
   *
   * is revisited to:
   *
   * <meta identifier> = <letter> <meta identifier character any>
   * <meta identifier character any> = <meta identifier character>*
   */
  {_META_IDENTIFIER_CHARACTER_ANY,   "<meta identifier character>*",             {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * special sequence = special sequence symbol, {special sequence character}, special sequence symbol
   *
   * is revisited to:
   *
   * <special sequence character any> = <special sequence character>*
   * <special sequence> = <special sequence symbol> <special sequence character any> <special sequence symbol>
   */
  {_SPECIAL_SEQUENCE_CHARACTER_ANY,   "<special sequence character>*",           {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
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
  {_SPECIAL_SEQUENCE_CHARACTER_ALT_1,        "<special sequence character alt 1>",        {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } },
  {_SPECIAL_SEQUENCE_CHARACTER_ALT_1_MARKER, "<special sequence character alt 1 marker>", {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SPECIAL_SEQUENCE_CHARACTER_ALT_2,        "<special sequence character alt 2>",        {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION } },
  {_SPECIAL_SEQUENCE_CHARACTER_ALT_2_MARKER, "<special sequence character alt 2 marker>", {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * bracketed textual comment = start comment symbol, {comment symbol}, end comment symbol
   *
   * is revisited to:
   *
   * <comment symbol any> = <comment symbol>*
   * <bracketed textual comment> = <start comment symbol> <comment symbol any> <end comment symbol>
   */
  {_COMMENT_SYMBOL_ANY,   "<comment symbol>*",           {         0,      0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
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
  {_BRACKETED_TEXTUAL_COMMENT_ANY, "<bracketed textual comment>*", { 0, 0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SYNTAX_UNIT,                   "<syntax unit>",                { 0, 0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SYNTAX_UNIT_MANY,              "<syntax unit>+",               { 0, 0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SYNTAX_01,                     "<syntax 01>",                  { 0, 0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  /*
   * syntax = syntax rule, {syntax rule}
   *
   * is revisited to:
   *
   * <syntax rule many>> = <syntax rule>+
   * <syntax 02> = <syntax rule many>
   * <syntax> = <syntax 01>
   */
  {_SYNTAX_RULE_MANY,              "<syntax rule>+",               { 0, 0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
  {_SYNTAX_02,                     "<syntax 02>",                  { 0, 0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE } },
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

  { "<definitions list>",            { 0, 0, 1,  DEFINITION_SEPARATOR_SYMBOL, 0, 1 }, DEFINITIONS_LIST,  1, { SINGLE_DEFINITION } },
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

  /* Delcare all the rules */
  for (i = 0; i < MARPAEBNF_SIZEOF_ARRAY(marpaEBNFRuleArray); i++) {
    rulei = marpaWrapperGrammar_newRulei(marpaEBNFp->marpaWrapperGrammarp,
					 &(marpaEBNFRuleArray[i].option),
					 marpaEBNFRuleArray[i].lhsSymboli,
					 marpaEBNFRuleArray[i].rhsSymboll,
					 marpaEBNFRuleArray[i].rhsSymbolip);
    /* We take advantage of the fact that symbols always start at 0 with marpa */
    if (rulei < 0) {
      goto err;
    }
  }
  
  return 1;

 err:
  return 0;
}
