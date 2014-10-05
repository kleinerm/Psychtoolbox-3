function glBlendFuncIndexedAMD( buf, src, dst )

% glBlendFuncIndexedAMD  Interface to OpenGL function glBlendFuncIndexedAMD
%
% usage:  glBlendFuncIndexedAMD( buf, src, dst )
%
% C function:  void glBlendFuncIndexedAMD(GLuint buf, GLenum src, GLenum dst)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glBlendFuncIndexedAMD', buf, src, dst );

return
