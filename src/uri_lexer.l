 /******************************************************************************
  * uri.l -- Routines dealing with URI, mainly parsing and merging.
  *
  * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
  * All rights reserved.
  *****************************************************************************/

%option c++ noyywrap stack
%option yyclass="myjfm::UriLexer"
%option prefix="Uri"

%s SCHEME
%s REL_PATH
%s AUTHORITY
%s USERINFO
%s HOST
%s PORT
%s REG_NAME
%s ABS_PATH
%s OPAQUE_PART
%s QUERY
%s FRAGMENT
%s ACCEPT

 /* The followings are BNFs generating URI-refernce, taken from RFC 2396. */
URI-reference     ({absoluteURI}|{relativeURI})?("#"{fragment})?
absoluteURI       {scheme}":"({hier_part}|{opaque_part})
relativeURI       ({net_path}|{abs_path}|{rel_path})("?"{query})?
hier_part         ({net_path}|{abs_path})("?"{query})?
opaque_part       {uric_no_slash}{uric}*
uric_no_slash     {unreserved}|{escaped}|";"|"?"|":"|"@"|"&"|"="|"+"|"$"|","
net_path          "//"{authority}{abs_path}?
abs_path          "/"{path_segments}
rel_path          {rel_segment}{abs_path}?
rel_segment       ({unreserved}|{escaped}|";"|"@"|"&"|"="|"+"|"$"|",")+
scheme            {alpha}({alpha}|{digit}|"+"|"-"|".")*
authority         {server}|{reg_name}
reg_name          ({unreserved}|{escaped}|"$"|","|";"|":"|"@"|"&"|"="|"+")+
server            (({userinfo}"@")?{hostport})?
userinfo          ({unreserved}|{escaped}|";"|":"|"&"|"="|"+"|"$"|",")*
hostport          {host}(":"{port})?
host              {hostname}|{IPv4address}
hostname          ({domainlabel}".")*{toplabel}"."?
domainlabel       {alphanum}|{alphanum}({alphanum}|"-")*{alphanum}
toplabel          {alpha}|{alpha}({alphanum}|"-")*{alphanum}
IPv4address       {digit}+"."{digit}+"."{digit}+"."{digit}+
port              {digit}*

path              ({abs_path}|{opaque_part})?
path_segments     {segment}("/"{segment})*
segment           {pchar}*(";"{param})*
param             {pchar}*
pchar             {unreserved}|{escaped}|":"|"@"|"&"|"="|"+"|"$"|","

query             {uric}*

fragment          {uric}*

uric              {reserved}|{unreserved}|{escaped}
reserved          ";"|"/"|"?"|":"|"@"|"&"|"="|"+"|"$"|","
unreserved        {alphanum}|{mark}
mark              "-"|"_"|"."|"!"|"~"|"*"|"'"|"("|")"

escaped           "%"{hex}{hex}
hex               {digit}|[A-Fa-f]

alphanum          {alpha}|{digit}
alpha             {lowalpha}|{upalpha}

lowalpha          [a-z]
upalpha           [A-Z]
digit             [0-9]

%{
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "utility.h"
#include "uri_lexer.h"
#include "uri.h"
%}

%%
<SCHEME>{scheme}":"/"/"|{uric_no_slash} {
  _uri->_scheme = Utility::strdupn(yytext, yyleng - 1);
	if (_uri->_scheme) {
		_length += yyleng;
		yy_push_state(AUTHORITY);
	} else {
		_uri->reinit();
		return -1;
	}
}

<SCHEME>.|\n {
	yyless(0);
	BEGIN REL_PATH;
}

<SCHEME><<EOF>> { BEGIN REL_PATH; }

<REL_PATH>{rel_path} {
	_uri->_path = Utility::strdupn(yytext, yyleng);
	if (_uri->_path) {
		_length += yyleng;
		BEGIN QUERY;
	} else {
		_uri->reinit();
		return -1;
	}
}

<REL_PATH>.|\n {
	yyless(0);
	yy_push_state(AUTHORITY);
}

<REL_PATH><<EOF>>	{ yy_push_state(AUTHORITY); }

 /* Authority and abs_path have conflict! If the following is "//", 
  * we assume that it's an authority; if the following is "/", it's
  * an abs_path. */
<AUTHORITY>"//" {
	yy_pop_state();
	_uri->_authority = new Authority(AT_SERVER);
	if (_uri->_authority) {
		_length += yyleng;
		BEGIN USERINFO;
	} else {
		_uri->reinit();
		return -1;
	}
}

<AUTHORITY>.|\n {
	yyless(0);
	yy_push_state(ABS_PATH);
}

<AUTHORITY><<EOF>> { yy_push_state(ABS_PATH); }

<USERINFO>{userinfo}"@" {
	_uri->_authority->_userinfo = Utility::strdupn(yytext, yyleng - 1);
	if (_uri->_authority->_userinfo) {
		_length += yyleng;
		BEGIN HOST;
	} else {
		_uri->reinit();
		return -1;
	}
}

<USERINFO>.|\n {
	yyless(0);
	BEGIN HOST;
}

<USERINFO><<EOF>>	{ BEGIN HOST; }

<HOST>{host} {
	_uri->_authority->_host = Utility::strdupn(yytext, yyleng);
	if (_uri->_authority->_host) {
		_length += yyleng;
		BEGIN PORT;
	} else {
		_uri->reinit();
		return -1;
	}
}

<HOST>.|\n {
	yyless(0);
	BEGIN REG_NAME;
}

<HOST><<EOF>> { BEGIN REG_NAME; }

<PORT>":"{port} {
	_uri->_authority->_port = Utility::strdupn(yytext + 1, yyleng - 1);
	if (_uri->_authority->_port) {
		_length += yyleng;
		BEGIN REG_NAME;
	} else {
		_uri->reinit();
		return -1;
	}
}

<PORT>.|\n {
	yyless(0);
	BEGIN REG_NAME;
}

<PORT><<EOF>> { BEGIN REG_NAME; }

<REG_NAME>{reg_name} {
  // We have assumed that the authority is a server, but it seems that 
  // we are wrong: it's a reg_name. We should join the userinfo, host 
  // and the port together with this yytext into a reg_name. This case 
  // seldom happens, almost never.
  char* curpos;
	int len = yyleng;

	if (_uri->_authority) {
		if (_uri->_authority->_userinfo) {
			len += strlen(_uri->_authority->_userinfo) + 1;
    }
		if (_uri->_authority->_host) {
			len += strlen(_uri->_authority->_host);
    }
		if (_uri->_authority->_port) {
			len += strlen(_uri->_authority->_port) + 1;
    }
	}

  char* reg_name = new char[len + 1];
	if (reg_name) {
		curpos = reg_name;
		if (_uri->_authority) {
			if (_uri->_authority->_userinfo) {
				memcpy(curpos, _uri->_authority->_userinfo, 
            strlen(_uri->_authority->_userinfo));
				*curpos++ = '@';
			}

			if (_uri->_authority->_host) {
				memcpy(curpos, _uri->_authority->_host, 
            strlen(_uri->_authority->_host));
      }

			if (_uri->_authority->_port) {
				*curpos++ = ':';
        memcpy(curpos, _uri->_authority->_port,
							strlen(_uri->_authority->_port));
			}
		}

		memcpy(curpos, yytext, yyleng);
		*curpos = '\0';

		_uri->_authority->destroy();
		_uri->_authority->reinit(AT_REG_NAME);
		_uri->_authority->_reg_name = reg_name;

		_length += yyleng;
		yy_push_state(ABS_PATH);
  } else {
		_uri->reinit();
		return -1;
	}
}

<REG_NAME>.|\n {
	yyless(0);
	yy_push_state(ABS_PATH);
}

<REG_NAME><<EOF>> { yy_push_state(ABS_PATH); }

<ABS_PATH>{abs_path} {
	yy_pop_state();
	if (YY_START == AUTHORITY) {
		yy_pop_state();
  }
	_uri->_path = Utility::strdupn(yytext, yyleng);
	if (_uri->_path) {
		_length += yyleng;
		BEGIN QUERY;
	} else {
		_uri->reinit();
		return -1;
	}
}

<ABS_PATH>.|\n |
<ABS_PATH><<EOF>> {
	// When encountered an EOF we can not yyless.
	if (yyleng == 1) {
		yyless(0);
  }
	yy_pop_state();

	// The previous state is "AUTHORITY" indicates the URI
  // has NO authority.
	if (YY_START == AUTHORITY) {
		yy_pop_state();
    // The previous state is "SCHEME" indicates the URI
    // HAS a scheme. It's a little confusing.
		if (YY_START == SCHEME) {
			BEGIN OPAQUE_PART;
		} else {
			BEGIN FRAGMENT;
    }
	} else {
		BEGIN QUERY;
  }
}

<OPAQUE_PART>{opaque_part} {
	_uri->_path = Utility::strdupn(yytext, yyleng);
	if (_uri->_path) {
		_length += yyleng;
		BEGIN FRAGMENT;
	} else {
		_uri->reinit();
		return -1;
	}
}

<OPAQUE_PART>.|\n {
	yyless(0);
	BEGIN FRAGMENT;
}

<OPAQUE_PART><<EOF>> { BEGIN FRAGMENT; }

<QUERY>"?"{query}	{
	_uri->_query = Utility::strdupn(yytext + 1, yyleng - 1);
	if (_uri->_query) {
		_length += yyleng;
		BEGIN FRAGMENT;
	} else {
		_uri->reinit();
		return -1;
	}
}

<QUERY>.|\n {
	yyless(0);
	BEGIN FRAGMENT;
}

<QUERY><<EOF>> { BEGIN FRAGMENT; }

<FRAGMENT>"#"{fragment} {
	_uri->_fragment = Utility::strdupn(yytext + 1, yyleng - 1);
	if (_uri->_fragment) {
		_length += yyleng;
		BEGIN ACCEPT;
	} else {
		_uri->reinit();
		return -1;
	}
}

<FRAGMENT>.|\n {
	yyless(0);
	BEGIN ACCEPT;
}

<FRAGMENT><<EOF>>	{ BEGIN ACCEPT; }

<ACCEPT>.|\n {
	yyless(0);
	return _length;
}

<ACCEPT><<EOF>> { return _length; }

<INITIAL>{URI-reference} { return yyleng; }

<INITIAL>.|\n {
	yyless(0);
	return 0;
}

<INITIAL><<EOF>> { return 0; }
%%

_START_MYJFM_NAMESPACE_

RES_CODE UriLexer::yylex(const char* content) {
  BEGIN SCHEME;
  String str(content);
  Istringstream strstr(str);
  switch_streams((Istream*)&strstr);
	_uri->reinit();
  _length = 0;
  if (yylex() <= 0) {
    return S_FAIL;
  } else {
    return S_OK;
  }
}

RES_CODE UriLexer::lex() {
  if (_str == NULL || _uri == NULL) {
    return S_FAIL;
  }

  return yylex(_str);
}

int UriLexer::uri_length() {
	BEGIN INITIAL;
	return yylex();
}

/* Merge two path. It sounds easy but indeed quite troublesome if you take
 * everything into consideration. Core of merging two URIs. The function will
 * allocate memory for you, which is NOT a very good programming style you
 * should not follow. */
RES_CODE UriLexer::path_merge(const char *rel_path, 
    const char *base_path, char **result) {
	Stack<String> stack;
	const char *curpos;
	const char *next_slash;
	int i;

	/* This merging algorithm is different from RFC 2396, which uses string,
	 * while this algorithm uses stack. */

	/* The "base_path" and the "rel_path" are divided into segments and push
	 * all these segments and their length into the stack. If a segment
	 * is ".", ignore it; if a segment is "..", pop one segment out. */
	for (i = 0; i < 2; i++) {
		/* Both "rel_path" and "base_path" can be NULL. */
		if (curpos = base_path) {
			while ((next_slash = strchr(curpos, '/'))) {
				if (strncmp(curpos, "../", next_slash - curpos + 1) == 0) {
					if (stack.size() > 1 || (!stack.empty() && stack.top() != "/")) {
						stack.pop();
					}
				} else if (strncmp(curpos, "./", next_slash - curpos + 1) != 0) {
          String tmp;
          tmp.append(curpos, next_slash - curpos + 1);
					stack.push(tmp);
				}
				curpos = next_slash + 1;
			}
			base_path = rel_path;
		}
	}

	/* This part deals with the "filename", which may be empty, may be "..",
	 * may be ".", and may be something else like "index.html". */
	if (curpos) {
    if (strcmp(curpos, "..") == 0) {
      if (stack.size() > 1 || (!stack.empty() && stack.top() != "/")) {
        stack.pop();
      }
    } else if (strcmp(curpos, ".") != 0) {
      String tmp = curpos;
      stack.push(tmp);
    }
  }

  /* Example:
   * rel_path: "../././../game/../document/rfc/rfc2616.pdf"
   * base_path: "/pub/incoming/./software/linux/nasm.tar.gz",
   * Now the stack is:
   *
   *	+---------------+		<-- stack top
   *	|	rfc2616.pdf   |
   *	+---------------|
   *	|	rfc/          |
   *	|---------------|
   *	|	document/     |
   *	|---------------|
   *	|	incoming/     |
   *	|---------------|
   *	|	pub/          |
   *	|---------------|
   *	|	/	            |
   *	+---------------+		<-- stack base
   *
   * All the information we need to compose the result path has been here.
   */
  
  if (*result != NULL) {
    delete [] *result;
    *result = NULL;
  }

  /* The result path is an "empty path". We should turn it into "no path".
   * "no path" is allowed while "empty path" is illegal. */
  if (stack.empty()) {
    *result = NULL;
  } else {
    String tmp;
    while (!stack.empty()) {
      tmp = stack.top() + tmp;
      stack.pop();
    }
    *result = new char[tmp.length() + 1];
    if (*result) {
      memcpy(*result, tmp.c_str(), tmp.length());
      (*result)[tmp.length()] = '\0';
    } else {
      *result = NULL;
      return S_FAIL;
    }
  }

  return S_OK;
}

RES_CODE UriLexer::uri_merge(Uri* result) {
  if (result == NULL) {
    return S_FAIL;
  }

  result->reinit();
  do {
    // If the relative URI has a scheme, take it; else take the scheme 
    // of the base URI.
    if (_uri->_scheme) {
      result->_scheme = Utility::strdup(_uri->_scheme);
    } else {
      result->_scheme = Utility::strdup(_base_uri->_scheme);
    }
    if (result->_scheme == NULL) {
      break;
    }

    // If the relative URI has a scheme or an authority, take it's 
    // authority; else take the authority of the base URI.
    if (_uri->_authority) {
      result->_authority = _uri->_authority->get_copy();
    } else {
      result->_authority = _base_uri->_authority->get_copy();
    }
    if (result->_authority == NULL) {
      break;
    }

    if (!_uri->_path) {
      result->_path = Utility::strdup(_base_uri->_path);
    } else if (*(_uri->_path) == '/') {
      result->_path = Utility::strdup(_uri->_path);
    } else if ((path_merge(_uri->_path, _base_uri->_path ? 
            _base_uri->_path : "/", &result->_path)) != S_OK) {
      break;
    } else {
      break;
    }
    if (result->_path == NULL) {
      break;
    }

    // Query is taken from relative URI.
    result->_query = Utility::strdup(_uri->_query);

    // Fragment is taken from relative URI.
    result->_fragment = Utility::strdup(_uri->_fragment);

    return S_OK;
  } while (0);

  result->reinit();
  return S_FAIL;
}

// Recombine a URI structure into a URI string. "flags" indicates what
// component(s) would you like to appear in the result string. Note that
// the result string is NOT necessarily a legal URI string (When you mask
// some components) though the second argument has the name "uristr".
RES_CODE UriLexer::uri_combine(String& uristr, int flags, Uri* uri) {
  if (uri == NULL) {
    uri = _uri;
  }

  if (uri == NULL) {
    return S_FAIL;
  }

  uristr = "";

  if ((flags & C_SCHEME) && uri->_scheme) {
    uristr = uristr + uri->_scheme + ":";
  }

  if ((flags & C_AUTHORITY) && uri->_authority) {
    uristr = uristr + "//";

    if (uri->_authority->_type == AT_SERVER) {
      if ((flags & C_USERINFO) && uri->_authority->_userinfo) {
        uristr = uristr + uri->_authority->_userinfo + "@";
      }

      if ((flags & C_HOST) && uri->_authority->_host) {
        uristr = uristr + uri->_authority->_host;
      }

      if ((flags & C_PORT) && uri->_authority->_port) {
        uristr = uristr + ":" + uri->_authority->_port;
      }
    } else if ((flags & C_REG_NAME) && uri->_authority->_reg_name) {
      uristr = uristr + uri->_authority->_reg_name;
    }
  }

  if ((flags & C_PATH) && uri->_path) {
    uristr = uristr + uri->_path;
  }

  if ((flags & C_QUERY) && uri->_query) {
    uristr = uristr + "?" + uri->_query;
  }

  if ((flags & C_FRAGMENT) && uri->_fragment) {
    uristr = uristr + "#" + uri->_fragment;
  }

  return S_OK;
}

_END_MYJFM_NAMESPACE_

