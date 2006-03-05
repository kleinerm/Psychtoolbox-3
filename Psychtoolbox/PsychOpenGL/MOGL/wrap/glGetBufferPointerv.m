function params = glGetBufferPointerv( target, pname )

% glGetBufferPointerv  Interface to glGetBufferPointerv
% 
% usage:  params = glGetBufferPointerv( target, pname )
%
% C function:  void glGetBufferPointerv(GLenum target, GLenum pname, GLvoid** params)

% 02-Jan-2006 -- created (RFM)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

params=uint32(0);
moglcore( 'glGetBufferPointerv', target, pname, params );

return
