function r = glGetVaryingLocationNV( program, name )

% glGetVaryingLocationNV  Interface to OpenGL function glGetVaryingLocationNV
%
% usage:  r = glGetVaryingLocationNV( program, name )
%
% C function:  GLint glGetVaryingLocationNV(GLuint program, const GLchar* name)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glGetVaryingLocationNV', program, uint8(name) );

return
