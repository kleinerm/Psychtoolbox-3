function glmSetClut( clut )

% glmSetClut  Open a screen in the mogl interface
% 
% usage:  glmSetClut( clut )

% 09-Dec-2005 -- created (RFM)

% ---protected---

if nargin==0,
    clut=repmat((0:255)',[ 1 3 ]);
elseif nargin~=1,
    error('invalid number of arguments');
end

if any(size(clut)~=[ 256 3 ]),
    error('lookup table must be 256x3');
end

moglcore('glmSetClut',clut);

return
