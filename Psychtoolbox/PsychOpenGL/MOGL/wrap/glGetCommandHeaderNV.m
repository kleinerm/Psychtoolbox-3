function r = glGetCommandHeaderNV( tokenID, size )

% glGetCommandHeaderNV  Interface to OpenGL function glGetCommandHeaderNV
%
% usage:  r = glGetCommandHeaderNV( tokenID, size )
%
% C function:  GLuint glGetCommandHeaderNV(GLenum tokenID, GLuint size)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glGetCommandHeaderNV', tokenID, size );

return
