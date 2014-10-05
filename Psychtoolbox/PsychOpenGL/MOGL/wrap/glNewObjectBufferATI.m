function r = glNewObjectBufferATI( size, pointer, usage )

% glNewObjectBufferATI  Interface to OpenGL function glNewObjectBufferATI
%
% usage:  r = glNewObjectBufferATI( size, pointer, usage )
%
% C function:  GLuint glNewObjectBufferATI(GLsizei size, const void* pointer, GLenum usage)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

r = moglcore( 'glNewObjectBufferATI', size, pointer, usage );

return
