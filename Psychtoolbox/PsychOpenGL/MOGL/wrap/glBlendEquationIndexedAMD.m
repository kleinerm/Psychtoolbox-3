function glBlendEquationIndexedAMD( buf, mode )

% glBlendEquationIndexedAMD  Interface to OpenGL function glBlendEquationIndexedAMD
%
% usage:  glBlendEquationIndexedAMD( buf, mode )
%
% C function:  void glBlendEquationIndexedAMD(GLuint buf, GLenum mode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glBlendEquationIndexedAMD', buf, mode );

return
