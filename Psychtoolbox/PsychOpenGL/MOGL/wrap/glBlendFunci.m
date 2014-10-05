function glBlendFunci( buf, src, dst )

% glBlendFunci  Interface to OpenGL function glBlendFunci
%
% usage:  glBlendFunci( buf, src, dst )
%
% C function:  void glBlendFunci(GLuint buf, GLenum src, GLenum dst)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glBlendFunci', buf, src, dst );

return
