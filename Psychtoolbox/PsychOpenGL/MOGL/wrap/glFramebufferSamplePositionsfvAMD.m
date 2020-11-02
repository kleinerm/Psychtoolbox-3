function glFramebufferSamplePositionsfvAMD( target, numsamples, pixelindex, values )

% glFramebufferSamplePositionsfvAMD  Interface to OpenGL function glFramebufferSamplePositionsfvAMD
%
% usage:  glFramebufferSamplePositionsfvAMD( target, numsamples, pixelindex, values )
%
% C function:  void glFramebufferSamplePositionsfvAMD(GLenum target, GLuint numsamples, GLuint pixelindex, const GLfloat* values)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glFramebufferSamplePositionsfvAMD', target, numsamples, pixelindex, single(values) );

return
