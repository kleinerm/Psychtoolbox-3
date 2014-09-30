function glMatrixLoadfEXT( mode, m )

% glMatrixLoadfEXT  Interface to OpenGL function glMatrixLoadfEXT
%
% usage:  glMatrixLoadfEXT( mode, m )
%
% C function:  void glMatrixLoadfEXT(GLenum mode, const GLfloat* m)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMatrixLoadfEXT', mode, single(m) );

return
