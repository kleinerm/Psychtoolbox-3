function params = glGetPointerv( pname )

% glGetPointerv  Interface to glGetPointerv
% 
% usage:  params = glGetPointerv( pname )
%
% C function:  void glGetPointerv (GLenum pname, GLvoid* *params);

% 01-Jan-2006 -- created (RFM)
% 24-Mar-2011 -- Made 64-bit clean (MK)

% ---allocate---
% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

params=uint64(0);
moglcore( 'glGetPointerv', pname, params );

return
