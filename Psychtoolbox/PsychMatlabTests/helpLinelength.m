function helpLinelength
% Mathworks Case ID:  41758, reported 4/12/98
% 
% HELP terminates if a line exceeds 131 characters, skipping rest of file.
% 
% Dear Denis,
% 
% I am writing in response to your email of Sunday, April 12, 1998, regarding
% the following observation:
% 
% >Typing "help foo" is supposed to print out the whole file, 
% >but i discover that the rest of the file is ignored if any 
% >line is longer than around 135 characters.
% 
% There seems to be a limit of 131 characters in the M-file comment lines
% that will be displayed as help text. I verified this using the following
% commands:
% 
% >>x=help('foo')
% >>size(x)
% 
% ans =
% 
% 	1    131
% 
% If this number is exceeded, the help text will ignore the rest of the
% M-file.  I will forward this to our development staff so that they may
% consider addressing this in a future release of Matlab.  As a workaround,
% please keep the comment lines to under a 131 characters in length.
% 
% If you have further questions regarding this issue, please be sure to
% mention the above Case ID in your email response to me.
% 
% Sincerely,
% 
% Jeff Arsenault
% Technical Support Engineer
% jeffreya@mathworks.com
