#ifndef MARPAEBNF_H
#define MARPAEBNF_H

#include "marpaWrapper.h"
#include "genericLogger.h"
#include "marpaEBNF/export.h"

typedef struct marpaEBNFOption {
  marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp;
} marpaEBNFOption_t;

typedef struct marpaEBNF marpaEBNF_t;
typedef struct marpaEBNFGrammar marpaEBNFGrammar_t;

#ifdef __cplusplus
extern "C" {
#endif
  marpaEBNF_EXPORT marpaEBNF_t         *marpaEBNF_newp(marpaEBNFOption_t *marpaEBNFOptionp);
  marpaEBNF_EXPORT marpaEBNFGrammar_t   marpaEBNFGrammar_newp(marpaEBNF_t *marpaEBNFp, char *grammars);
  marpaEBNF_EXPORT void                 marpaEBNFGrammar_freev(marpaEBNFGrammar_t *marpaEBNFGrammarp);
  marpaEBNF_EXPORT void                 marpaEBNF_freev(marpaEBNF_t *marpaEBNFp);
#ifdef __cplusplus
}
#endif

#endif /* MARPAEBNF_H*/
