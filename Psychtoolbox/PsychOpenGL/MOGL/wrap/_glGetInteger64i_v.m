function data = glGetInteger64i_v( target, index )

% glGetInteger64i_v  Interface to OpenGL function glGetInteger64i_v
%
% usage:  data = glGetInteger64i_v( target, index )
%
% C function:  void glGetInteger64i_v(GLenum target, GLuint index, GLint64* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

data = int64(0);

moglcore( 'glGetInteger64i_v', target, index, data );

return
