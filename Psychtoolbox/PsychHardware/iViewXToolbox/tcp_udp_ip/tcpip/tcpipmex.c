/**********************************************************


  MEX file for tcpip toolbox version Candidate 1.2 2000-04-14
  for MATLAB 5.x

  Copyrigtht (C) Peter Rydesäter 1998 - 2000, Mitthögskolan, SWEDEN
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


  Notes for Unix implementation
  Compile this with:

  mex -O tcpipmex.c


  Notes for Windows implementation

  Compile this with:
  mex -O tcpipmex.c ws2_32.lib -DWIN32



  Please, send us an e-mail if you use this and/or improve it.

  Peter Rydesäter                     Mario Bergeron
  Mitthögskolan                      LYR Signal Processing
  Östersund, Sweden                  Québec, Canada
  e-mail: Peter.Rydesater@ite.mh.se  e-mail: Mario.Bergeron@lyre.qc.ca

**********************************************************/


/******* GENERAL DEFINES *********/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******* WINDOWS ONLY DEFINES *********/
#ifdef WIN32
//#include <windows.h>
#include <winsock2.h>
#define close(s) closesocket(s)
#define nonblockingsocket(s) {  unsigned long ctl = 1;  ioctlsocket( s, FIONBIO, &ctl ); }
#define s_errno WSAGetLastError()
#define EWOULDBLOCK WSAEWOULDBLOCK
#define usleep(a) Sleep(a)

/******* NON WINDOWS DEFINES *********/
#else
#include <errno.h>
#define s_errno errno // ?? Is this OK ??
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#define nonblockingsocket(s) fcntl(s,F_SETFL,O_NONBLOCK)
#include <signal.h>
#endif

/* DEFINE SIGPIPE and SIG_DFL if its not defined */
#ifndef WIN32

#ifndef SIGPIPE
#define SIGPIPE 13
#endif

#ifndef SIG_DFL
#define SIG_DFL (void(*)())0
#endif

#ifndef INADDR_NONE
#define INADDR_NONE (-1)
#endif 
#endif

/* Include header file for matlab mex file functionality */
#include "mex.h" 


/********** DEFINES reletad to tcpipmex own functionality *****************/
 
/*   Set debuging on/off   */
#define debug_view_ipi_status(X)   __debug_view_ipi_status(X)

#define MAX_IPI         100       /* Maximum number of simultanius tcpip connections.*/
#define NAMEBUFF_LEN    100
#define MINBUFFSIZE     1000      /* readbuffer will shrink to this size if datalength
                                     is smaller. */
#define CBNAMELEN       30

#define IPI_FREE         -1       /* Find a new free struct in struct array */
#define IPI_CURRENT      -2       /* DO NOT USE! No longer valid .... */

#define TCPIP_BACKLOG    10       /* How many pending connections queue will hold */


/* Different status of a ipi_info struct handles a file descriptor    */
#define STATUS_FREE       -1 
#define STATUS_NOCONNECT   0
#define STATUS_SERVSOCKET  1
#define STATUS_CLIENT      10
#define STATUS_SERVER      12

/* Structure that hold all information about a tcpip connection. */
struct ip_info
{
    int fid;
    double writetimeout;
    double readtimeout;
    struct sockaddr_in remote_addr;
    struct hostent *he;
    int status;       /* STATUS_... FREE, NOCONNECT, SERVER, CLIENT ... */
    char *buff;       /* Pointer to buffert. */
    int bufflen;      /* Length of buffert. */
    int buffdatalen;  /* Length used of buffer for data storage.*/
    char callback[CBNAMELEN+1];
};

/* Global list with static length of connections structures holding info about current connection */
struct ip_info ipi[MAX_IPI];


/* Some global variables that holds values from one to an another matlab call to the tcpipmex */
int ipi_index=0;                   /* Current index possition for list of handlers */ 
unsigned long mex_call_counter=0;  /* Counter that counts how many calls that have been done to tcpipmex */
int ret_args=0;                    /* Global variable that holds number of matlab return argumens returned */
void *matlab_signal_handler;       /* Holds matlabs default signal handler... */

/* Declaration of all sub functions */
int move_ipi(int idx);
void my_mexReturnValue(int nlhs,mxArray *plhs[],double val);
void my_mexReturnString(int nlhs,mxArray *plhs[],const char *str);
void my_mexReturnCharArray(int nlhs, mxArray *plhs[], const char *buff,int bufflen);
void CopymxCharArray2Buff(const mxArray *array,char *buff,int bufflen);
void CleanUpMex(void);
int closeall(void);
int tcpipserv(int port);
void init_ipi(int fid,int server);
void close_ipi();
void read2buff(int len);
void removefrombuff(int len);
void *myrealloc(void *ptr,int newsize);
void get_sigpipe_handler(int sig);
void get_sigpipe_on(void);
void get_sigpipe_off(void);
int writedata(char*,int);
int tcpipsocket(int port);
int tcpiplisten(int sockfid);
int tcpipserver(int);
void __debug_view_ipi_status(char *msg);

/* Declaration of implemetation of perror for windows */
#ifdef WIN32
void perror( char *context );
#endif


/*****************************************************************/
/*                                                               */
/*    ----Main function that is called from matlab--------       */
/*                                                               */

void mexFunction(
    int           nlhs,           /* number of expected outputs */
    mxArray       *plhs[],        /* array of pointers to output arguments */
    int           nrhs,           /* number of inputs */
    const mxArray *prhs[]         /* array of pointers to input arguments */
)
{
  int fun;
  struct in_addr addr;

  /* Initialization on first call to the mex file */
  if(mex_call_counter==0)
  {
//      int i;
#ifdef WIN32
      {
	  WORD wVersionRequested;
	  WSADATA wsaData;
	  int wsa_err;
	  
	  wVersionRequested = MAKEWORD( 2, 0 );
	  wsa_err = WSAStartup( wVersionRequested, &wsaData );
	  if ( wsa_err )
	  {
	      mexErrMsgTxt("Error starting WINSOCK32.");
	      return;
	  }
      }
#endif
      mexAtExit(CleanUpMex);
      /* Init all to free */
      for(ipi_index=0;ipi_index<MAX_IPI;ipi_index++)
	  init_ipi(-1,STATUS_FREE);
      ipi_index=0;
  }

  mex_call_counter++;
  ret_args=0;

  debug_view_ipi_status("ENTER_MEX");

  if(nrhs<2) 
      mexErrMsgTxt("You must specify at least two arguments!");

  if(mxGetM(prhs[0])*mxGetN(prhs[0])!=1)
      mexErrMsgTxt("Error on first argument! should be a function selection number.");
  fun=(int)mxGetScalar(prhs[0]);


  /* ---CLOSE ALL----  */
  if(fun<0)
  {
    closeall();
    return;
  }

  /* Find given handel */
  {
      int idx;
      if(mxGetM(prhs[1])*mxGetN(prhs[1])!=1)
	  mexErrMsgTxt("Error on second argument. Specify handler as scalar!");
      idx=(int)mxGetScalar(prhs[1]);
      if(move_ipi(idx)==0)
	  mexErrMsgTxt("Unknown handler!");
  }
  debug_view_ipi_status("IPI_MOVED!!");  
  switch(fun)
  {


/* ---CLOSE--- */
  case 0:
      {   
	  if(ipi[ipi_index].fid<0)   /* Already closed?? */
	      mexPrintf("Cant close already closed fid.\n");
	  else
	      close_ipi();
	  break;
      }

/* ---OPEN AS CLIENT--- */
  case 1:
      {
	char hostname[NAMEBUFF_LEN+1];
	int len;
	int port;

   debug_view_ipi_status("O1");
	if(ipi[ipi_index].fid>=0)
	  mexErrMsgTxt("This handler is already open !!??");

   debug_view_ipi_status("O2");
	if(nrhs<3)
	    mexErrMsgTxt("Wrong number of arguments for OPEN!");
	if(mxIsChar(prhs[2])==0 || mxIsNumeric(prhs[3])==0)
	    mexErrMsgTxt("Wrong argument datatype for OPEN!");

	len=mxGetM(prhs[2])*mxGetN(prhs[2])+1;
	if(len>=NAMEBUFF_LEN)
	    mexErrMsgTxt("To long hostname!");

	mxGetString(prhs[2], hostname, len);

	port=(int)mxGetScalar(prhs[3]);
   debug_view_ipi_status("O3");
//	ipi[ipi_index].he=gethostbyname(hostname);
   debug_view_ipi_status("O4");

	if(ipi[ipi_index].he!=NULL)
	    addr = *((struct in_addr *)ipi[ipi_index].he->h_addr);
	else
	{
	    /* Can't lookup hostname, try IP address */
	    addr.s_addr=inet_addr(hostname);
	    if (addr.s_addr==INADDR_NONE)
	    {
		my_mexReturnValue(nlhs,plhs,-1);	
		break;
	    }
	}
   debug_view_ipi_status("O5");
	ipi[ipi_index].fid=socket(AF_INET, SOCK_STREAM, 0);
   debug_view_ipi_status("O5");

	if(ipi[ipi_index].fid== IPI_FREE)
	{
	    /* Can't make socket for connnection to remote host. */
	    my_mexReturnValue(nlhs,plhs,-1);
	    break;
	}
   debug_view_ipi_status("O6");

	ipi[ipi_index].remote_addr.sin_family=AF_INET;
	ipi[ipi_index].remote_addr.sin_port=htons(port);
	ipi[ipi_index].remote_addr.sin_addr=addr;
	ipi[ipi_index].remote_addr.sin_family=AF_INET;
	memset(&ipi[ipi_index].remote_addr.sin_zero, 0,8);
	if(connect(ipi[ipi_index].fid,(struct sockaddr *)&ipi[ipi_index].remote_addr,
		   sizeof(struct sockaddr)) == -1)
	{
	    /*Can't connect to remote host. */
	    my_mexReturnValue(nlhs,plhs,-1);
	    close_ipi();
	    break;
	}
   debug_view_ipi_status("O7");
	init_ipi(ipi[ipi_index].fid,STATUS_CLIENT);
	nonblockingsocket(ipi[ipi_index].fid); /* Non blocking read! */
   debug_view_ipi_status("O8");
	my_mexReturnValue(nlhs,plhs,ipi_index);
	break;
      }

/* ---- WRITESTRING ---- */
  case 2:
      {
	int retval=100000;
	int sentlen=0;
	char *buff;
	int len;
	
	if(nrhs<3)
	  mexErrMsgTxt("You must specify string to write as third argument!");
	if(mxIsChar(prhs[2])==0)
	  mexErrMsgTxt("Write string must be array of chars!");
	len=mxGetM(prhs[2])*mxGetN(prhs[2]);
	if(len==0)
	{
          my_mexReturnValue(nlhs,plhs,0);
	  break;
	}
	if((buff=(char *)mxMalloc(len))==NULL)
	  mexErrMsgTxt("Running out of memory! Can't send string!");
	CopymxCharArray2Buff(prhs[2],buff,len);
	sentlen=writedata(buff,len);
	my_mexReturnValue(nlhs,plhs,sentlen);
	mxFree(buff);
	break;
      }
      break;

/* ---- READSTRING ---- */
  case 3:
      {
	  int readlen;
	  if(nrhs<3)
	      mexErrMsgTxt("You must specify maximum of characteer to read!");
	  readlen=(int)mxGetScalar(prhs[2]);
	  read2buff(readlen);
	  if(readlen>ipi[ipi_index].buffdatalen)
	      readlen=ipi[ipi_index].buffdatalen;
	  my_mexReturnCharArray(nlhs,plhs,ipi[ipi_index].buff,readlen);
	  removefrombuff(readlen);
      }
      break;


/* ---- READSTRING UNTIL (READLINE) ---- */
  case 4:
      {
	int i;
	int readlen;
	int flag=0;
	int termchar1=13;  /* Two default string termination characters */
	int termchar2=10;

	if(nrhs<3)
	  mexErrMsgTxt("You must specify maximum of characters to read!");

	if(nrhs>3) 
	  termchar1=termchar2=(int)mxGetScalar(prhs[3]);
	if(nrhs>4) 
	  termchar2=(int)mxGetScalar(prhs[4]);
	readlen=(int)mxGetScalar(prhs[2]);
	read2buff(readlen);

	for(i=0; i<readlen, i<ipi[ipi_index].buffdatalen; i++)
	{
	    if(ipi[ipi_index].buff[i]==termchar1 || ipi[ipi_index].buff[i]==termchar2)
	    {
	      my_mexReturnCharArray(nlhs,plhs,ipi[ipi_index].buff,i+1);
	      removefrombuff(i+1);
	      break;
	    }
	}
	if(readlen==ipi[ipi_index].buffdatalen)                 /*Is read buffer full? */
	{
	  /* then wrap line...*/
	  my_mexReturnCharArray(nlhs,plhs,ipi[ipi_index].buff,ipi[ipi_index].buffdatalen);
	  removefrombuff(ipi[ipi_index].buffdatalen); 
        }
	else
	  /* else return empty*/ 
	  my_mexReturnCharArray(nlhs,plhs,"",0);       
      }
      break;



/* ----- "VIEW" INCOMMING DATA, (NOT DELETE FROM BUFFER)------ */
  case 5:
      {
	  int readlen;
	  readlen=(int)mxGetScalar(prhs[2]);
	  read2buff(readlen);
	  if(readlen>ipi[ipi_index].buffdatalen)
	      readlen=ipi[ipi_index].buffdatalen;
	  my_mexReturnCharArray(nlhs,plhs,ipi[ipi_index].buff,readlen);
      }
      break;


/* --- RETURN STATUS --- */
  case 6:
      my_mexReturnValue(nlhs,plhs,ipi[ipi_index].status);
      break;


/* --- SET_CALLBACK_FUNCTION --- */
  case 7:
      {
	if(nrhs<3)
	  mexErrMsgTxt("You must specify a string as callback function name!");
	if(mxIsChar(prhs[2])==0)
	  mexErrMsgTxt("You must specify a string as callback function name!");
	mxGetString(prhs[2],ipi[ipi_index].callback, CBNAMELEN);
	break;
      }
      break;



/* --- EXECUTE_CALLBACK_FUNCTION --- */
  case 9:
      {

	  /*.............

	    Type a lote here

	    ................*/

      }
      break;


/* ---- OPEN_AS_SERVER  ----*/          /* OLD!!! Remove this ???? */ 
  case 10:
      {
	  int port;
	  int idx;
	  if(ipi[ipi_index].fid>=0) 
	      mexErrMsgTxt("already open??? Always specify handler -1 when open a new!");
	  if(nrhs<3)
	      mexErrMsgTxt("Wrong number of arguments for OPEN_AS_SERVER!");
	  port=(int)mxGetScalar(prhs[2]);
	  idx=tcpipserver(port);
	  my_mexReturnValue(nlhs,plhs,idx);
      }
      break;



/* ---- OPEN_SERVER_SOCKET ---*/
  case 20:
     {
	 int fd;   /* File descriptor for socket! */
	 int port;
	 if(ipi[ipi_index].status!=STATUS_NOCONNECT) 
	     mexErrMsgTxt("already open??? Always specify handler -1 when open a new!");
	 if(nrhs<3)
	     mexErrMsgTxt("Wrong number of arguments. specify port number.");
	 port=(int)mxGetScalar(prhs[2]);
	 fd=tcpipsocket(port);
	 if(fd>=0)
	 {
	     init_ipi(fd,STATUS_SERVSOCKET);
  	     my_mexReturnValue(nlhs,plhs,ipi_index);
	 }
	 else
	 {
	     init_ipi(-1,STATUS_FREE);
  	     my_mexReturnValue(nlhs,plhs,-1);
	 }
     }
     break;
     
/* ---- OPEN_CONNECTED TO SERVER ---   Listen for calls and return */
  case 21:
     {
	 int conn_fd;
//	 int sockfd;
	 if(ipi[ipi_index].status!=STATUS_SERVSOCKET)
	     mexErrMsgTxt("Its not a open socket you tries listens to...");
	 conn_fd=tcpiplisten(ipi[ipi_index].fid);
	 if( conn_fd >=0)
	 {
             /* Find a new free ipi struct for the new connection.... */
	     move_ipi(IPI_FREE);
	     init_ipi(conn_fd,STATUS_SERVER);
  	     my_mexReturnValue(nlhs,plhs,ipi_index);
	 }
	 else
	 {
  	     my_mexReturnValue(nlhs,plhs,-1);
	 }
     }
     break;

  default:
      mexErrMsgTxt("Unknown number for called function! Not implemented in this verion?");
      break;
      
  }
  debug_view_ipi_status("EXIT");
}


/*******************************************************************      */
/* Function Creating socket for later listning for connections
   on specified port. Returns socket fid after */

int tcpipsocket(int port)
{
//    int new_fd;                   /* new connection on new_fd */
	int sockfd;
    struct sockaddr_in my_addr;    /* my address information */
//    int sin_size;
    const int on=1;
    const int off=0;
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	return -1;
    }
    my_addr.sin_family = AF_INET;         /* host byte order */
    my_addr.sin_port = htons(port);       /* short, network byte order */
    my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
    memset(&(my_addr.sin_zero),0, 8);        /* zero the rest of the struct */

    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(const char *)&on,sizeof(on));

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) \
	== -1) {
	close(sockfd);
	return -1;
    }
   nonblockingsocket(sockfd);
   if (listen(sockfd, TCPIP_BACKLOG) == -1) {
	close(sockfd);
	return -1;
    }
    return sockfd;
}

/* Listen to socket and returns connection if their is one...
   else it returns -1 */

int tcpiplisten(int sockfd)
{
    int new_fd;
    int sin_size;
    sin_size = sizeof(struct sockaddr_in);
    if ((new_fd = accept(sockfd, (struct sockaddr *)&ipi[ipi_index].remote_addr, \
			 &sin_size)) == -1)
    {
	return -1;
    }
    setsockopt(new_fd,SOL_SOCKET,SO_KEEPALIVE,(void *)1,0); /* realy needed?*/
    return new_fd;
}


/*******************************************************************************/
/* Checks that given index is valid index and set current index, "ipi_index"   */
/* to that point. If index is IPI_FREE (-1) then is seatch done for a free one */
/* Returns 1 On success. 0 on error                                            */

int move_ipi(int idx)
{
    if(idx>=0)
    {
	ipi_index=idx;
	if(ipi[ipi_index].status==STATUS_FREE) 
	    mexErrMsgTxt("No valid handler! already closed?\n");
	return 1;
    }    
    debug_view_ipi_status("START MOVE");
    if(idx==IPI_CURRENT)
	mexErrMsgTxt("Current fid (-2) no longer supported!\n");
    if(idx>=MAX_IPI)
	mexErrMsgTxt("Unvalid value of handler!\n");
    if(idx==IPI_FREE)    /* Move ipi_index until it find a free non used struct */
    {
	for(ipi_index=0;ipi_index<MAX_IPI;ipi_index++)
	{
	    debug_view_ipi_status("STEP MOVE"); 
	    if(ipi[ipi_index].status==STATUS_FREE)
	    {
		ipi[ipi_index].status=STATUS_NOCONNECT;
		return 1;
	    }
	}
	mexErrMsgTxt("To many open connection! Forgot to close?\n");
    }
    if(idx<0)
	mexErrMsgTxt("Unvalid value of handler!\n");
    return 0;
}


/*******************************************************************************/
/* Puts double scalar in matlab variable in the array of return argument for mex*/

void my_mexReturnValue(int nlhs, mxArray *plhs[],double val)
{
    if((ret_args>nlhs) && (ret_args>1))
    {
	printf("No use for all return arguments!\n");
	return;
    }
    plhs[ret_args]=mxCreateDoubleMatrix(1,1,mxREAL);
    if(plhs[ret_args]==NULL)
	mexErrMsgTxt("Matrix creation error! Lack of memory?");
    else
    {
	*mxGetPr(plhs[ret_args])=val;
	ret_args++;
    }
}


/******************************************************************************/
/* Puts string as matlab char char variable in array of return argument for mex*/

void my_mexReturnString(int nlhs, mxArray *plhs[], const char *str)
{
    if(ret_args>nlhs & ret_args>1)
	return;
    plhs[ret_args]=mxCreateString(str);
    if(plhs[ret_args]==NULL)
	mexErrMsgTxt("String creation error! Lack of memory?");
    else
	ret_args++;
}


/******************************************************************************/
/* Puts part of string as char variable in array of return arguments from mex */

void my_mexReturnCharArray(int nlhs, mxArray *plhs[], const char *buff,int bufflen)
{
    int ndim = 2, dims[2];
    mxArray  *array_ptr;
    mxChar  *pr;
    int      c;
    unsigned char *ubuff;  /* src buffert pointer as unsigned char. */

    if(bufflen==0)
    {
	dims[0]=0;
	dims[1]=0;
    }
    else
    {
	dims[0]=1;
	dims[1]=bufflen;
    }
    if(ret_args>nlhs & ret_args>1)
	return;
    /* Create a 2-Dimensional character mxArray. */
    array_ptr = mxCreateCharArray(ndim, dims);
    if (array_ptr == NULL)
	mexErrMsgTxt("Could not create Character mxArray.");

    pr = (mxChar *)mxGetPr(array_ptr);

    ubuff=(unsigned char *)buff; /* readout data as unsigned */
    for (c=0;c<bufflen;c++)
    {
	*pr = (mxChar)(*ubuff);
	ubuff++;
	pr++;
    }
    plhs[ret_args]=array_ptr;
    ret_args++;
}


/****************************************************************************/
/* This function is called on unloading of mex-file                         */

void CleanUpMex(void)
{
    if(closeall()) /* close all still open connections...*/
    {
	/* ....if not all alread closed put out a matlab warning*/
        /* should an breaking error message be better ??? */
	mexWarnMsgTxt("Unloading mex file!\n"
		      "Unclosed tcpip connections will be lost!!!!\n");
    }
#ifdef WIN32
  WSACleanup();
#else
  signal(SIGPIPE,SIG_DFL);  /*reset SIGPIPE to default. Is this good ??? */
#endif
}

/*******************************************************************      */
/* Function to close all still open tcpip connections */
int closeall(void)
{
  for(ipi_index=0;ipi_index<MAX_IPI;ipi_index++)
      close_ipi();
	return 0;
}

/*******************************************************************      */
/* Copys a matlab Char array to a char * string buffer*/
void CopymxCharArray2Buff(const mxArray *array,char *buff,int bufflen)
{
    int arraylen;
    int a;
    mxChar *pr;
    arraylen=mxGetM(array)*mxGetN(array);
    
    
    pr = (mxChar *)mxGetPr(array);
    for(a=0;a<bufflen & a<arraylen;a++)
    {
	*buff=*pr;
	buff++;
	pr++;
    }
}

/*******************************************************************      */
/* Function Creating socket and waits for a connection on specified port.
   Returns connection fid after removing socket to avoid more connections */

int tcpipserver(int port)
{
    int sockfd, new_fd;            /* listen on sock_fd, new connection on new_fd */
    struct sockaddr_in my_addr;    /* my address information */
    int sin_size;
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	return -1;
    }
    my_addr.sin_family = AF_INET;         /* host byte order */
    my_addr.sin_port = htons(port);       /* short, network byte order */
    my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
    memset(&(my_addr.sin_zero),0, 8);        /* zero the rest of the struct */
    
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(void *)1,0);
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) \
	== -1) {
	close(sockfd);
	return -1;
    }
    if (listen(sockfd, TCPIP_BACKLOG) == -1) {
	close(sockfd);
	return -1;
    }
    sin_size = sizeof(struct sockaddr_in);
    if ((new_fd = accept(sockfd, (struct sockaddr *)&ipi[ipi_index].remote_addr, \
			 &sin_size)) == -1)
    {
	close(sockfd);
	return -1;
    }
    close(sockfd);
    setsockopt(new_fd,SOL_SOCKET,SO_KEEPALIVE,(void *)1,0); /* realy needed?*/
    init_ipi(new_fd,STATUS_SERVER);
    return new_fd;
}

/********************************************************************/
/* Init current record with values                                  */
void init_ipi(int fid,int status)
{
    ipi[ipi_index].writetimeout=0;
    ipi[ipi_index].readtimeout=0;
    ipi[ipi_index].fid=fid;
    ipi[ipi_index].status=status;
    ipi[ipi_index].buff=NULL;
    ipi[ipi_index].callback[0]='\0';
    ipi[ipi_index].bufflen=0;
    ipi[ipi_index].buffdatalen=0;
    if(ipi[ipi_index].fid>=0)
      nonblockingsocket(ipi[ipi_index].fid);   /* Non blocking read!*/
}

/********************************************************************/
/* Close ipi struct                                                 */

void close_ipi()
{
    if(ipi[ipi_index].fid>=0)
	close(ipi[ipi_index].fid);
    if(ipi[ipi_index].buff==NULL)
	myrealloc(ipi[ipi_index].buff,0);
    init_ipi(-1,STATUS_FREE);
}

/********************************************************************/
/* Turn handler on                                                  */
void get_sigpipe_on(void)
{
#ifndef WIN32
    void *rethand;
    rethand=(void *)signal(SIGPIPE,get_sigpipe_handler);
    /* If signal hander was not tcpipmex own handler then is must be matlabs...*/
    if(rethand!=get_sigpipe_handler && rethand>NULL )
	/* .... then store it to have when restoring with "get_signal_off() */
	matlab_signal_handler=rethand;
#endif
}

/* Turn handler off */
void get_sigpipe_off(void)
{
//    void *rethand;
#ifndef WIN32
  rethand=(void *)signal(SIGPIPE,matlab_signal_handler);
  if(rethand!=get_sigpipe_handler)
      mexErrMsgTxt("Something (in matlab/operating system ??) has replaced tcpipmex signal handler.\n"
                   "Please report this bug in tcpip toolbox to: Peter.Rydsater@ite.mh.se.\n"
                   "Report tcpip toolbox, OS and matlab version.");
#endif
}


/********************************************************************/
/*         Handler for signals when tcpip pipe is broken            */
void get_sigpipe_handler(int sig)
{
  get_sigpipe_on(); /* Turn on handler agian for signal implementations where it is ...
		       automatic turned of after each call.*/
  ipi[ipi_index].status=STATUS_NOCONNECT;
  mexPrintf("\nSIGPIPE -> TCPIP PIPE DISCONNECTED!\n"); /* Just for debugging purpose...*/
}


/********************************************************************/
/* Writes from specified position (pointer) in buffer of spec. length*/
int writedata(char *buff,int len)
{
  int sentlen=0;
  int retval=0;
  int lastsize=1000000;
  if(ipi[ipi_index].status<=STATUS_SERVSOCKET)
    return 0;
  get_sigpipe_on();
  while(sentlen<len)
  {
    if(lastsize<1000)
      usleep(2000);
    retval=send(ipi[ipi_index].fid,&buff[sentlen],len-sentlen,0);
    if(ipi[ipi_index].status==STATUS_NOCONNECT) /* If Disconnected then exit read loop */
      break;
    lastsize=retval>0?retval:0;
    sentlen+=lastsize;
    if(retval<0 && s_errno!=EWOULDBLOCK)
    {
      ipi[ipi_index].status=STATUS_NOCONNECT;
      perror( "send()" );
      mexPrintf("\nERROR -> TCPIP PIPE DISCONNECTED!\n");
      close(ipi[ipi_index].fid);
    }
  }
  get_sigpipe_off();
  return sentlen;
}



/********************************************************************/
/* Read specified length from network to input buffer */
void read2buff(int len)
{
//    int i;
    int retval;
    int readlen;

    if(len==0)
	return;
    if(ipi[ipi_index].status<=STATUS_SERVSOCKET) /* If not read/write fid (broken pipe)
					  then exit. */
      return;    

    /* If exsisting buffer is to small, then realloc a new and move data to it. */
    if(len>ipi[ipi_index].bufflen)
    {
	ipi[ipi_index].buff=(char *)myrealloc(ipi[ipi_index].buff,len);
	if(ipi[ipi_index].buff==NULL)
	    mexErrMsgTxt("Not enouth of memory for allocation of read buffert.");
	ipi[ipi_index].bufflen=len;
    }
    /* Read only to buffert if their is'nt already anouth. */
    if(ipi[ipi_index].buffdatalen<len)
    {
      get_sigpipe_on();   /* Turn on "take care of broken tcpip pipes" */
      retval=recv(ipi[ipi_index].fid, &ipi[ipi_index].buff[ipi[ipi_index].buffdatalen],
                  len-ipi[ipi_index].buffdatalen, 0);
      readlen=retval>0?retval:0;
      ipi[ipi_index].buffdatalen+=readlen;
      if(retval==0)
      {
        ipi[ipi_index].status=STATUS_NOCONNECT;
        mexPrintf("\nFIN -> TCPIP PIPE DISCONNECTED!\n");
        close(ipi[ipi_index].fid);
      }
      else if(retval<0 && s_errno!=EWOULDBLOCK)
      {
        ipi[ipi_index].status=STATUS_NOCONNECT;
        perror( "recv()" );
        mexPrintf("\nERROR -> TCPIP PIPE DISCONNECTED!\n");
        close(ipi[ipi_index].fid);
      }
      get_sigpipe_off();  /* turn off "take care..." */
    }
}


/* Removes specifed number of characters from buffer that is readed into matlab.*/
void removefrombuff(int len)
{
    int newlen;
    if(len>0)
    {
      newlen=ipi[ipi_index].buffdatalen-len;         /* Calc new buff length;*/
      newlen=newlen>0?newlen:0;               /* Not allow negative length.*/
      memmove(&(ipi[ipi_index].buff[0]),
	      &(ipi[ipi_index].buff[len]),newlen);   /* Remove old data in buff;*/
      ipi[ipi_index].buffdatalen=newlen;             /* Set new length.*/
    }
    if(ipi[ipi_index].bufflen>MINBUFFSIZE & ipi[ipi_index].buffdatalen<=MINBUFFSIZE)
    {
      ipi[ipi_index].buff=myrealloc(ipi[ipi_index].buff,MINBUFFSIZE);
      ipi[ipi_index].bufflen=MINBUFFSIZE;
      if(ipi[ipi_index].buff==NULL)
      {
	ipi[ipi_index].bufflen=0;
	mexErrMsgTxt("Not enouth of memory for reallocation"
		       " of smaller read buffert.");
      }
    } 
}

/* A "wrapper" function for memory allocation. Most for debuging /tracing purpose */
void *myrealloc(void *ptr,int newsize)
{
  ptr=realloc(ptr,newsize);
  return ptr;
}

/* DEBUGING FUNCTION */
void __debug_view_ipi_status(char *str)
{
    int a;

    mexPrintf("%s\n",str);
    for(a=0;a<5;a++)
    {
	mexPrintf("[%02d] FID:%02d STATUS:%02d",a,ipi[a].fid,ipi[a].status);
	if(a==ipi_index)
	    mexPrintf("<--\n");
	else
	    mexPrintf("\n");
    }
    mexPrintf("--------------------\n");
}

/* Windows implementation of perror() function */
#ifdef WIN32
void perror( char *context )
{
	int wsa_err;
	wsa_err = WSAGetLastError();
	mexPrintf( "[%s]: WSA error: ", context );
	switch ( wsa_err )
	{
	case WSANOTINITIALISED: mexPrintf( "WSANOTINITIALISED\n" ); break;
	case WSAENETDOWN:       mexPrintf( "WSAENETDOWN      \n" ); break;
	case WSAEADDRINUSE:     mexPrintf( "WSAEADDRINUSE    \n" ); break;
	case WSAEACCES:         mexPrintf( "WSAEACCES        \n" ); break;
	case WSAEINTR:          mexPrintf( "WSAEINTR         \n" ); break;
	case WSAEINPROGRESS:    mexPrintf( "WSAEINPROGRESS   \n" ); break;
	case WSAEALREADY:       mexPrintf( "WSAEALREADY      \n" ); break;
	case WSAEADDRNOTAVAIL:	mexPrintf( "WSAEADDRNOTAVAIL \n" ); break;
	case WSAEAFNOSUPPORT:   mexPrintf( "WSAEAFNOSUPPORT  \n" ); break;
	case WSAEFAULT:         mexPrintf( "WSAEFAULT        \n" ); break;
	case WSAENETRESET:      mexPrintf( "WSAENETRESET     \n" ); break;
	case WSAENOBUFS:        mexPrintf( "WSAENOBUFS       \n" ); break;
	case WSAENOTSOCK:       mexPrintf( "WSAENOTSOCK      \n" ); break;
	case WSAEOPNOTSUPP:     mexPrintf( "WSAEOPNOTSUPP    \n" ); break;
	case WSAESHUTDOWN:      mexPrintf( "WSAESHUTDOWN     \n" ); break;
	case WSAEWOULDBLOCK:    mexPrintf( "WSAEWOULDBLOCK   \n" ); break;
	case WSAEMSGSIZE:       mexPrintf( "WSAEMSGSIZE      \n" ); break;
	case WSAEHOSTUNREACH:	mexPrintf( "WSAEHOSTUNREACH  \n" ); break;
	case WSAEINVAL:         mexPrintf( "WSAEINVAL        \n" ); break;
	case WSAECONNREFUSED:   mexPrintf( "WSAECONNREFUSED  \n" ); break;
	case WSAECONNABORTED:   mexPrintf( "WSAECONNABORTED  \n" ); break;
	case WSAECONNRESET:     mexPrintf( "WSAECONNRESET    \n" ); break;
	case WSAEISCONN:        mexPrintf( "WSAEISCONN       \n" ); break;
	case WSAENOTCONN:       mexPrintf( "WSAENOTCONN      \n" ); break;
	case WSAETIMEDOUT:      mexPrintf( "WSAETIMEDOUT     \n" ); break;
	default:                mexPrintf( "Unknown(%d) error!\n", wsa_err ); break;
	}
	return;
}
#endif

