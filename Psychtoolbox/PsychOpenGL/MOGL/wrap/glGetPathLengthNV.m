function r = glGetPathLengthNV( path, startSegment, numSegments )

% glGetPathLengthNV  Interface to OpenGL function glGetPathLengthNV
%
% usage:  r = glGetPathLengthNV( path, startSegment, numSegments )
%
% C function:  GLfloat glGetPathLengthNV(GLuint path, GLsizei startSegment, GLsizei numSegments)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

r = moglcore( 'glGetPathLengthNV', path, startSegment, numSegments );

return
