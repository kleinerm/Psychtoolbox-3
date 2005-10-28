function ConcatenationBug
% Mathworks Case ID:  84454, reported 7/17/98
% 
% Square and curly braces sometimes can't embrace multiple lines.
% 
% This works:
% 
% {'a'
% 'b'}
% 
% So does this:
% 
% x={'a'
% 'b'}
% 
% And this:
% 
% x=char({'a'
% 'b'})
% 
% But this fails:
% 
% char({'a'
% 'b'})
% ??? 'b'}
%        |
% Missing operator, comma, or semi-colon.
% 
% 
% Shouldn't the braces work consistently?
% 
% Case ID:  84454
% 
% Denis,
% 
% I am writing in response to your email of July 20, 1998, regarding the
% error you receive when pass CHAR an expression enclosed in curly braces.
% 
% This has been verified here at The MathWorks, and it has been forwarded to
% our development staff to be addressed in a future release.
% 
% ...
% 
% Sincerely,
% 
% Vincent Hodges
% Technical Support Engineer
% vhodges@mathworks.com
