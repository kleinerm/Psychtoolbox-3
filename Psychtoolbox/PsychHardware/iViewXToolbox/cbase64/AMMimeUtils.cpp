#include "StdAfx.h"
#include "AMMimeUtils.h"
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

////////// a small helper function, to do case insensitive search ////////

#define strupos(x, y) (strustr(x, y) != NULL ? strustr(x, y) - x : -1) //char version

char* strustr(char *source, char *s)
{
  //make an uppercase copy af source and s
  char *csource = strdup(source);
  char *cs = strdup(s);
  strupr(csource);
  strupr(cs);
  //find cs in csource...
  char *result = strstr(csource, cs);
  if (result != NULL)
  {
    //cs is somewhere in csource
    int pos = result - csource;
    result = source;
    result += pos;
  }
  //clean up
  free(csource);
  free(cs);
  return result;
}

////////// The "real" decoding stuff //////////

const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define SKIP '\202'
#define NOSKIP 'A'
#define MaxLineLength 76

const char base64map[] =
{
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,  SKIP,  SKIP,   SKIP,  SKIP,  SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,    62,   SKIP,   SKIP,   SKIP,    63,
      52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,    0 ,    1 ,    2 ,    3 ,    4 ,    5 ,    6 ,
      7 ,    8 ,    9 ,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP
};

const char hexmap[] = {
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
      0 ,    1 ,    2 ,    3 ,    4 ,    5 ,    6 ,    7 ,
      8 ,    9 ,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,    10,    11,    12,    13,    14,    15,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP
};

const char QpEncodeMap[] = {
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   NOSKIP,   SKIP,   SKIP,   NOSKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     NOSKIP,   SKIP,   SKIP,   SKIP,   SKIP,   NOSKIP,   NOSKIP,   NOSKIP,
     NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,
     NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,
     NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   SKIP,   NOSKIP,   NOSKIP,
     SKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,
     NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,
     NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,
     NOSKIP,   NOSKIP,   NOSKIP,   SKIP,   SKIP,   SKIP,   SKIP,   NOSKIP,
     SKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,
     NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,
     NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,   NOSKIP,
     NOSKIP,   NOSKIP,   NOSKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,
     SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP,   SKIP
};

char* MimeDecodeMailHeaderField(char *s)
{
  if (s == NULL) return s;
  //if s ends with a crlf pair, remove it...
  if (s[strlen(s) - 2] == '\r')
  {
    s[strlen(s) - 2] = '\0';
  }
  //remove starting spaces...
  char *s1 = s;
  char *rest = NULL;
  char *start = NULL;
  while (*s1 == ' ') s1++;

  if (strupos(s1, "=?") > 0)
  {
    int startend = strupos(s1, "=?");
    start = (char*)_alloca((startend + 1) * sizeof(char));
    strncpy(start, s, startend);
    start[startend] = '\0';
    s1 += startend;
  }

  if (strupos(s1, "=?") == 0)
  {
    //we have to decode
    //if there are text after "?=" we have to move it to another string, so the decoder don't delete it...
    //if isn't qp-encoded and the first char is encoded, we have to move past the "?=", to find the next...

    int plainpos = strupos(s1, "Q?=");
    if (plainpos > 0)
    {
      plainpos += 3;
      char *m = s1 + plainpos;
      plainpos += strupos(m, "?=");
    }
    else
    {
      plainpos = strupos(s1, "?=");
    }

    if (plainpos > 1)
    {
      char *mid = s1 + plainpos + 2;
      s1[plainpos] = '\0';
      if (strlen(mid) > 0)
      {
        //we have some text we need to "cut"
        rest = (char*)_alloca(strlen(mid) + sizeof(char));
        strcpy(rest, mid);
      }
    }
    //do the decode...
    char *subj = s1;
    char *decodedText;
    if (strupos(s1, "?Q?") > 0)
    {
      //it's quoted printable
      //find the right "starting point" in the string...
      int pos =  strupos(s1, "?Q?");
      s1 += pos;
      if (strlen(s1) < 4) return s; //this should never happend, if the sender is RFC compliant...
      s1 += 3;
      CQPUtils qp;
      decodedText = qp.Decode(s1);
    }
    if (strupos(s1, "?B?") > 0)
    {
      //it's base64
      //find the right "starting point" in the string...
      int pos =  strupos(s1, "?B?");
      s1 += pos;
      if (strlen(s1) < 4) return s; //this should never happend, if the sender is RFC compliant...
      s1 += 3;
      CBase64Utils bu;
      int sLen = strlen(s1);
      decodedText = bu.Decode(s1, &sLen);
    }
    //put the decoded text back in s, and append rest...

    int alloclen = strlen(decodedText) + 1;
    if (start != NULL) alloclen += strlen(start);
    if (rest != NULL) alloclen += strlen(rest);
    alloclen *= sizeof(char);
    s = (char*)realloc(s, alloclen);
    s[0] = '\0';
    if (start != NULL)
    {
      strcat(s, start);
    }
    strcat(s, decodedText);
    if (rest != NULL)
    {
      strcat(s, rest);
    }
    free(decodedText);
  }
  return s;
}

CBase64Utils::CBase64Utils()
{
  ErrorCode = 0;
}

CBase64Utils::~CBase64Utils()
{
}

char* CBase64Utils::Encode(char *input, int bufsize)
{
  int alsize = ((bufsize * 4) / 3);
  char *finalresult = (char*)calloc(alsize + ((alsize / 76) * 2) + (10 * sizeof(char)), sizeof(char));
  int count = 0;
  int LineLen = 0;
  char* fresult = finalresult;
  char *s = input;
  int tmp = 0;
  //let's step through the buffer and encode it...
  while (count <= bufsize)
  {
    if (count % 3 == 0 && count != 0)
    {
      tmp >>= 8;
      tmp &= 0xFFFFFF;
      //we have 4 new b64 chars, add them to finalresult
      int mid = tmp;
      mid >>= 18;
      mid &= 0x3F;
      *(fresult++) = base64chars[mid];
      LineLen++;
      mid = tmp;
      mid >>= 12;
      mid &= 0x3F;
      *(fresult++) = base64chars[mid];
      LineLen++;
      mid = tmp;
      mid >>= 6;
      mid &= 0x3F;
      *(fresult++) = base64chars[mid];
      LineLen++;
      mid = tmp;
      mid &= 0x3F;
      *(fresult++) = base64chars[mid];
      LineLen++;
      //reset tmp
      tmp = 0;
      //should we break the line...
      if (LineLen >= MaxLineLength)
      {
        *(fresult++) = '\r';
        *(fresult++) = '\n';
        LineLen = 0;
      }
      if (bufsize - count < 3)
        break;
    }
    unsigned char mid = (256 - (0 - *s));
    tmp |= mid;
    tmp <<= 8;
    count++;
    s++;
  }
  //do we have some chars left...
  int rest = (bufsize - count) % 3;
  if (rest != 0)
  {
    tmp = 0;
    int i;
    for (i = 0; i < 3; i++)
    {
      if (i < rest)
      {
        unsigned char mid = (256 - (0 - *s));
        tmp |= mid;
        tmp |= *s;
        tmp <<= 8;
        count++;
        s++;
      }
      else
      {
        tmp |= 0;
        tmp <<= 8;
      }
    }
    tmp >>= 8;
    tmp &= 0xFFFFFF;
    //we have some new b64 chars, add them to finalresult
    int mid = tmp;
    if (rest >= 1)
    {
      mid >>= 18;
      mid &= 0x3F;
      *(fresult++) = base64chars[mid];
      mid = tmp;
      mid >>= 12;
      mid &= 0x3F;
      *(fresult++) = base64chars[mid];
    }
    if (rest >= 2)
    {
      mid = tmp;
      mid >>= 6;
      mid &= 0x3F;
      *(fresult++) = base64chars[mid];
    }
    if (rest >= 3)
    {
      mid = tmp;
      mid &= 0x3F;
      *(fresult++) = base64chars[mid];
    }
    for (int c = 3; c > rest; c--)
    {
      *(fresult++) = '=';
    }
  }
  return finalresult;
}

char* CBase64Utils::Decode( char *input, int *bufsize)
{
  int std = 0, count = 1, resultlen = 0;
  char *finalresult = (char*)calloc(*bufsize + sizeof(char), sizeof(char));
  char *s = input, *result = finalresult;
  while (*s != '=' && count <= *bufsize)
  {
    //check to see if it's a legal base64 char...
    while (base64map[*s] == SKIP)
    {
      if (*s != '\r' && *s != '\n')
      {
        //bad char...
        //we might want to tell the user that there was error in the encoded data...
        ErrorCode = 1;
      }
      s++;
      (*bufsize)--;
      if (count >= *bufsize)
      {
        break;
      }
    }
    //add the base64 char to std...
    std |= base64map[*(s++) & 0xFF];
    std <<= 6;
    if (count % 4 == 0) //we have 3 more real chars...
    {
      //put std in the next 3 chars in finalresult
      int tmp;
      std >>= 6;
      tmp = std;
      tmp >>= 16;
      tmp &= 0xFF;
      *(result++) = (tmp);
      tmp = std;
      tmp >>= 8;
      tmp &= 0xFF;
      *(result++) = (tmp);
      tmp = std;
      tmp &= 0xFF;
      *(result++) = (tmp);
      std = 0; //empty std
      resultlen += 3;
    }
    count++;
  }
  //find and decode the remaining chars, if any...
  count--;
  if (count % 4 != 0)
  {
    //we have some remaining chars, now decode them...
    for (int i = 0; i < 4 - (count % 4); i++)
    {
      std <<= 6;
      resultlen++;
    }
    int tmp;
    std >>= 6;
    tmp = std;
    tmp >>= 16;
    tmp &= 0xFF;
    *(result++) = (tmp);
    tmp = std;
    tmp >>= 8;
    tmp &= 0xFF;
    *(result++) = (tmp);
    tmp = std;
    tmp &= 0xFF;
    *(result++) = (tmp);
  }
  *bufsize = resultlen;
  return finalresult;
}

CQPUtils::CQPUtils()
{
  ErrorCode = 0;
}

CQPUtils::~CQPUtils()
{
}

char* CQPUtils::Decode(char *input)
{
  char *s = input;
  char *finalresult = (char*)calloc(strlen(input) + sizeof(char), sizeof(char));
  char *result = finalresult;
  while (*s != '\0') //loop through the entire string...
  {
    if (*s == '=') //woops, needs to be decoded...
    {
      for (int i = 0; i < 3; i++) //is s more than 3 chars long...
      {
        if (s[i] == '\0')
        {
          //error in the decoding...
          ErrorCode = 1;
          return finalresult;
        }
      }
      char mid[3];
      s++; //move past the "="
      //let's put the hex part into mid...
      bool ok = true;
      for (i = 0; i < 2; i++)
      {
        if (hexmap[s[i]] == SKIP)
        {
          //we have an error, or a linebreak, in the encoding...
          ok = false;
          if (s[i] == '\r' && s[i + 1] == '\n')
          {
            s += 2;
            //*(result++) = '\r';
            //*(result++) = '\n';
            break;
          }
          else
          {
            //we have an error in the encoding...
            ErrorCode = 1;
            //s--;
          }
        }
        mid[i] = s[i];
      }
      //now we just have to convert the hex string to an char...
      if (ok)
      {
        s += 2;
        int m = hexmap[mid[0]];
        m <<= 4;
        m |= hexmap[mid[1]];
        *(result++) = m;
      }
    }
    else
    {
      if (*s != '\0') *(result++) = *(s++);
    }
  }

  return finalresult;
}

#define BufAdd 10 //the size that we expands the buffer with...

char* CQPUtils::ExpandBuffer(char *buffer, int UsedSize, int *BufSize, bool SingleChar)
{
  //should we expand the buffer now...
  int AddVal;
  if (SingleChar) AddVal = 3;
  else AddVal = 5;
  if (UsedSize >= *BufSize - AddVal)
  {
    //expand the buffer
    *BufSize += BufAdd;
    return (char*)realloc(buffer, *BufSize * sizeof(char));
  }
  return buffer;
}

char* CQPUtils::Encode(char *input)
{
  int BufSize = strlen(input) + BufAdd; //size of the result buffer
  int UsedSize = 0; //used space in result buffer
  int LineLen = 0; //length of the current line...
  char *finalresult = (char*)calloc(BufSize, sizeof(char)); //the result buffer
  char *fresult = finalresult;
  char *s = input;
  while (*s != '\0')
  {
    //convert the signed char to an unsigned char...
    unsigned char mid = (256 - (0 - *s));
    //should we reset the linelength...
    if (*s == '\n')
      LineLen = 0; //we are starting on a new line...
    if (QpEncodeMap[mid] == SKIP)
    {
      //we need to encode this char...
      //is the line too long...
      if (LineLen >= MaxLineLength - 4)
      {
        //wrap the line...
        finalresult = ExpandBuffer(finalresult, UsedSize, &BufSize, false);
        *(fresult++) = '=';
        *(fresult++) = '\r';
        *(fresult++) = '\n';
        UsedSize += 3;
        LineLen = 0;
      }
      //check buffersize...
      finalresult = ExpandBuffer(finalresult, UsedSize, &BufSize, false);
      //add the hex value for the char...
      char mids[3];
      itoa(mid, mids, 16);
      strupr(mids);
      *(fresult++) = '=';
      *(fresult++) = mids[0];
      *(fresult++) = mids[1];
      UsedSize += 3;
      LineLen += 2;
      s++;
    }
    else
    {
      //just add the char...
      //is the line too long...
      if (LineLen >= MaxLineLength - 4)
      {
        //wrap the line...
        finalresult = ExpandBuffer(finalresult, UsedSize, &BufSize, false);
        *(fresult++) = '=';
        *(fresult++) = '\r';
        *(fresult++) = '\n';
        UsedSize += 3;
        LineLen = 0;
      }
      //check buffersize...
      finalresult = ExpandBuffer(finalresult, UsedSize, &BufSize);
      UsedSize++;
      LineLen++;
      *(fresult++) = *(s++);
    }
  }
  *(fresult++) = '\0';
  return finalresult;
}