#ifndef _LINK_LEXER_H_
#define _LINK_LEXER_H_

#include "config.h"
#include "shared_pointer.h"
#include "page.h"

#ifndef __FLEX_LEXER_H
#include <FlexLexer.h>
#endif

static char __uri_chr[] = {
  0x00, 0x00, 0x00, 0x00, 
  0x5b, 0xff, 0xff, 0xf5, 
  0xff, 0xff, 0xff, 0xe1, 
  0x7f, 0xff, 0xff, 0xe2
};

_START_MYJFM_NAMESPACE_

class LinkLexer : public ::yyFlexLexer {
public:
  LinkLexer(SharedPointer<Page>& page_p, Vector<String>* uris) : 
    _page_p(page_p), 
    _uris(uris) {}
  virtual ~LinkLexer() {}

  RES_CODE lex();

  RES_CODE get_http_equiv(String& http_equiv);
  RES_CODE get_content(String& http_equiv);

private:
  LinkLexer(const LinkLexer&);
  LinkLexer& operator=(const LinkLexer&);

  RES_CODE yylex(const char* content);

  // hide this one from public view
  int yylex();

#define is_uri_chr(c) ({ \
  unsigned char tmp = (c); \
  tmp < 128 && __uri_chr[tmp >> 3] & 0x80 >> (tmp & 0x07); \
})

  // temporary variable
  String _cur_attr;
  String _tmp_buf;

  // for meta label
  String _http_equiv;
  String _content;

  SharedPointer<Page> _page_p;

  // store all the uris parsed from the content
  Vector<String>* _uris;
};

_END_MYJFM_NAMESPACE_

#endif

