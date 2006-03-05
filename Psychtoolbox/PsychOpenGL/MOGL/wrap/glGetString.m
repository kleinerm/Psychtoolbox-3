function r = glGetString( name )

% glGetString  Interface to glGetString
% 
% usage:  r = glGetString( name )
% 
% C function:  const GLubyte * glGetString (GLenum name)

% 21-Dec-2005 -- created (RFM)

% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glGetString', name );

return
