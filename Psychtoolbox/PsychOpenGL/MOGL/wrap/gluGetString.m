function r = gluGetString( name )

% gluGetString  Interface to gluGetString
% 
% usage:  r = gluGetString( name )
% 
% C function:  const GLubyte * gluGetString (GLenum name)

% 21-Dec-2005 -- created (RFM)

% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'gluGetString', name );

return
