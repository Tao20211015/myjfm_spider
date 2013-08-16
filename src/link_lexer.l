/*******************************************************************************
 * link_lexer.l -- Function for detecting hyper links in html file.
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

%option c++ noyywrap stack
%option yyclass="myjfm::LinkLexer"

%s COMMENT
%s META
%s A
%s AREA
%s BASE
%s FRAME
%s IFRAME
%s IMG
%s LINK
%s SCRIPT
%s VALUE
%s IGNORE_VALUE
%s DQUOTED
%s SQUOTED
%s UQUOTED
%s ENTITY

O_BLANK             [ \t\r\n]
O_CDATA             [A-Za-z][A-Za-z0-9\-_:\.]*
O_DIGIT             [0-9]
O_HEX               {O_DIGIT}|[A-Fa-f]
O_L_META            "<"[Mm][Ee][Tt][Aa]{O_BLANK}
O_L_A               "<"[Aa]{O_BLANK}
O_L_AREA            "<"[Aa][Rr][Ee][Aa]{O_BLANK}
O_L_BASE            "<"[Bb][Aa][Ss][Ee]{O_BLANK}
O_L_FRAME           "<"[Ff][Rr][Aa][Mm][Ee]{O_BLANK}
O_L_IFRAME          "<"[Ii][Ff][Rr][Aa][Mm][Ee]{O_BLANK}
O_L_IMG             "<"[Ii][Mm][Gg]{O_BLANK}
O_L_LINK            "<"[Ll][Ii][Nn][Kk]{O_BLANK}
O_L_SCRIPT          "<"[Ss][Cc][Rr][Ii][Pp][Tt]{O_BLANK}

%{
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "config.h"
#include "log.h"
#include "url.h"
#include "link_lexer.h"
#include "uri.h"
%}

%%
<INITIAL>"<!--" { BEGIN COMMENT; }

<COMMENT>"-->" |
<COMMENT><<EOF>> { BEGIN INITIAL; }
<COMMENT>.|\n {}

<INITIAL>{O_L_META} {
  yy_push_state(META);
}

<INITIAL>{O_L_A} {
  yy_push_state(A);
}

<INITIAL>{O_L_AREA} {
  yy_push_state(AREA);
}

<INITIAL>{O_L_FRAME} {
  yy_push_state(FRAME);
}

<INITIAL>{O_L_IFRAME} {
  yy_push_state(IFRAME);
}

<INITIAL>{O_L_IMG} {
  yy_push_state(IMG);
}

<INITIAL>{O_L_LINK} {
  yy_push_state(LINK);
}

<INITIAL>{O_L_SCRIPT} {
  yy_push_state(SCRIPT);
}

<INITIAL>.|\n {}
<INITIAL><<EOF>> { return 0; }

<META>{O_BLANK}{0,512}{O_CDATA}"="{O_BLANK}{0,512} {
  while (IS_BLANK(*yytext)) {
    ++yytext;
  }
  yyleng = 0;
  while (yytext[yyleng] != '=') {
    if (yytext[yyleng] >= 'A' && yytext[yyleng] <= 'Z') {
      yytext[yyleng] += 'a' - 'A';
    }
    ++yyleng;
  }
  yytext[yyleng] = '\0';
  if (Utility::strcmp(yytext, "http-equiv") == 0 || 
      (Utility::strcmp(yytext, "content") == 0 && 
       _cur_attr == "http-equiv")) {
    _cur_attr = yytext;
    yy_push_state(VALUE);
  } else {
    _cur_attr = "";
    yy_push_state(IGNORE_VALUE);
  }
}

<A,AREA,BASE,LINK>{O_BLANK}{0,512}{O_CDATA}"="{O_BLANK}{0,512} {
  while (IS_BLANK(*yytext)) {
    ++yytext;
  }
  yyleng = 0;
  while (yytext[yyleng] != '=') {
    if (yytext[yyleng] >= 'A' && yytext[yyleng] <= 'Z') {
      yytext[yyleng] += 'a' - 'A';
    }
    ++yyleng;
  }
  yytext[yyleng] = '\0';
  if (Utility::strcmp(yytext, "href") == 0) {
    if (YY_START == BASE) {
      _base_flag = true;
    }
    _cur_attr = yytext;
    yy_push_state(VALUE);
  } else {
    _cur_attr = "";
    yy_push_state(IGNORE_VALUE);
  }
}

<FRAME,IFRAME,IMG>{O_BLANK}{0,512}{O_CDATA}"="{O_BLANK}{0,512} {
  while (IS_BLANK(*yytext)) {
    ++yytext;
  }
  yyleng = 0;
  while (yytext[yyleng] != '=') {
    if (yytext[yyleng] >= 'A' && yytext[yyleng] <= 'Z') {
      yytext[yyleng] += 'a' - 'A';
    }
    ++yyleng;
  }
  yytext[yyleng] = '\0';
  if (Utility::strcmp(yytext, "src") == 0) {
    _cur_attr = yytext;
    yy_push_state(VALUE);
  } else {
    _cur_attr = "";
    yy_push_state(IGNORE_VALUE);
  }
}

<META,A,AREA,BASE,FRAME,IFRAME,IMG,LINK>{O_BLANK}{0,512}">" |
<META,A,AREA,BASE,FRAME,IFRAME,IMG,LINK><<EOF>> { yy_pop_state(); }
<META,A,AREA,BASE,FRAME,IFRAME,IMG,LINK>.|\n {}

<IGNORE_VALUE>{O_BLANK}{0,512}\"[^"]* |
<IGNORE_VALUE>{O_BLANK}{0,512}"'"[^']* { yyinput(); yy_pop_state(); }
<IGNORE_VALUE>{O_BLANK}{0,512}[^ \t\r\n>]* {
  char c = yyinput();
  if (c == '>') {
    BEGIN INITIAL;
  } else {
    yy_pop_state();
  }
}

<IGNORE_VALUE><<EOF>> { yy_pop_state(); }

<VALUE>\"{O_BLANK}{0,512} {
  _tmp_buf = "";
  yy_push_state(DQUOTED);
}

<VALUE>"'"{O_BLANK}{0,512} {
  _tmp_buf = "";
  yy_push_state(SQUOTED);
}

<VALUE>[^"'] {
  _tmp_buf = "";
  unput(yytext[yyleng - 1]);
  yy_push_state(UQUOTED);
}

<VALUE><<EOF>> { yy_pop_state(); }

<DQUOTED,SQUOTED>\r|\n {}

<DQUOTED>{O_BLANK}{0,512}\" |
<SQUOTED>{O_BLANK}{0,512}"'" |
<UQUOTED>{O_BLANK}|">" {
  if (_cur_attr == "http-equiv") {
    _http_equiv = _tmp_buf;
  } else if (_cur_attr == "content") {
    _content = _tmp_buf;
  } else if (_cur_attr == "href" || _cur_attr == "src") {
    save_link(_tmp_buf);
  } else {
    yyterminate();
  }

  _tmp_buf = "";
  yy_pop_state();
  yy_pop_state();

  if (yytext[yyleng - 1] == '>') {
    yy_pop_state();
  }
}

<DQUOTED,SQUOTED,UQUOTED>"&#"{O_DIGIT}{1,10}";" |
<DQUOTED,SQUOTED,UQUOTED>"&#"(X|x){O_HEX}{1,8}";" {
  yytext[yyleng - 1] = '\0';
  if (yytext[2] == 'X' || yytext[2] == 'x') {
    sscanf(yytext + 3, "%x", &yyleng);                                          
  } else {
    yyleng = atoi(yytext + 2);
    do {
      unput(yyleng & 0xff);
      yy_push_state(ENTITY);
    } while ((yyleng = (unsigned int)yyleng >> 8) > 0);
  }
}
 
<DQUOTED,SQUOTED,UQUOTED>"&lt;" {
  unput('<');
  yy_push_state(ENTITY);
}

<DQUOTED,SQUOTED,UQUOTED>"&gt;" {
  unput('>');
  yy_push_state(ENTITY);
}

<DQUOTED,SQUOTED,UQUOTED>"&amp;" {
  unput('&');
  yy_push_state(ENTITY);
}

<DQUOTED,SQUOTED,UQUOTED>"&quot;" {
  unput('"');
  yy_push_state(ENTITY);
}

<DQUOTED,SQUOTED,UQUOTED,ENTITY>.|\n {
  if (YY_START == ENTITY) {
    yy_pop_state();
  }
  
  if (IS_URI_CHR(*yytext) || *yytext == '%') {
    _tmp_buf += yytext;
  } else {
    char tmp[4];
    sprintf(tmp, "%%%X%X", (unsigned char)*yytext >> 4, *yytext & 0x0f);
    _tmp_buf += tmp;
  }
}

<DQUOTED,SQUOTED,UQUOTED><<EOF>> {
  yy_pop_state();
}

<SCRIPT>"</script>" |
<SCRIPT><<EOF>> {
  yy_pop_state();
}
<SCRIPT>.|\n {}
%%

_START_MYJFM_NAMESPACE_

RES_CODE LinkLexer::yylex(const char* content) {
  String str(content);
  Istringstream strstr(str);
  switch_streams((Istream*)&strstr);
  BEGIN INITIAL;
  yylex();
  return S_OK;
}

RES_CODE LinkLexer::lex() {
  if (_uris == NULL) {
    return S_FAIL;
  }

  if (_page_p.is_null()) {
    _uris->clear();
    return S_FAIL;
  }

  String site;
  _page_p->get_site(site);
  uint16_t port_i;
  String port;
  _page_p->get_port(port_i);
  Utility::integer2str<uint16_t>(port_i, port);

  String file;
  _page_p->get_file(file);

  Uri uri;
  if (generate_uri(site, port, file, uri) != S_OK) {
    return S_FAIL;
  }

  _base_uri.copy_from(uri);

  LOG(INFO, "");
  LOG(INFO, "***************************");
  LOG(INFO, "scheme:   %s", _base_uri._scheme);
  LOG(INFO, "path:     %s", _base_uri._path);
  if (_base_uri._authority && _base_uri._authority->_type == AT_SERVER) {
    LOG(INFO, "userinfo: %s", _base_uri._authority->_userinfo);
    LOG(INFO, "host:     %s", _base_uri._authority->_host);
    LOG(INFO, "port:     %s", _base_uri._authority->_port);
  }
  LOG(INFO, "query:    %s", _base_uri._query);
  LOG(INFO, "fragment: %s", _base_uri._fragment);
  LOG(INFO, "***************************");
  LOG(INFO, "");

  char* page_content = NULL;
  _page_p->get_page_content(page_content);

  yylex(page_content);

  return S_OK;
}

RES_CODE LinkLexer::get_http_equiv(String& http_equiv) {
  http_equiv = _http_equiv;
  return S_OK;
}

RES_CODE LinkLexer::get_content(String& content) {
  content = _content;
  return S_OK;
}

RES_CODE LinkLexer::save_link(String& link) {
  LOG(INFO, "[raw] %s", link.c_str());

  Uri uri;
  if (_base_flag) {
    _base_flag = false;
    Url url(link);
    Url::EnumStatus status;
    url.get_status(status);
    if (status != Url::INITIALIZED) {
      return S_FAIL;
    }
    String site;
    url.get_site(site);
    uint16_t port_i;
    String port;
    url.get_port(port_i);
    Utility::integer2str<uint16_t>(port_i, port);
    String file;
    url.get_file(file);
    if (generate_uri(site, port, file, uri) != S_OK) {
      return S_FAIL;
    }
    _base_uri.copy_from(uri);
  }

  uri.reinit();
  _uri_lexer->init(link.c_str(), &_base_uri, &uri);
  if (_uri_lexer->lex() != S_OK) {
    return S_FAIL;
  }

  if (uri._scheme != NULL && Utility::strcasecmp(uri._scheme, "http") != 0) {
    return S_OK;
  }

  Uri merged_uri;
  if (_uri_lexer->uri_merge(&merged_uri) != S_OK) {
    return S_FAIL;
  }

  link = "";
  _uri_lexer->uri_combine(link, C_URI, &merged_uri);

  LOG(INFO, "[new] %s", link.c_str());

  _uris->push_back(link);

  return S_OK;
}

RES_CODE LinkLexer::generate_uri(String& site, 
    String& port, String& file, Uri& uri) {
  uri.reinit();
  uri._scheme = Utility::strdup("http");

  uri._authority = new Authority(AT_SERVER);
  uri._authority->_host = Utility::strdup(site.c_str());
  if (port != "80") {
    uri._authority->_port = Utility::strdup(port.c_str());
  }

  String_size_t pos = file.find("#");
  if (pos == String_tail) {
    pos = file.find("?");
    if (pos == String_tail) {
      uri._path = Utility::strdup(file.c_str());
    } else {
      uri._path = Utility::strdup(file.substr(0, pos).c_str());
      uri._query = Utility::strdup(file.substr(pos + 1).c_str());
    }
  } else {
    uri._fragment = Utility::strdup(file.substr(pos + 1).c_str());
    String new_file = file.substr(0, pos);
    pos = new_file.find("?");
    if (pos == String_tail) {
      uri._path = Utility::strdup(new_file.c_str());
    } else {
      uri._path = Utility::strdup(new_file.substr(0, pos).c_str());
      uri._query = Utility::strdup(new_file.substr(pos).c_str());
    }
  }

  return S_OK;
}

_END_MYJFM_NAMESPACE_

