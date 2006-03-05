function glSamplePass( pass )

% glSamplePass  Interface to OpenGL function glSamplePass
%
% usage:  glSamplePass( pass )
%
% C function:  void glSamplePass(GLenum pass)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSamplePass', pass );

return
