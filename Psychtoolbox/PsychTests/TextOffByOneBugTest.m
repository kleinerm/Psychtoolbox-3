% TextBug
% Demonstrates an off-by-one bug in Screen('Drawtext'). The rendered text
% is drawn +1 pixel to the right of where it should be. This leaves the
% leftmost pixel unused, and clips off the rightmost pixel of the string.
% The bug is demonstrated here by drawing a letter twice, first in green as
% part of a string that has a space added (so no clipping occurs) and is
% then overwritten in black, without the added space. A green right edge is
% apparent in many rendered characters, at least in the Sloan font whose
% characters have no built-in spacing. The program also finds the bounding
% box of each character. For Sloan, the top bound is always zero, which is
% correct, but the left bound is always 1, which should also be zero.
% 
% Denis Pelli March 21, 2007
% denis.pelli@...
% The Sloan font may be downloaded from here:
% http://psych.nyu.edu/pelli/software.html
 

% 3/21/07    dgp     Wrote it, based on allen ingling's DrawSomeTextDemo.m
 

alphabet='CDHKNORSVZ';
font='Sloan';
fontSize=640;
try
    w=Screen('OpenWindow',0);
    Screen('TextFont',w,font);
    Screen('TextSize',w,fontSize);
    fprintf('\n\nFont is %s at size %d.\n',Screen('TextFont',w),fontSize);
    black=BlackIndex(w);
    white=WhiteIndex(w);
    green=[0 255 0];
    rect=1.5*[0 0 fontSize fontSize];
    screenRect=Screen('Rect',w);
    rect=ClipRect(rect,screenRect);
    for a=alphabet
        Screen('FillRect',w,white);
        Screen('DrawText',w,[a ' '],0,0,green);
        Screen('DrawText',w,a,0,0,black);
        Screen('Flip',w);
        m=Screen('GetImage',w,rect);
        b=m(:,:,2)==black(1); % convert to binary bitmap
        vMargin=zeros(1,size(b,1));
        for i=1:size(b,1)
            vMargin(i)=any(b(i,:));
        end
        hMargin=zeros(1,size(b,2));
        for j=1:size(b,2)
            hMargin(j)=any(b(:,j));
        end
        vList=[nan find(vMargin)];
        hList=[nan find(hMargin)];
        bounds=[min(hList)-1 min(vList)-1 max(hList) max(vList)];
        width=RectWidth(bounds);
        height=RectHeight(bounds);
        fprintf('"%s" is %3.0f wide %3.0f high, with bounding rect [%3d %3d %3d %3d].',a,width,height,bounds);
        g=(m(:,:,1)==green(1)) & (m(:,:,2)==green(2));
        ghMargin=zeros(1,size(g,2));
        for j=1:size(g,2)
            ghMargin(j)=any(g(:,j));
        end
        gList=find(ghMargin);
        if length(gList)>0
            fprintf(' Clipped cols:');
            fprintf(' %d',gList);
            fprintf('.\n',sum(g(:)));
        else
            fprintf('\n');
        end
    end
    Screen('CloseAll');
catch
    Screen('CloseAll');
    psychrethrow(psychlasterror);
end
