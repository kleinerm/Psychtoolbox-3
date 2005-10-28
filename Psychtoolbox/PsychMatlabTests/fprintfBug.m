function fprintfBug
% Mathworks Case ID:  201530, reported 3/19/99
% 
% FPRINTF may interpret the first argument as a filename.
% 
% Dear Denis,
% 
% I am responding to your e-mails of March 19, concerning FPRINTF.
% 
% > in my first message (below) i noted that
% >
% > >fprintf('"%s" hit at time %.3f s\n','5%',1)
% > >
% > >produces no output at all.
% >
% > I now discover that FPRINTF created a file called '"%s" hit
% > at time %.3f s\n' on my disk, and stored a '5' in it each
% > time i executed that statement. Treating the first argument
% > of FPRINTF as a file NAME is undocumented, and not what I
% > expected.
% >
% > Can you clarify what Matlab is doing?
% 
% Yes, this behavior is due to a bug in the FPRINTF command.  For some
% reason, passing more than two arguments to FPRINTF can cause the first
% argument to be treated as the filename or file ID.  This has been reported
% to our development staff to be addressed in a future version of Matlab.
% 
% As a workaround you can manually specify FID to 1 (for standard output) in
% your FPRINTF statement:
% 
%   a='%04d';
%   fprintf(1,'ls %s %i\n',a,4);
% 
% Another workaround is to make multiple calls to the FPRINTF command.  For
% example:
% 
%   a='%04d';
%   fprintf('ls %s ',a);
%   fprintf('%d\n',4);
% 
% If you have further questions regarding this issue, please be sure to
% mention the above case ID number in your e-mail response to me.
% 
% Sincerely,
% 
% Jeff Mather
% Technical Support Engineer
% jmather@mathworks.com
