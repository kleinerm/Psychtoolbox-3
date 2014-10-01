function glSampleCoverageARB( value, invert )

% glSampleCoverageARB  Interface to OpenGL function glSampleCoverageARB
%
% usage:  glSampleCoverageARB( value, invert )
%
% C function:  void glSampleCoverageARB(GLfloat value, GLboolean invert)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glSampleCoverageARB', value, invert );

return
