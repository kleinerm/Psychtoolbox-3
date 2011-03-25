function params = glGetVertexAttribPointerv( index, pname )

% glGetVertexAttribPointerv  Interface to glGetVertexAttribPointerv
% 
% usage:  params = glGetVertexAttribPointerv( index, pname )
%
% C function:  void glGetVertexAttribPointerv (GLuint index, GLenum pname, GLvoid** pointer)

% 02-Jan-2006 -- created (RFM)
% 24-Mar-2011 -- Made 64-bit clean (MK)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

params=uint64(0);
moglcore( 'glGetVertexAttribPointerv', index, pname, params );

return
