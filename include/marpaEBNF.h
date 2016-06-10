#ifndef MARPAEBNF_H
#define MARPAEBNF_H

#include "marpaWrapper.h"
#include "genericLogger.h"
#include "marpaEBNF/export.h"

typedef struct marpaEBNFOption {
  genericLogger_t *genericLoggerp;
} marpaEBNFOption_t;

typedef struct marpaEBNF marpaEBNF_t;
typedef struct marpaEBNFGrammar marpaEBNFGrammar_t;

#ifdef __cplusplus
extern "C" {
#endif
  marpaEBNF_EXPORT marpaEBNF_t *marpaEBNF_newp(marpaEBNFOption_t *marpaEBNFOptionp);
  marpaEBNF_EXPORT short        marpaEBNF_grammarb(marpaEBNF_t *marpaEBNFp, char *grammars);
  marpaEBNF_EXPORT void         marpaEBNF_freev(marpaEBNF_t *marpaEBNFp);
#ifdef __cplusplus
}
#endif

#endif /* MARPAEBNF_H*/
