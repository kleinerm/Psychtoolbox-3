function R = qGetR( Qrotation )
% qGetR: get a 3x3 rotation matrix R from a Quaternion Qrotation.
%
% R = qGetR( Qrotation )
%
% IN: 
%  Qrotation - quaternion (x,y,z,w) describing rotation
%
% OUT:
%  R - 3 x 3 rotation matrix 
%
% VERSION: 03.03.2012
%
% This file is a modified version of qGetR from the "quaternion" package
% of  Przemyslaw Baranski downloaded from Mathworks File Exchange at URL:
%
% http://de.mathworks.com/matlabcentral/fileexchange/35475-quaternions
%
% The file and its parent toolbox are licensed under BSD License, as follows:
%
%  Copyright (c) 2012, Przemyslaw Baranski
%  All rights reserved.
%
%  Redistribution and use in source and binary forms, with or without
%  modification, are permitted provided that the following conditions are
%  met:
%
%      * Redistributions of source code must retain the above copyright
%        notice, this list of conditions and the following disclaimer.
%      * Redistributions in binary form must reproduce the above copyright
%        notice, this list of conditions and the following disclaimer in
%        the documentation and/or other materials provided with the distribution
%
%  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
%  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
%  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
%  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
%  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
%  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
%  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
%  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
%  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
%  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
%  POSSIBILITY OF SUCH DAMAGE.

% History:
% 03-Mar-2012     Written by Przemyslaw Baranski
% 14-Sep-2015 mk  Modified for use of quaternion format [x,y,z,w] instead of
%                 original format [w,x,y,z]. Included into PsychToolbox.

% Original:
%w = Qrotation( 1 );
%x = Qrotation( 2 );
%y = Qrotation( 3 );
%z = Qrotation( 4 );

% mk modification - different order (x,y,z,w) instead of (w,x,y,z)!
x = Qrotation( 1 );
y = Qrotation( 2 );
z = Qrotation( 3 );
w = Qrotation( 4 );

Rxx = 1 - 2*(y^2 + z^2);
Rxy = 2*(x*y - z*w);
Rxz = 2*(x*z + y*w);

Ryx = 2*(x*y + z*w);
Ryy = 1 - 2*(x^2 + z^2);
Ryz = 2*(y*z - x*w );

Rzx = 2*(x*z - y*w );
Rzy = 2*(y*z + x*w );
Rzz = 1 - 2 *(x^2 + y^2);

R = [ 
    Rxx,    Rxy,    Rxz;
    Ryx,    Ryy,    Ryz;
    Rzx,    Rzy,    Rzz];
