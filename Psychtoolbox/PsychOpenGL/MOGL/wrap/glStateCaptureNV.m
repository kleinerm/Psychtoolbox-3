function glStateCaptureNV( state, mode )

% glStateCaptureNV  Interface to OpenGL function glStateCaptureNV
%
% usage:  glStateCaptureNV( state, mode )
%
% C function:  void glStateCaptureNV(GLuint state, GLenum mode)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glStateCaptureNV', state, mode );

return
