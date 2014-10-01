function [ stride, layout ] = glMapTexture2DINTEL( texture, level, access )

% glMapTexture2DINTEL  Interface to OpenGL function glMapTexture2DINTEL
%
% usage:  [ stride, layout ] = glMapTexture2DINTEL( texture, level, access )
%
% C function:  void* glMapTexture2DINTEL(GLuint texture, GLint level, GLbitfield access, GLint* stride, GLenum* layout)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

stride = int32(0);
layout = uint32(0);

moglcore( 'glMapTexture2DINTEL', texture, level, access, stride, layout );

return
