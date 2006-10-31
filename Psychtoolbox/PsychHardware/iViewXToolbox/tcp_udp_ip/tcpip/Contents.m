%  Tcpip toolbox version 1.2.x  2000-12-14 for MATLAB 5.x 6.x
%
%  Copyrigtht (C) Peter Rydesäter 1998 - 2000, Mitthögskolan, SWEDEN
%
%  This program is free software; you can redistribute it and/or
%  modify it under the terms of the GNU General Public License
%  as published by the Free Software Foundation; either version 2
%  of the License, or (at your option) any later version.
%
%  This program is distributed in the hope that it will be useful,
%  but WITHOUT ANY WARRANTY; without even the implied warranty of
%  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%  GNU General Public License for more details.
%
%  You should have received a copy of the GNU General Public License
%  along with this program; if not, write to the Free Software
%  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
%
%  Please, send me an e-mail if you use this and/or improve it.
%
%  Main implementation:              Implementation of windows support:
%  ===================
%
%  Peter Rydesäter                   Mario Bergeron
%  Mitthögskolan                     LYR Signal Processing
%  Östersund,Sweden                  Québec, Canada
%  e-mail:Peter.Rydesater@ite.mh.se  e-mail: Mario.Bergeron@lyre.qc.ca
%  
%
%  Toolbox to do tcpip connection over internet
%  and send/receive data from/to matlab.
%
%  The core of this toolbox is a mex file.
%  Seams to Works well with matlab for Linux (intel),
%  Windows 95(98?), Windows NT and  Solaris.
%  Should be easy to compile in any unix system.
%
%  With this toolbox you can do tcpip connection with
%  matlab and transmit data over Intranet/Internet
%  Between matlab processes or other applications.
%
%  Use tcpip_open to do a remote connection and get
%  a returned handler number that is used for all
%  other command.
%  tcpip_servsocket is used to act as a multi-connection
%  server.
%  All I/O operation is non blocking except tcpip_servopen
%  and tcpip_write
%
%
%  Contents         This help file.
%  tcpip_close      Closes an open tcpip connection.
%  tcpip_open       Opens a new tcpip connection.
%  tcpip_read       Reads an array of bytes from pipe.
%  tcpip_readln     Reads a line of chars (bytes) if their is a complete.
%  tcpip_sendfile   Sends a file throw connection to receiving "tcpip_getfile"
%  tcpip_getfile    Receives a data from sending "tcpip_sendfile" and saves to file.
%  tcpip_sendvar    Send matlab variable.
%  tcpip_getvar     Get matlab variable.
%  tcpip_feval      Remote/paralell "feval" executes function remotely in other machine
%  tcpip_feval_end  Gets remote return arguments from tcpip_feval
%  tcpip_feval_server Corresponding server function to tcpip_feval
%  tcpip_calc_server  Complete tcpip_feval_server includning tcpip_serveropen....
%  tcpip_servopen   OLD! Only for compatibility. Blocking wait for connetion!
%  tcpip_servsocket Creates a socket binded to a port, waiting for connections!
%  tcpip_listen     Checks/Gets connection connected to tcpip_servsocket
%  tcpip_status     Returns status of open connection. Detects broken connections.
%  tcpip_viewbuff   Returns whats in receiving buffer but will not empty it.
%  tcpip_write      Sends an array of bytes to connection.
%  popmail_demo     Demo program that read mail from pop mail server.
%  webserver_demo   Matlab as a little webserver, does matlab calculations.
%  tcpipmex.c       C-source for the mex file that is core of this toolbox.
%  tcpipmex         m-file that will compile the .c-file to mex for your platform.
%  tcpipmex.dll     mex file (.dll file) for Windows 95/98, Windows NT
%  tcpipmex.mexlx   mex file for Intel Linux matlab 5.x
%  tcpipmex.mexglx  mex file for Intel Linux matlab 6.x
%  tcpipmex.mexsol  mex file for Sparc Solaris.
%  whatsnew.txt     Information about changed between versions
%
