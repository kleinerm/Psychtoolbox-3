function glNamedFramebufferSamplePositionsfvAMD( framebuffer, numsamples, pixelindex, values )

% glNamedFramebufferSamplePositionsfvAMD  Interface to OpenGL function glNamedFramebufferSamplePositionsfvAMD
%
% usage:  glNamedFramebufferSamplePositionsfvAMD( framebuffer, numsamples, pixelindex, values )
%
% C function:  void glNamedFramebufferSamplePositionsfvAMD(GLuint framebuffer, GLuint numsamples, GLuint pixelindex, const GLfloat* values)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNamedFramebufferSamplePositionsfvAMD', framebuffer, numsamples, pixelindex, single(values) );

return
