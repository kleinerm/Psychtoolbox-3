function fprintfEmpty
% Mathworks Case ID: 297266, reported 10/3/99.
% 
% FPRINTF terminates when it gets an empty matrix [].
% 
% dear mathworks
% 
% i was very surprised to discover a bug in FPRINTF today:
% 
% x=1;
% y=1;
% fprintf('x %d y %d\n',x,y);
% PRODUCES THIS EXPECTED OUTPUT:
% x 1 y 1
% 
% x=[];
% y=1;
% fprintf('x %d y %d\n',x,y);
% PRODUCES THIS UNEXPECTED OUTPUT:
% x 
% WHEREAS I EXPECTED THIS OUTPUT:
% x [] y 1
% 
% when the variable's value is the empty matrix, FPRINTF terminates the 
% printout prematurely, suppressing the rest of it. The FPRINTF 
% documentation provided by HELP and DOC says nothing of this awkward 
% behavior. I expected FPRINTF to print the Matlab symbol for empty matrix, 
% "[]". 
% 
% Using the empty matrix to represent missing values is one of the great 
% features of Matlab. Having FPRINTF abort the rest of the print out, and 
% my data, is contrary to the whole spirit of coping gracefully with 
% missing values.
% 
% I suspect that this bug is platform-independent. I'm running Matlab 5.2.1 
% on a PowerMac. I know that MathWorks has no current plans to upgrade the 
% OS9 version, but I understand that it is still logging bugs, to keep its 
% options open. 
% 
% denis pelli
% professor of psychology and neural science at nyu
% apple developer
% 
%
% Reported as a bug to Mathworks on 10/3/99, Case ID: 297266. Their reply (10/4) follows:
% 
% "The behavior you reported is caused by a bug which has been forwarded to
% development for cosideration in a future release of Matlab."
% 
% Kelly Bletsis
% Technical Support Engineer
% kbletsis@mathworks.com
% 
%  
% Denis Pelli

