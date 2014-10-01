function glGenerateTextureMipmap( texture )

% glGenerateTextureMipmap  Interface to OpenGL function glGenerateTextureMipmap
%
% usage:  glGenerateTextureMipmap( texture )
%
% C function:  void glGenerateTextureMipmap(GLuint texture)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glGenerateTextureMipmap', texture );

return
