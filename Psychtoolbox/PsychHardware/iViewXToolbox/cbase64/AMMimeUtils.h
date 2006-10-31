//decodes a single line from a header-field in a mail...
char* MimeDecodeMailHeaderField(char *s);

//class to handle all base64 stuff...
class CBase64Utils
{
private:
  int ErrorCode;
public:
  int GetLastError() {return ErrorCode;};
	CBase64Utils();
	~CBase64Utils();
  char* Decode(char *input, int *bufsize); //caller must free the result, bufsize holds the decoded length
  char* Encode(char *input, int bufsize); //caller must free the result, bufsize is the length of the input buffer
};

//class to handle quoted-printable stuff
class CQPUtils
{
private:
  char* ExpandBuffer(char *buffer, int UsedSize, int *BufSize, bool SingleChar = true);
  int ErrorCode;
public:
  int GetLastError() {return ErrorCode;};
	char* Decode(char *input); //caller must free the result
  char* Encode(char *input); //caller must free the result
	CQPUtils();
	~CQPUtils();
};
