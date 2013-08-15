#ifndef _URI_LEXER_H_
#define _URI_LEXER_H_

#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif

#include "config.h"
#include "shared.h"
#include "uri.h"

_START_MYJFM_NAMESPACE_

class UriLexer : public yyFlexLexer, public Shared {
public:
  UriLexer() {
    init(NULL, NULL, NULL);
  }

  virtual ~UriLexer() {}

  RES_CODE init(const char* str, const Uri* base_uri, Uri* uri) {
    _str = str;
    _base_uri = base_uri;
    _uri = uri;
    return S_OK;
  }

  RES_CODE lex();

  int uri_length();
  
  RES_CODE uri_merge(Uri* result);

  RES_CODE uri_combine(String& uristr, int flags, Uri* uri = NULL);

private:
  UriLexer(const UriLexer&);
  UriLexer& operator=(const UriLexer&);

  RES_CODE yylex(const char* content);

  // hide this one from public view
  int yylex();

  RES_CODE path_merge(const char *rel_path, 
      const char *base_path, char **result);

  const char* _str;
  int _length;
  const Uri* _base_uri;
  Uri* _uri;
};

_END_MYJFM_NAMESPACE_

#endif

