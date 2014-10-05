function [ r, x, y, tangentX, tangentY ] = glPointAlongPathNV( path, startSegment, numSegments, distance )

% glPointAlongPathNV  Interface to OpenGL function glPointAlongPathNV
%
% usage:  [ r, x, y, tangentX, tangentY ] = glPointAlongPathNV( path, startSegment, numSegments, distance )
%
% C function:  GLboolean glPointAlongPathNV(GLuint path, GLsizei startSegment, GLsizei numSegments, GLfloat distance, GLfloat* x, GLfloat* y, GLfloat* tangentX, GLfloat* tangentY)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=4,
    error('invalid number of arguments');
end

x = single(0);
y = single(0);
tangentX = single(0);
tangentY = single(0);

r = moglcore( 'glPointAlongPathNV', path, startSegment, numSegments, distance, x, y, tangentX, tangentY );

return
