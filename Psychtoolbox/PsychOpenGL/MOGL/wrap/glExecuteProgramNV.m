function glExecuteProgramNV( target, id, params )

% glExecuteProgramNV  Interface to OpenGL function glExecuteProgramNV
%
% usage:  glExecuteProgramNV( target, id, params )
%
% C function:  void glExecuteProgramNV(GLenum target, GLuint id, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glExecuteProgramNV', target, id, single(params) );

return
