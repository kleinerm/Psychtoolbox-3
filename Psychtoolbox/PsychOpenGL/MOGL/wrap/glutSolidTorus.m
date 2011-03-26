function glutSolidTorus( innerRadius, outerRadius, sides, rings )

% glutSolidTorus  Interface to OpenGL function glutSolidTorus
%
% usage:  glutSolidTorus( innerRadius, outerRadius, sides, rings )
%
% C function:  void glutSolidTorus(GLdouble innerRadius, GLdouble outerRadius, GLint sides, GLint rings)

% 26-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glutSolidTorus', innerRadius, outerRadius, sides, rings );

return
