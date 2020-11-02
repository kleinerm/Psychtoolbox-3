function pixels = glGetnTexImage( target, level, format, type, bufSize )

% glGetnTexImage  Interface to OpenGL function glGetnTexImage
%
% usage:  pixels = glGetnTexImage( target, level, format, type, bufSize )
%
% C function:  void glGetnTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, void* pixels)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=5,
    error('invalid number of arguments');
end

pixels = (0);

moglcore( 'glGetnTexImage', target, level, format, type, bufSize, pixels );

return
