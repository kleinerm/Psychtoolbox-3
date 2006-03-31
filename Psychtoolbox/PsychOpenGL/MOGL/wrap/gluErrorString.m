function r = gluErrorString( err )

% gluErrorString  Interface to gluErrorString
% 
% usage:  r = gluErrorString( err )
%         r = gluErrorString
% 
% - with no input arguments, 'err' is obtained by calling glGetError
%
% C function:  const GLubyte * gluErrorString (GLenum error)

% 21-Dec-2005 -- created (RFM)

% ---protected---

if nargin==0,
    err=glGetError;
elseif nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'gluErrorString', err );

return
