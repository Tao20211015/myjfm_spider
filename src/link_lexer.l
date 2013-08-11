/*******************************************************************************
 * link_lexer.l -- Function for detecting hyper links in html file.
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

%option c++ noyywrap stack
%option yyclass="myjfm::LinkLexer"

%s COMMENT
%s LABEL
%s VALUE
%s IGNORE_VALUE
%s DQUOTED
%s SQUOTED
%s UQUOTED
%s ENTITY

O_BLANK     [ \t\r\n]
O_CDATA     [A-Za-z][A-Za-z0-9\-_:\.]*
O_DIGIT     [0-9]
O_HEX       {O_DIGIT}|[A-Fa-f]
O_L_META    "<"[Mm][Ee][Tt][Aa]{O_BLANK}
O_L_A       "<"[Aa]{O_BLANK}
O_L_AREA    "<"[Aa][Rr][Ee][Aa]{O_BLANK}
O_L_BASE    "<"[Bb][Aa][Ss][Ee]{O_BLANK}
O_L_FRAME   "<"[Ff][Rr][Aa][Mm][Ee]{O_BLANK}
O_L_IFRAME  "<"[Ii][Ff][Rr][Aa][Mm][Ee]{O_BLANK}
O_L_IMG     "<"[Ii][Mm][Gg]{O_BLANK}
O_L_LINK    "<"[Ii][Mm][Gg]{O_BLANK}

%{
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "config.h"
#include "link_lexer.h"

#define is_blank(c) ({ \
  char tmp = (c); \
  tmp == ' ' || tmp == '\t' || tmp == '\r' || tmp == '\n'; \
})
%}

%%
<INITIAL>"<!--" { BEGIN COMMENT; }

<COMMENT>"-->" |
<COMMENT><<EOF>> { BEGIN INITIAL; }
<COMMENT>.|\n {}

<INITIAL>{O_L_META}|{O_L_A}|{O_L_AREA}|{O_L_BASE} |
<INITIAL>{O_L_FRAME}|{O_L_IFRAME}|{O_L_IMG}|{O_L_LINK} { BEGIN LABEL; }

<INITIAL>"<"{O_CDATA}/{O_BLANK}|">" |
<INITIAL>"</"{O_CDATA}">" |
<INITIAL>.|\n {}
<INITIAL><<EOF>> { return 0; }

<LABEL>{O_BLANK}{0,512}{O_CDATA}"="{O_BLANK}{0,512} {
  while (is_blank(*yytext)) {
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
  if (strcasecmp(yytext, "http-equiv") == 0 || 
      (strcasecmp(yytext, "content") == 0 && _cur_attr == "http-equiv") ||
      strcasecmp(yytext, "href") == 0 || 
      strcasecmp(yytext, "src") == 0) {
    _cur_attr = yytext;
    yy_push_state(VALUE);
  } else {
    _cur_attr = "";
    yy_push_state(IGNORE_VALUE);
  }
}

<LABEL>"/>"|">" |
<LABEL><<EOF>> { BEGIN INITIAL; }
<LABEL>.|\n {}

<IGNORE_VALUE>{O_BLANK}{0,512}["][^"]* |
<IGNORE_VALUE>{O_BLANK}{0,512}['][^']* { yyinput(); yy_pop_state(); }
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

<DQUOTED>{O_BLANK}{0,512}["] |
<SQUOTED>{O_BLANK}{0,512}['] |
<UQUOTED>{O_BLANK}|">" {
  if (_cur_attr == "http-equiv") {
    _http_equiv = _tmp_buf;
  } else if (_cur_attr == "content") {
    _content = _tmp_buf;
  } else if (_cur_attr == "href" || _cur_attr == "src") {
    _uris->push_back(_tmp_buf);
  } else {
    yyterminate();
  }

  _tmp_buf = "";

  if (yytext[yyleng - 1] == '>') {
    BEGIN INITIAL;
  } else {
    yy_pop_state();
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
  
  if (is_uri_chr(*yytext) || *yytext == '%') {
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
%%

RES_CODE _MYJFM_NAMESPACE_::LinkLexer::yylex(const char* content) {
  String str(content);
  Istringstream strstr(str);
  switch_streams((Istream*)&strstr);
  yylex();
  return S_OK;
}

RES_CODE _MYJFM_NAMESPACE_::LinkLexer::lex() {
  char* page_content = NULL;
  if (_page_p.is_null()) {
    _uris->clear();
    return S_FAIL;
  }
  _page_p->get_page_content(page_content);
  yylex(page_content);
  return S_OK;
}

RES_CODE _MYJFM_NAMESPACE_::LinkLexer::get_http_equiv(String& http_equiv) {
  http_equiv = _http_equiv;
  return S_OK;
}

RES_CODE _MYJFM_NAMESPACE_::LinkLexer::get_content(String& content) {
  content = _content;
  return S_OK;
}

