function rect=AlignRect(rect,fixedRect,side1,side2)
% rect=AlignRect(rect,fixedRect,side1,[side2])
%
% Moves rect to align its center/top/bottom/left/right with the
% corresponding edge(s)/point of fixedRect. Does "side1" and then "side2".
% The legal values for side1 and side2 are 'center', 'left', 'right',
% 'top', and 'bottom'. 
%      r=AlignRect(r,screenRect,'center','top');
% For backward compatibility, also accepts 1,2,3,4,5. You may use the
% pre-defined constants RectLeft,RectRight,RectTop,RectBottom, but there is
% no named constant for centering.
% Also see PsychRects.

% Denis Pelli 5/27/96, 7/10/96, 8/5/96, 11/8/06

if nargin<3
    error('Usage:  rect=AlignRect(rect,fixedRect,side1,[side2])');
end
if size(rect,2)~=4 || size(fixedRect,2)~=4
    error('Wrong size rect arguments. Usage:  rect=AlignRect(rect,fixedRect,side1,[side2])');
end
side{1}=side1;
if nargin>3
    side{2}=side2;
end
for i=1:length(side)
    if isnumeric(side{i})
        if ~ismember(side{i},1:5)
            error('Illegal side1 or side2 value.');
        end
    elseif ischar(side{i})
        [ok,n]=ismember(side{i},{'left','top','right','bottom','center'});
        if ~ok
            error('Illegal side1 or side2 value.');
        end
        side{i}=n;
    else
        error('Illegal side1 or side2 value.');
    end
    switch side{i}
        case 5
            rect=CenterRect(rect,fixedRect);
        case {RectLeft,RectRight},
            rect=OffsetRect(rect,fixedRect(side{i})-rect(side{i}),0);
        case {RectTop,RectBottom},
            rect=OffsetRect(rect,0,fixedRect(side{i})-rect(side{i}));
    end
end
