function params = glGetVertexAttribPointerv( index, pname )

% glGetVertexAttribPointerv  Interface to glGetVertexAttribPointerv
% 
% usage:  params = glGetVertexAttribPointerv( index, pname )
%
% C function:  void glGetVertexAttribPointerv (GLuint index, GLenum pname, GLvoid** pointer)

% 02-Jan-2006 -- created (RFM)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

params=uint32(0);
moglcore( 'glGetVertexAttribPointerv', index, pname, params );

return
