#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "config.h"
#include <genericStack.h>
#include <marpaWrapper.h>
#include "marpaEBNF.h"
#include "marpaEBNF/internal/logging.h"

#define MARPAEBNF_LENGTH_ARRAY(x) (sizeof(x)/sizeof((x)[0]))

/* Convenience enum, also used to make sure that symbol id is the enum -; */
typedef enum marpaEBNFSymbolEnum {
  _START,          /* Just because the default is to set start symbol to the first symbol */

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

  _CARRIAGE_RETURN,
  _CARRIAGE_RETURN_ANY,
  _LINE_FEED,

  _START_REPEAT_SYMBOL_0,
  _START_REPEAT_SYMBOL_1,

  _END_REPEAT_SYMBOL_0,
  _END_REPEAT_SYMBOL_1,

  _START_OPTION_SYMBOL_0,
  _START_OPTION_SYMBOL_1,

  _END_OPTION_SYMBOL_0,
  _END_OPTION_SYMBOL_1,

  _FIRST_TERMINAL_CHARACTER_MANY,
  _SECOND_TERMINAL_CHARACTER_MANY,

  _GAP_SEPARATOR_ANY,
  _GAP_SYMBOL_UNIT,
  _GAP_SYMBOL_UNIT_MANY,

  _COMMENTLESS_SYMBOL_TERMINAL_CHARACTER,

  _META_IDENTIFIER_CHARACTER_ANY,

  _SPECIAL_SEQUENCE_CHARACTER_ANY,

  _COMMENT_SYMBOL_ANY,

  _BRACKETED_TEXTUAL_COMMENT_ANY,
  _COMMENTLESS_SYMBOL_UNIT,
  _COMMENTLESS_SYMBOL_UNIT_MANY,

  _DEFINITIONS_SEQUENCE,
} marpaEBNFSymbolEnum_t;

typedef struct marpaEBNFSymbol {
  marpaEBNFSymbolEnum_t symboli;
  char                 *descriptions;
} marpaEBNFSymbol_t;

typedef struct marpaEBNFRule {
  marpaWrapperGrammarRuleOption_t   option;
  int                               lhsSymboli;
  size_t                            rhsSymboll;
  int                               rhsSymbolip[20]; /* Flexible array and ANSI-C are not friends */
  size_t                            rhsExceptl;
  int                               rhsExceptp[20]; /* Ditto */
} marpaEBNFRule_t;

/* List of all symbols of the EBNF grammar as per ISO/IEC 14977:1996 */
static marpaEBNFSymbol_t marpaEBNFSymbolArray[] = {
  /* -------------------------------------------
  {symboli                        , descriptions
  ---------------------------------------------- */
  {_START                         , "[:start:]" },
  {LETTER                         , "<letter>" },
  {DECIMAL_DIGIT                  , "<decimal digit>" },
  {CONCATENATE_SYMBOL             , "<concatenate symbol>" },
  {DEFINING_SYMBOL                , "<defining symbol>" },
  {DEFINITION_SEPARATOR_SYMBOL    , "<definition separator symbol>" },
  {END_COMMENT_SYMBOL             , "<end comment symbol>" },
  {END_GROUP_SYMBOL               , "<end group symbol>" },
  {END_OPTION_SYMBOL              , "<end option symbol>" },
  {END_REPEAT_SYMBOL              , "<end repeat symbol>" },
  {EXCEPT_SYMBOL                  , "<except symbol>" },
  {FIRST_QUOTE_SYMBOL             , "<first quote symbol>" },
  {REPETITION_SYMBOL              , "<repetition symbol>" },
  {SECOND_QUOTE_SYMBOL            , "<second quote symbol>" },
  {SPECIAL_SEQUENCE_SYMBOL        , "<special sequence symbol>" },
  {START_COMMENT_SYMBOL           , "<start comment symbol>" },
  {START_GROUP_SYMBOL             , "<start group symbol>" },
  {START_OPTION_SYMBOL            , "<start option symbol>" },
  {START_REPEAT_SYMBOL            , "<start repeat symbol>" },
  {TERMINATOR_SYMBOL              , "<terminator symbol>" },
  {OTHER_CHARACTER                , "<other character>" },
  {SPACE_CHARACTER                , "<space character>" },
  {HORIZONTAL_TABULATION_CHARACTER, "<horizontal tabulation character>" },
  {NEW_LINE                       , "<new line>" },
  {VERTICAL_TABULATION_CHARACTER  , "<vertical tabulation character>" },
  {FORM_FEED                      , "<form feed>" },
  {TERMINAL_CHARACTER             , "<terminal character>" },
  {GAP_FREE_SYMBOL                , "<gap free symbol>" },
  {TERMINAL_STRING                , "<terminal string>" },
  {FIRST_TERMINAL_CHARACTER       , "<first terminal character>" },
  {SECOND_TERMINAL_CHARACTER      , "<second terminal character>" },
  {GAP_SEPARATOR                  , "<gap separator>" },
  {SYNTAX                         , "<syntax>" },
  {COMMENTLESS_SYMBOL             , "<commentless symbol>" },
  {INTEGER                        , "<integer>" },
  {META_IDENTIFIER                , "<meta identifier>" },
  {META_IDENTIFIER_CHARACTER      , "<meta identifier character>" },
  {SPECIAL_SEQUENCE               , "<special sequence>" },
  {SPECIAL_SEQUENCE_CHARACTER     , "<special sequence character>" },
  {COMMENT_SYMBOL                 , "<comment symbol>" },
  {BRACKETED_TEXTUAL_COMMENT      , "<bracketed textual comment>" },
  {SYNTAX_RULE                    , "<syntax rule>" },
  {DEFINITIONS_LIST               , "<definitions list>" },
  {SINGLE_DEFINITION              , "<single definition>" },
  {SYNTACTIC_TERM                 , "<syntactic term>" },
  {SYNTACTIC_EXCEPTION            , "<syntactic exception>" },
  {SYNTACTIC_FACTOR               , "<syntactic factor>" },
  {SYNTACTIC_PRIMARY              , "<syntactic primary>" },
  {OPTIONAL_SEQUENCE              , "<optional sequence>" },
  {REPEATED_SEQUENCE              , "<repeated sequence>" },
  {GROUPED_SEQUENCE               , "<grouped sequence>" },
  {EMPTY_SEQUENCE                 , "<empty sequence>" },
  {COMMENT                        , "<comment>" },
  /*
    some symbols are explicitely splitted into their two letters
  */
  {_CARRIAGE_RETURN              , "<carriage return>" },
  {_CARRIAGE_RETURN_ANY          , "<carriage return any*>" },
  {_LINE_FEED                    , "<line feed>" },
  {_START_REPEAT_SYMBOL_0         , "<start repeat symbol 0>" },
  {_START_REPEAT_SYMBOL_1         , "<start repeat symbol 1>" },
  {_END_REPEAT_SYMBOL_0           , "<end repeat symbol 0>" },
  {_END_REPEAT_SYMBOL_1           , "<end repeat symbol 1>" },
  {_START_OPTION_SYMBOL_0         , "<start option symbol 0>" },
  {_START_OPTION_SYMBOL_1         , "<start option symbol 1>" },
  {_END_OPTION_SYMBOL_0           , "<end option symbol 0>" },
  {_END_OPTION_SYMBOL_1           , "<end option symbol 1>" },

  /*
   * ------------------------------------------
   * Note: handling of the exception with Marpa
   * ------------------------------------------
   *
   * Given a rule X = A - B, we rewrite it as X = A, and remember that B is not allowed when traversing the ASF.
   * B can be composite and must contain only RHS's of X.
   *
   * ------------------------------------------
   * Note: ' ' character
   * ------------------------------------------
   *
   * It can happen in two cases: <space character> and <other character>. Marpa is handling this ambiguity.
   *
  /* ---------------- */
  /* Internal symbols */
  /* ---------------- */
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
  /* -------------------------------------------
  {symboli                        , descriptions
  ---------------------------------------------- */
  {_FIRST_TERMINAL_CHARACTER_MANY,  "<first terminal character many>" },
  {_SECOND_TERMINAL_CHARACTER_MANY, "<second terminal character many>" },
  /*
   * syntax = {gap separator}, gap free symbol, {gap separator}, {gap free symbol, {gap separator}}
   *
   * is revisited to:
   *
   * <gap separator any> = <gap separator>*
   * <gap symbol unit> = <gap free symbol> <gap separator any>
   * <gap symbol unit many> = <gap free symbol unit>+
   * <syntax> = <gap separator any> <gap symbol unit many>
   *
   */
  /* ----------------------------------------------
  {symboli                  , descriptions
  -------------------------------------------------- */
  {_GAP_SEPARATOR_ANY       , "<gap separator any>" },
  {_GAP_SYMBOL_UNIT         , "<gap symbol unit>" },
  {_GAP_SYMBOL_UNIT_MANY    , "<gap symbol unit many>" },
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
   * commentless symbol terminal character = terminal character
   * commentless symbol = commentless symbol terminal character
   *                     | meta identifier
   *                     | integer
   *                     | terminal string
   *                     | special sequence
   *
   */
  {_COMMENTLESS_SYMBOL_TERMINAL_CHARACTER , "<commentless symbol terminal character>" },
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
  /* ------------------------------------------
  {symboli                       , descriptions
  --------------------------------------------- */
  {_META_IDENTIFIER_CHARACTER_ANY, "<meta identifier character any>" },
  /*
   * special sequence = special sequence symbol, {special sequence character}, special sequence symbol
   *
   * is revisited to:
   *
   * <special sequence character any> = <special sequence character>*
   * <special sequence> = <special sequence symbol> <special sequence character any> <special sequence symbol>
   */
  /* -------------------------------------------
  {symboli                        , descriptions
  ---------------------------------------------- */
  {_SPECIAL_SEQUENCE_CHARACTER_ANY, "<special sequence character any>" },
  /*
   * special sequence character = terminal character - special sequence symbol
   *
   * is revisited to:
   *
   * special sequence character = terminal character
   *
   * and we will remember in the rules that it is not possible if <terminal characer> matches <special sequence symbol>
   */
  /*
   * bracketed textual comment = start comment symbol, {comment symbol}, end comment symbol
   *
   * is revisited to:
   *
   * <comment symbol any> = <comment symbol>*
   * <bracketed textual comment> = <start comment symbol> <comment symbol any> <end comment symbol>
   */
  /* -------------------------------
  {symboli            , descriptions
  ---------------------------------- */
  {_COMMENT_SYMBOL_ANY, "<comment symbol any>" },
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
  /* ------------------------------------------
  {symboli                       , descriptions
  --------------------------------------------- */
  {_BRACKETED_TEXTUAL_COMMENT_ANY, "<bracketed textual comment any>" },
  {_COMMENTLESS_SYMBOL_UNIT      , "<commentless symbol unit>" },
  {_COMMENTLESS_SYMBOL_UNIT_MANY,  "<commentless symbol unit many>" },
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
  /* ---------------------------------
  {symboli              , descriptions
  ------------------------------------ */
  {_DEFINITIONS_SEQUENCE, "<definitions sequence>" },
};

static marpaEBNFRule_t marpaEBNFRuleArray[] = {
  { { 0, 0, 0,            -1, 0, 0 },             _START,               1, { SYNTAX }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { DECIMAL_DIGIT }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { CONCATENATE_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { DEFINING_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { DEFINITION_SEPARATOR_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { END_COMMENT_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { END_GROUP_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { END_OPTION_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { END_REPEAT_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { EXCEPT_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { FIRST_QUOTE_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { REPETITION_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { SECOND_QUOTE_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { SPECIAL_SEQUENCE_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { START_COMMENT_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { START_GROUP_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { START_OPTION_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { START_REPEAT_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { TERMINATOR_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_CHARACTER,               1, { OTHER_CHARACTER }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, GAP_FREE_SYMBOL,                  1, { TERMINAL_CHARACTER }, 2, {  FIRST_QUOTE_SYMBOL, SECOND_QUOTE_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, GAP_FREE_SYMBOL,                  1, { TERMINAL_STRING }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_STRING,                  3, { FIRST_QUOTE_SYMBOL, _FIRST_TERMINAL_CHARACTER_MANY, FIRST_QUOTE_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, TERMINAL_STRING,                  3, { SECOND_QUOTE_SYMBOL, _SECOND_TERMINAL_CHARACTER_MANY, SECOND_QUOTE_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, NEW_LINE,                         3, { _CARRIAGE_RETURN_ANY, _LINE_FEED, _CARRIAGE_RETURN_ANY }, 0, { -1 } },
  { { 0, 0, 1,            -1, 0, 0 }, _CARRIAGE_RETURN_ANY,             1, { _CARRIAGE_RETURN }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, START_REPEAT_SYMBOL,              1, { _START_REPEAT_SYMBOL_0 }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, START_REPEAT_SYMBOL,              1, { _START_REPEAT_SYMBOL_1 }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, END_REPEAT_SYMBOL,                1, { _END_REPEAT_SYMBOL_0 }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, END_REPEAT_SYMBOL,                1, { _END_REPEAT_SYMBOL_1 }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, START_OPTION_SYMBOL,              1, { _START_OPTION_SYMBOL_0 }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, START_OPTION_SYMBOL,              1, { _START_OPTION_SYMBOL_1 }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, END_OPTION_SYMBOL,                1, { _END_OPTION_SYMBOL_0 }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, END_OPTION_SYMBOL,                1, { _END_OPTION_SYMBOL_1 }, 0, { -1 } },
  { { 0, 0, 1,            -1, 0, 1 }, _FIRST_TERMINAL_CHARACTER_MANY,   1, { FIRST_TERMINAL_CHARACTER }, 0, { -1 } },
  { { 0, 0, 1,            -1, 0, 1 }, _SECOND_TERMINAL_CHARACTER_MANY,  1, { SECOND_TERMINAL_CHARACTER }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, FIRST_TERMINAL_CHARACTER,         1, { TERMINAL_CHARACTER }, 1, { FIRST_QUOTE_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, SECOND_TERMINAL_CHARACTER,        1, { TERMINAL_CHARACTER }, 1, { SECOND_QUOTE_SYMBOL } },
  { { 0, 0, 0,            -1, 0, 0 }, GAP_SEPARATOR,                    1, { SPACE_CHARACTER }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, GAP_SEPARATOR,                    1, { HORIZONTAL_TABULATION_CHARACTER }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, GAP_SEPARATOR,                    1, { NEW_LINE }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, GAP_SEPARATOR,                    1, { VERTICAL_TABULATION_CHARACTER }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, GAP_SEPARATOR,                    1, { FORM_FEED }, 0, { -1 } },
  { { 0, 0, 1,            -1, 0, 0 }, _GAP_SEPARATOR_ANY,               1, { GAP_SEPARATOR }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, _GAP_SYMBOL_UNIT,                 2, { GAP_FREE_SYMBOL, _GAP_SEPARATOR_ANY }, 0, { -1 } },
  { { 0, 0, 1,            -1, 0, 1 }, _GAP_SYMBOL_UNIT_MANY,            1, { _GAP_SYMBOL_UNIT }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTAX,                           2, { _GAP_SEPARATOR_ANY, _GAP_SYMBOL_UNIT_MANY }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_TERMINAL_CHARACTER, 1, { TERMINAL_CHARACTER }, 8, { LETTER, DECIMAL_DIGIT, FIRST_QUOTE_SYMBOL, SECOND_QUOTE_SYMBOL, START_COMMENT_SYMBOL, END_COMMENT_SYMBOL, SPECIAL_SEQUENCE_SYMBOL, OTHER_CHARACTER } },
  { { 0, 0, 0,            -1, 0, 0 }, COMMENTLESS_SYMBOL,               1, { _COMMENTLESS_SYMBOL_TERMINAL_CHARACTER }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, COMMENTLESS_SYMBOL,               1, { META_IDENTIFIER }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, COMMENTLESS_SYMBOL,               1, { INTEGER }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, COMMENTLESS_SYMBOL,               1, { TERMINAL_STRING }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, COMMENTLESS_SYMBOL,               1, { SPECIAL_SEQUENCE }, 0, { -1 } },
  { { 0, 0, 1,            -1, 0, 1 }, INTEGER,                          1, { DECIMAL_DIGIT }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, META_IDENTIFIER,                  2, { LETTER, _META_IDENTIFIER_CHARACTER_ANY }, 0, { -1 } },
  { { 0, 0, 1,            -1, 0, 0 }, _META_IDENTIFIER_CHARACTER_ANY,   1, { META_IDENTIFIER_CHARACTER }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, META_IDENTIFIER_CHARACTER,        1, { LETTER }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, META_IDENTIFIER_CHARACTER,        1, { DECIMAL_DIGIT }, 0, { -1 } },
  { { 0, 0, 1,            -1, 0, 0 }, _SPECIAL_SEQUENCE_CHARACTER_ANY,  1, { SPECIAL_SEQUENCE_CHARACTER }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, SPECIAL_SEQUENCE,                 3, { SPECIAL_SEQUENCE_SYMBOL, _SPECIAL_SEQUENCE_CHARACTER_ANY, SPECIAL_SEQUENCE_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, SPECIAL_SEQUENCE_CHARACTER,       1, { TERMINAL_CHARACTER }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, COMMENT_SYMBOL,                   1, { BRACKETED_TEXTUAL_COMMENT }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, COMMENT_SYMBOL,                   1, { OTHER_CHARACTER }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, COMMENT_SYMBOL,                   1, { COMMENTLESS_SYMBOL }, 0, { -1 } },
  { { 0, 0, 1,            -1, 0, 0 }, _COMMENT_SYMBOL_ANY,              1, { COMMENT_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, BRACKETED_TEXTUAL_COMMENT,        3, { START_COMMENT_SYMBOL, _COMMENT_SYMBOL_ANY, END_COMMENT_SYMBOL }, 0, { -1 } },
  { { 0, 0, 1,            -1, 0, 0 }, _BRACKETED_TEXTUAL_COMMENT_ANY,   1, { BRACKETED_TEXTUAL_COMMENT }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, _COMMENTLESS_SYMBOL_UNIT,         2, { COMMENTLESS_SYMBOL, _BRACKETED_TEXTUAL_COMMENT_ANY }, 0, { -1 } },
  { { 0, 0, 1,            -1, 0, 1 }, _COMMENTLESS_SYMBOL_UNIT_MANY,    1, { _COMMENTLESS_SYMBOL_UNIT }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTAX,                           2, { _BRACKETED_TEXTUAL_COMMENT_ANY, _COMMENTLESS_SYMBOL_UNIT_MANY }, 0, { -1 } },
  { { 1, 0, 0,            -1, 0, 0 }, SYNTAX,                           2, { SYNTAX_RULE, SYNTAX }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTAX,                           1, { SYNTAX_RULE }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTAX_RULE,                      4, { META_IDENTIFIER, DEFINING_SYMBOL, DEFINITIONS_LIST, TERMINATOR_SYMBOL }, 0, { -1 } },
  { { 0, 0, 1, DEFINITION_SEPARATOR_SYMBOL, 0, 1 }, _DEFINITIONS_SEQUENCE, 1, { SINGLE_DEFINITION }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, DEFINITIONS_LIST,                 1, { _DEFINITIONS_SEQUENCE }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, DEFINITIONS_LIST,                 1, { EMPTY_SEQUENCE }, 0, { -1 } },
  { { 0, 0, 1, CONCATENATE_SYMBOL,          0, 1 }, SINGLE_DEFINITION,            1, { SYNTACTIC_TERM }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_TERM,                   1, { SYNTACTIC_FACTOR }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_TERM,                   3, { SYNTACTIC_FACTOR, EXCEPT_SYMBOL, SYNTACTIC_EXCEPTION }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_FACTOR,                 3, { INTEGER, REPETITION_SYMBOL, SYNTACTIC_PRIMARY }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_FACTOR,                 1, { SYNTACTIC_PRIMARY }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { OPTIONAL_SEQUENCE }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { REPEATED_SEQUENCE }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { GROUPED_SEQUENCE }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { META_IDENTIFIER }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { TERMINAL_STRING }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { SPECIAL_SEQUENCE }, 0, { -1 } },
  /*
  { { 0, 0, 0,            -1, 0, 0 }, SYNTACTIC_PRIMARY,                1, { EMPTY_SEQUENCE }, 0, { -1 } },
  */
  { { 0, 0, 0,            -1, 0, 0 }, OPTIONAL_SEQUENCE,                3, { START_OPTION_SYMBOL, DEFINITIONS_LIST, END_OPTION_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, REPEATED_SEQUENCE,                3, { START_REPEAT_SYMBOL, DEFINITIONS_LIST, END_REPEAT_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, GROUPED_SEQUENCE,                 3, { START_GROUP_SYMBOL, DEFINITIONS_LIST, END_GROUP_SYMBOL }, 0, { -1 } },
  { { 0, 0, 0,            -1, 0, 0 }, EMPTY_SEQUENCE,                   0, { -1 } } /* Some compilers like cl does not like an empty [] */
};

/* Internally, EBNF is nothing else but an instance of marpaWrapperGrammar_t along */
/* with symbols and rules book-keeping.                                            */
struct marpaEBNF {
  marpaEBNFOption_t      marpaEBNFOption;      /* Option */
  marpaWrapperGrammar_t *grammarp;             /* External grammar */
  marpaWrapperGrammar_t *marpaWrapperGrammarp; /* Internal grammar */
  marpaEBNFSymbol_t     *symbolArrayp;         /* Copy of marpaEBNFSymbolArray */
  marpaEBNFRule_t       *ruleArrayp;           /* Copy of marpaEBNFRuleArray */
  genericStack_t        *inputStackp;
  genericStack_t        *outputStackp;
  genericStack_t        *exceptionStackp;      /* Current exceptions when traversing the ASF */
  int                    asfCallbackLeveli;    /* For tracing */
};

/* When pruning the ASF, we distinguish concatenated strings from structures */
typedef enum marpaEBNFOutputStackType {
  MARPAEBNFOUTPUTSTACKTYPE_STRING = 0,
  MARPAEBNFOUTPUTSTACKTYPE_STRUCT
} marpaEBNFOutputStackType_t;

typedef struct marpaEBNFOutputStack {
  marpaEBNFOutputStackType_t type;
  void                      *p;
} marpaEBNFOutputStack_t;

static inline short  _marpaEBNF_internalGrammarb(marpaEBNF_t *marpaEBNFp);
static inline char  *_marpaEBNF_symbolDescription(void *userDatavp, int symboli);
static inline short  _marpaEBNF_okRuleCallbackb(void *userDatavp, genericStack_t *parentRuleiStackp, int rulei);
static inline short  _marpaEBNF_valueRuleCallback(void *userDatavp, int rulei, int arg0i, int argni, int resulti);
static inline short  _marpaEBNF_valueSymbolCallback(void *userDatavp, int symboli, int argi, int resulti);
static inline short  _marpaEBNF_valueNullingCallback(void *userDatavp, int symboli, int resulti);

static marpaEBNFOption_t marpaEBNFOptionDefault = {
  NULL /* genericLoggerp */
};

static inline void  _marpaEBNF_inputStackFree(marpaEBNF_t *marpaEBNFp);
static inline void  _marpaEBNF_outputStackFree(marpaEBNF_t *marpaEBNFp);
static inline void  _marpaEBNF_exceptionStackFree(marpaEBNF_t *marpaEBNFp);

/****************************************************************************/
marpaEBNF_t *marpaEBNF_newp(marpaEBNFOption_t *marpaEBNFOptionp)
/****************************************************************************/
{
  marpaEBNF_t                *marpaEBNFp;
  marpaWrapperGrammarOption_t marpaWrapperGrammarOption;
  const static size_t         marpaEBNFSymbolArraySizel = MARPAEBNF_LENGTH_ARRAY(marpaEBNFSymbolArray) * sizeof(marpaEBNFSymbol_t);
  const static size_t         marpaEBNFRuleArraySizel = MARPAEBNF_LENGTH_ARRAY(marpaEBNFRuleArray) * sizeof(marpaEBNFRule_t);

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
  marpaEBNFp->symbolArrayp         = NULL;
  marpaEBNFp->ruleArrayp           = NULL;
  
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
  if (marpaEBNFp->marpaWrapperGrammarp == NULL) {
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
  genericStack_t                *alternativeStackp       = NULL;
  int                            posi                    = 0;
  const static size_t            marpaEBNFSymbolArraySizel = MARPAEBNF_LENGTH_ARRAY(marpaEBNFSymbolArray) * sizeof(marpaEBNFSymbol_t);

  marpaWrapperAsf_t             *marpaWrapperAsfp = NULL;
  marpaWrapperRecognizerOption_t marpaWrapperRecognizerOption;
  marpaWrapperAsfOption_t        marpaWrapperAsfOption;
  size_t                         i;
  size_t                         nSymboll;
  int                           *expectedSymbolArrayp;
  marpaEBNFSymbolEnum_t          symboli;
  marpaEBNFSymbol_t             *marpaEBNFSymbolp;
  char                           c1;
  char                           c2;
  char                          *p;
  char                          *q;
  char                          *maxp;
  size_t                         grammarLengthl;
  short                          okb;
  size_t                         lengthl;
  short                          rci;
  int                            valuei;
  
  /* Initialize sensible data used in the err section */
  marpaEBNFp->inputStackp  = NULL;
  marpaEBNFp->outputStackp = NULL;
  marpaEBNFp->exceptionStackp = NULL;

  if (grammars == NULL) {
    errno = EINVAL;
    goto err;
  }

  /* An ISO EBNF grammar is always expressed in the 7-bit ASCII range; that is entirely */
  /* covered by the basic C default character set. We just have to take care to always  */
  /* compare with a char, not an integer value.                                         */
  /* We assume a grammar always first in a single string within memory.                 */
  grammarLengthl = strlen(grammars);

  /* Create input stack */
  GENERICSTACK_NEW(marpaEBNFp->inputStackp);
  if (GENERICSTACK_ERROR(marpaEBNFp->inputStackp)) {
    MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "GENERICSTACK_NEW error, %s", strerror(errno));
    goto err;
  }
  /* Marpa does not like the indice 0 with the input stack, it means an unvalued symbol */
  GENERICSTACK_PUSH_NA(marpaEBNFp->inputStackp);
  if (GENERICSTACK_ERROR(marpaEBNFp->inputStackp)) {
    MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "GENERICSTACK_PUSH_NA error, %s", strerror(errno));
    goto err;
  }
  /* Create output stack */
  GENERICSTACK_NEW(marpaEBNFp->outputStackp);
  if (GENERICSTACK_ERROR(marpaEBNFp->outputStackp)) {
    MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "GENERICSTACK_NEW error, %s", strerror(errno));
    goto err;
  }
  /* Create exception stack and initialize it with no exception (start symbol) */
  GENERICSTACK_NEW(marpaEBNFp->exceptionStackp);
  if (GENERICSTACK_ERROR(marpaEBNFp->exceptionStackp)) {
    MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "GENERICSTACK_NEW error, %s", strerror(errno));
    goto err;
  }
  GENERICSTACK_PUSH_PTR(marpaEBNFp->exceptionStackp, NULL);
  if (GENERICSTACK_ERROR(marpaEBNFp->exceptionStackp)) {
    MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "GENERICSTACK_PUSH_PTR error, %s", strerror(errno));
    goto err;
  }

  /* Parse external grammar using internal grammar, supposed to fit entirely in memory */
  marpaWrapperRecognizerOption.genericLoggerp    = marpaEBNFp->marpaEBNFOption.genericLoggerp;
  marpaWrapperRecognizerOption.disableThresholdb = 0;

  marpaWrapperRecognizerp = marpaWrapperRecognizer_newp(marpaEBNFp->marpaWrapperGrammarp, &marpaWrapperRecognizerOption);
  if (marpaWrapperRecognizerp == NULL) {
    goto err;
  }

  /* Loop until it is consumed or an error */
  p = q = grammars;
  c2 = p[0];         /* Ultimately, this will be '\0' */
  maxp = p + grammarLengthl;
  while (p <= maxp) {

    /* Initialize alternatives stack */
    GENERICSTACK_FREE(alternativeStackp);
    GENERICSTACK_NEW_SIZED(alternativeStackp, marpaEBNFSymbolArraySizel);
    if (GENERICSTACK_ERROR(alternativeStackp)) {
      MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "GENERICSTACK_NEW error, %s", strerror(errno));
      goto err;
    }

    /* Lookup expected terminals */
    if (marpaWrapperRecognizer_expectedb(marpaWrapperRecognizerp, &nSymboll, &expectedSymbolArrayp) == 0) {
      goto err;
    }

    lengthl = 0;
    if (nSymboll > 0) {

      okb = 0;

      if (p < maxp) {
        c1 = c2;
	c2 = *++q;

	for (i = 0; i < nSymboll; i++) {
	  symboli = expectedSymbolArrayp[i];
	  marpaEBNFSymbolp = &(marpaEBNFp->symbolArrayp[symboli]);

	  /* Lexing */
	  switch (symboli) {
	  case LETTER:
	    okb = (((c1 >= 'a') && (c1 <= 'z')) || ((c1 >= 'A') && (c1 <= 'Z')));
	    break;
	  case DECIMAL_DIGIT:
	    okb = ((c1 >= '0') && (c1 <= '9'));
	    break;
	  case CONCATENATE_SYMBOL:
	    okb = (c1 == ',');
	    break;
	  case DEFINING_SYMBOL:
	    okb = (c1 == '=');
	    break;
	  case DEFINITION_SEPARATOR_SYMBOL:
	    okb = ((c1 == '|') || (c1 == '/') || (c1 == '!'));
	    break;
	  case END_COMMENT_SYMBOL:
	    okb = ((c1 == '*') && (c2 == ')'));
	    break;
	  case END_GROUP_SYMBOL:
	    okb = (c1 == ')');
	    break;
	  case _END_OPTION_SYMBOL_0:
	    okb = (c1 == ']');
	    break;
	  case _END_OPTION_SYMBOL_1:
	    okb = ((c1 == '/') && (c2 == ')'));
	    break;
	  case _END_REPEAT_SYMBOL_0:
	    okb = (c1 == '}');
	    break;
	  case _END_REPEAT_SYMBOL_1:
	    okb = ((c1 == ':') && (c2 == ')'));
	    break;
	  case EXCEPT_SYMBOL:
	    okb = (c1 == '-');
	    break;
	  case FIRST_QUOTE_SYMBOL:
	    okb = (c1 == '\'');
	    break;
	  case REPETITION_SYMBOL:
	    okb = (c1 == '*');
	    break;
	  case SECOND_QUOTE_SYMBOL:
	    okb = (c1 == '"');
	    break;
	  case SPECIAL_SEQUENCE_SYMBOL:
	    okb = (c1 == '?');
	    break;
	  case START_COMMENT_SYMBOL:
	    okb = ((c1 == '(') && (c2 == '*'));
	    break;
	  case START_GROUP_SYMBOL:
	    okb = (c1 == '(');
	    break;
	  case _START_OPTION_SYMBOL_0:
	    okb = (c1 == '[');
	    break;
	  case _START_OPTION_SYMBOL_1:
	    okb = ((c1 == '(') && (c2 == '/'));
	    break;
	  case _START_REPEAT_SYMBOL_0:
	    okb = (c1 == '{');
	    break;
	  case _START_REPEAT_SYMBOL_1:
	    okb = ((c1 == '(') && (c2 == ':'));
	    break;
	  case TERMINATOR_SYMBOL:
	    okb = (c1 == ';' || c1 == '.');
	    break;
	  case OTHER_CHARACTER:
	    okb = ((c1 == ' ') || (c1 == ':') || (c1 == '+') || (c1 == '_') || (c1 == '%') || (c1 == '@')  ||
		   (c1 == '&') || (c1 == '#') || (c1 == '$') || (c1 == '<') || (c1 == '>') || (c1 == '\\') ||
		   (c1 == '^') || (c1 == '`') || (c1 == '~'));
	    break;
	  case SPACE_CHARACTER:
	    okb = (c1 == ' ');
	    break;
	  case HORIZONTAL_TABULATION_CHARACTER:
	    okb = (c1 == '\t');
	    break;
          case _CARRIAGE_RETURN:
	    okb = (c1 == '\r');
	    break;
          case _LINE_FEED:
	    if ((okb = (c1 == '\n'))) {
	      ++linel;
	      columnl = 1;
	    }
	    break;
	    break;
	  case VERTICAL_TABULATION_CHARACTER:
	    okb = (c1 == '\v');
	    break;
	  case FORM_FEED:
	    okb = (c1 == '\f');
	    break;
	  default:
	    okb = 0;
	    break;
	  }

	  if (okb) {
	    /* Force lengthl for multiple-characters mapped to a single token */
	    switch (symboli) {
	    case START_COMMENT_SYMBOL:
	    case END_COMMENT_SYMBOL:
	    case _START_OPTION_SYMBOL_1:
	    case _END_OPTION_SYMBOL_1:
	    case _START_REPEAT_SYMBOL_1:
	    case _END_REPEAT_SYMBOL_1:
	      if (lengthl != 2) {
		/* First time we hit a multiple-characters lexeme: re-initialize alternatives */
		GENERICSTACK_FREE(alternativeStackp);
		GENERICSTACK_NEW_SIZED(alternativeStackp, marpaEBNFSymbolArraySizel);
		if (GENERICSTACK_ERROR(alternativeStackp)) {
		  MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "GENERICSTACK_NEW error, %s", strerror(errno));
		  goto err;
		}
		lengthl = 2;
	      } else if ((lengthl > 0) && (lengthl != 2)) {
		/* Not the first time, and we are hitting a single-character lexeme: ignore it */
		okb = 0;
	      }
	      break;
	    default:
	      if (lengthl > 0) {
		/* This can happen only when lengthl is 2, and when current lexeme is a single character: ignored */
		okb = 0;
	      }
	      break;
	    }
	  }

	  if (okb) {
	    MARPAEBNF_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, funcs, "[%d/%d] %c%c %s", (int) (p - grammars), (int) (grammarLengthl - 1), (lengthl == 2) ? c1 : ' ', (lengthl == 2) ? c2 : c1, _marpaEBNF_symbolDescription(marpaEBNFp, symboli));
            GENERICSTACK_PUSH_INT(alternativeStackp, symboli);
            if (GENERICSTACK_ERROR(alternativeStackp)) {
              MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "GENERICSTACK_PUSH_INT error, %s", strerror(errno));
              goto err;
            }
	    /* Force lengthl for multiple-characters mapped to a single token */
	    switch (symboli) {
	    case START_COMMENT_SYMBOL:
	    case END_COMMENT_SYMBOL:
	    case _START_OPTION_SYMBOL_1:
	    case _END_OPTION_SYMBOL_1:
	    case _START_REPEAT_SYMBOL_1:
	    case _END_REPEAT_SYMBOL_1:
	      lengthl = 2;
	      break;
	    default:
	      break;
	    }
	  }
	  
	}
      }

      /* We micro-optimized by not coding lengthl = 1 */
      if ((GENERICSTACK_USED(alternativeStackp) > 0) && (lengthl <= 0)) {
	lengthl = 1;
      }
    }

    if (lengthl > 0) {
      size_t alternativel;
      int    valuei;
      char  *tokens;

      /* A token value is a copy of its corresponding string */
      tokens = (char *) malloc(lengthl + 1);
      if (tokens == NULL) {
	MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "malloc error, %s", strerror(errno));
	goto err;
      }
      strncpy(tokens, grammars + posi, lengthl);
      tokens[lengthl] = '\0';
      GENERICSTACK_PUSH_PTR(marpaEBNFp->inputStackp, tokens);
      if (GENERICSTACK_ERROR(marpaEBNFp->inputStackp)) {
        MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "GENERICSTACK_PUSH_PTR error, %s", strerror(errno));
        goto err;
      }

      /* Because we pushed a NULL as a first member of inputStack, valuei is always > 0 here: */
      valuei = GENERICSTACK_USED(marpaEBNFp->inputStackp) - 1;
      for (alternativel = 0; alternativel < GENERICSTACK_USED(alternativeStackp); alternativel++) {
	symboli = GENERICSTACK_GET_INT(alternativeStackp, alternativel);
        if (GENERICSTACK_ERROR(alternativeStackp)) {
          MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "GENERICSTACK_GET_INT error, %s", strerror(errno));
          goto err;
        }
        /* If lengthl > 1, we know it can be only one specific lexeme */
        if ((lengthl > 1)                       &&
            (symboli != START_COMMENT_SYMBOL)   &&
            (symboli != END_COMMENT_SYMBOL)     &&
            (symboli != _START_OPTION_SYMBOL_1) &&
            (symboli != _END_OPTION_SYMBOL_1)   &&
            (symboli != _START_REPEAT_SYMBOL_1) &&
            (symboli != _END_REPEAT_SYMBOL_1)) {
          continue;
        }
	if (marpaWrapperRecognizer_alternativeb(marpaWrapperRecognizerp, symboli, valuei, 1) == 0) {
	  goto err;
	}
      }
      if (marpaWrapperRecognizer_completeb(marpaWrapperRecognizerp) == 0) {
	goto err;
      }
    } else {
      /* This is an error if we are not at the end of the grammar and this is not a "space" */
      if (p < maxp) {
        if (! isspace(c1)) {
          MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "[%d/%d] Current character: 0x%02x '%c'%s is not recognized",
                           (int) (p - grammars),
                           (int) (grammarLengthl - 1),
                           (int) c1,
                           isprint((int) c1) ? c1 : ' ',
                           isprint((int) c1) ? "" : " (non printable)");
          goto err;
        } else {
          MARPAEBNF_TRACEF(marpaEBNFp->marpaEBNFOption.genericLoggerp, funcs, "[%d/%d] Discarded character: 0x%02x '%c'%s",
                           (int) (p - grammars),
                           (int) (grammarLengthl - 1),
                           (int) c1, isprint((int) c1) ? c1 : ' ',
                           isprint((int) c1) ? "" : " (non printable)");
          lengthl = 1;
        }
      }
    }

    if (p < maxp) {
      /* Resume */   
      p += lengthl;
      posi += lengthl;
      q = p;
      c2 = *p;
    } else {
      break;
    }
  }

  /* Traverse the ASF */
  MARPAEBNF_TRACE(marpaEBNFp->marpaEBNFOption.genericLoggerp, funcs, "Building ASF");
  marpaWrapperAsfOption.genericLoggerp = marpaEBNFp->marpaEBNFOption.genericLoggerp;
  marpaWrapperAsfOption.highRankOnlyb = 0;
  marpaWrapperAsfOption.orderByRankb = 0;
  marpaWrapperAsfOption.ambiguousb = 1;
  marpaWrapperAsfp = marpaWrapperAsf_newp(marpaWrapperRecognizerp, &marpaWrapperAsfOption);
  if (marpaWrapperAsfp == NULL) {
    goto err;
  }

  /* Prune the ASF */
  if (! marpaWrapperAsf_valueb(marpaWrapperAsfp,
			       marpaEBNFp,
			       _marpaEBNF_okRuleCallbackb,
			       _marpaEBNF_valueRuleCallback,
			       _marpaEBNF_valueSymbolCallback,
			       _marpaEBNF_valueNullingCallback,
			       &valuei)) {
    goto err;
  }

  rci = 1;
  goto done;

 err:
  rci = 0;

 done:
  _marpaEBNF_inputStackFree(marpaEBNFp);
  _marpaEBNF_outputStackFree(marpaEBNFp);
  _marpaEBNF_exceptionStackFree(marpaEBNFp);

  GENERICSTACK_FREE(alternativeStackp);

  if (marpaWrapperAsfp != NULL) {
    /* Must be done before free on the recognizer */
    marpaWrapperAsf_freev(marpaWrapperAsfp);
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
  int                   i;
  marpaEBNFSymbolEnum_t symboli;
  int                   rulei;

  /* Declare all the symbols */
  for (i = 0; i < MARPAEBNF_LENGTH_ARRAY(marpaEBNFSymbolArray); i++) {
    symboli = marpaWrapperGrammar_newSymboli(marpaEBNFp->marpaWrapperGrammarp, NULL);
    /* We take advantage of the fact that symbols always start at 0 with marpa */
    if (symboli != i) {
      MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "symboli is %d != %d", symboli, i);
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
    /* We take advantage of the fact that rules always start at 0 with marpa */
    if (rulei != i) {
      MARPAEBNF_ERRORF(marpaEBNFp->marpaEBNFOption.genericLoggerp, "rulei is %d != %d", rulei, i);
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
static inline void _marpaEBNF_inputStackFree(marpaEBNF_t *marpaEBNFp)
/****************************************************************************/
{
  size_t i;
  
  if (marpaEBNFp->inputStackp != NULL) {
    for (i = 0; i < GENERICSTACK_USED(marpaEBNFp->inputStackp); i++) {
      if (GENERICSTACK_IS_PTR(marpaEBNFp->inputStackp, i)) {
	char *p = GENERICSTACK_GET_PTR(marpaEBNFp->inputStackp, i);
	if (p != NULL) {
	  free(p);
	}
      }
    }
    GENERICSTACK_FREE(marpaEBNFp->inputStackp);
  }
}

/****************************************************************************/
static inline void _marpaEBNF_outputStackFree(marpaEBNF_t *marpaEBNFp)
/****************************************************************************/
{
  size_t i;
  
  if (marpaEBNFp->outputStackp != NULL) {
    for (i = 0; i < GENERICSTACK_USED(marpaEBNFp->outputStackp); i++) {
      if (GENERICSTACK_IS_PTR(marpaEBNFp->outputStackp, i)) {
	marpaEBNFOutputStack_t *marpaEBNFOutputStackp = (marpaEBNFOutputStack_t *) GENERICSTACK_GET_PTR(marpaEBNFp->outputStackp, i);

	if (marpaEBNFOutputStackp != NULL) {
	  switch (marpaEBNFOutputStackp->type) {
	  case MARPAEBNFOUTPUTSTACKTYPE_STRING:
	    {
	      char *p = (char *) marpaEBNFOutputStackp->p;
	      if (p != NULL) {
		free(p);
	      }
	    }
	    break;
	  default:
	    MARPAEBNF_WARN(marpaEBNFp->marpaEBNFOption.genericLoggerp, "TO DO");
	    break;
	  }
	  free(marpaEBNFOutputStackp);
	}
      }
    }
    GENERICSTACK_FREE(marpaEBNFp->outputStackp);
  }
}

/****************************************************************************/
static inline void _marpaEBNF_exceptionStackFree(marpaEBNF_t *marpaEBNFp)
/****************************************************************************/
{
  if (marpaEBNFp->exceptionStackp != NULL) {
    GENERICSTACK_FREE(marpaEBNFp->exceptionStackp);
  }
}

/****************************************************************************/
static inline short  _marpaEBNF_okRuleCallbackb(void *userDatavp, genericStack_t *parentRuleiStackp, int rulei)
/****************************************************************************/
{
  const static char  funcs[]          = "_marpaEBNF_okRuleCallbackb";
  marpaEBNF_t       *marpaEBNFp       = (marpaEBNF_t *) userDatavp;
  genericLogger_t   *genericLoggerp   = marpaEBNFp->marpaEBNFOption.genericLoggerp;
  size_t             parentStackUsedl = GENERICSTACK_USED(parentRuleiStackp);
  int                grandParentRulei;
  marpaEBNFRule_t   *rulep;
  int                i;
  int                maxi;
  short              rcb = 0;

  if (parentStackUsedl > 2) {
    grandParentRulei = GENERICSTACK_GET_INT(parentRuleiStackp, parentStackUsedl - 2);
    if (GENERICSTACK_ERROR(parentRuleiStackp)) {
      /* Is rulei an exception of grandParentRulei->parentRulei ? */
      rulep = &(marpaEBNFp->ruleArrayp[grandParentRulei]);
      maxi = (int) rulep->rhsExceptl;
      for (i = 0; i < maxi; maxi++) {
	if (rulep->rhsExceptp[i] == rulei) {
	  rcb = 1;
	  break;
	}
      }
    }
  }

  MARPAEBNF_TRACEF(genericLoggerp, funcs, "return %d", (int) rcb);
  return 0;
}

/****************************************************************************/
static inline short  _marpaEBNF_valueSymbolCallback(void *userDatavp, int symboli, int argi, int resulti)
/****************************************************************************/
{
  const static char  funcs[]          = "_marpaEBNF_okRuleCallbackb";
  marpaEBNF_t       *marpaEBNFp       = (marpaEBNF_t *) userDatavp;
  genericLogger_t   *genericLoggerp   = marpaEBNFp->marpaEBNFOption.genericLoggerp;

  /* We set in outputStack at indice resulti the duplicate of inputStack at indice argi */
}

