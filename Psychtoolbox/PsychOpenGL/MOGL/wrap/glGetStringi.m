function r = glGetStringi( name, index )

% glGetStringi  Interface to OpenGL function glGetStringi
%
% usage:  r = glGetStringi( name, index )
%
% C function:  const GLubyte* glGetStringi(GLenum name, GLuint index)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glGetStringi', name, index );

return
