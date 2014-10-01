function glUnmapTexture2DINTEL( texture, level )

% glUnmapTexture2DINTEL  Interface to OpenGL function glUnmapTexture2DINTEL
%
% usage:  glUnmapTexture2DINTEL( texture, level )
%
% C function:  void glUnmapTexture2DINTEL(GLuint texture, GLint level)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glUnmapTexture2DINTEL', texture, level );

return
