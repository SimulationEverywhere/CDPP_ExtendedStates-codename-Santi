/*******************************************************************
*
*  DESCRIPTION: class TokenIterator
*
*  AUTHOR:      Alejandro López 
*
*  EMAIL:       aplopez@dc.uba.ar
*
*  DATE:        10/6/2001
*
*******************************************************************/
#include <string>
#include "tokit.h"

#define TOKEN_SEPARATOR     ':'
#define ASSIGN_CHAR         '='

using namespace std;

// When reading a token from the istream_iterator, we receive a
// string with no space character (32). That string is a list_of_tokens:
//
// list_of_tokens: ':'                 |
//                 token               | 
//                 ':' list_of_tokens  |
//                 list_of_tokens ':'

 
// prefix ++
TokenIterator& TokenIterator::operator++()
{
    int pos;

    if (ended)
        return *this;
    
    // check that buffer is not empty
    if (buffer.length() == 0) {
        // read next group of tokens from istream
        if (base == base_stream()) {
            ended = true;
            current = "";
            return *this;
        } else {
            buffer = *(base++);
        }
    }

    // read next token from buffer
    if (buffer[0] == TOKEN_SEPARATOR) {
        // The first character is a colon
	
	// Is this part of an assignation token?
	if (buffer.length() > 1 && buffer[1] == ASSIGN_CHAR) {
		// Is an assignation
		current = buffer.substr(0, 2);
		buffer = buffer.substr(2);
	} else {
		// Is a separator
	        current = buffer.substr(0, sizeof(TOKEN_SEPARATOR));
        	buffer = buffer.substr(sizeof(TOKEN_SEPARATOR));
	}
    } else if ((pos = findInString(buffer, TOKEN_SEPARATOR)) == -1) {
        // There is no colon in buffer
        current = buffer;
        buffer = "";
    } else {
        // There is at least one colon in buffer
        current = buffer.substr(0, pos);
        buffer = buffer.substr(pos);
    }
    
    return *this;
}

// postfix ++
TokenIterator TokenIterator::operator++(int)
{
    TokenIterator t = *this;

    ++*this;
        
    return t;
}

bool TokenIterator::operator==(const TokenIterator& a)
{
    return (a.ended == this->ended      && 
            a.current == this->current  &&
            a.base == this->base);
}

int TokenIterator::findInString(string &str, char c)
{
    int pos = 0;
    int len = str.length();
    
    while (pos < len) {
        if (str[pos] == c)
            return pos;
         pos++;
    }
    
    return -1;
}

