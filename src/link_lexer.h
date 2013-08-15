#ifndef _LINK_LEXER_H_
#define _LINK_LEXER_H_

#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif

#include "config.h"
#include "shared.h"
#include "shared_pointer.h"
#include "page.h"
#include "uri.h"
#include "uri_lexer.h"

_START_MYJFM_NAMESPACE_

class LinkLexer : public yyFlexLexer, public Shared {
public:
  LinkLexer() {
    _page_p = SharedPointer<Page>(NULL);
    _uris = NULL;
    _cur_attr = "";
    _tmp_buf = "";
    _http_equiv = "";
    _content = "";
    _uri_lexer = SharedPointer<UriLexer>(new UriLexer());
    _base_uri.reinit();
    _base_flag = false;
  }

  LinkLexer(SharedPointer<Page>& page_p, Vector<String>* uris) {
    init(page_p, uris);
    _uri_lexer = SharedPointer<UriLexer>(new UriLexer());
    _base_uri.reinit();
    _base_flag = false;
  }

  virtual ~LinkLexer() {}

  RES_CODE init(SharedPointer<Page>& page_p, Vector<String>* uris) {
    _page_p = page_p;
    _uris = uris;
    _cur_attr = "";
    _tmp_buf = "";
    _http_equiv = "";
    _content = "";
    _base_uri.reinit();
    _base_flag = false;
    return S_OK;
  }

  RES_CODE lex();

  RES_CODE get_http_equiv(String& http_equiv);
  RES_CODE get_content(String& http_equiv);
  RES_CODE generate_uri(String& site, String& port, String& file, Uri& uri);

private:
  LinkLexer(const LinkLexer&);
  LinkLexer& operator=(const LinkLexer&);

  RES_CODE yylex(const char* content);

  // hide this one from public view
  int yylex();
  
  RES_CODE save_link(String& link);

  // temporary variable
  String _cur_attr;
  String _tmp_buf;

  // for meta label
  String _http_equiv;
  String _content;

  SharedPointer<Page> _page_p;

  // store all the uris parsed from the content
  Vector<String>* _uris;
  SharedPointer<UriLexer> _uri_lexer;
  Uri _base_uri;
  bool _base_flag;
};

_END_MYJFM_NAMESPACE_

#endif

