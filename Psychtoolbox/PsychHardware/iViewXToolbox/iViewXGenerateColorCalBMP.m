% generate calibration bmps
try
    myimgdir='calbmps';

    clear ivx;

    makecolorim=1;
    makehighcontrastim=0;
    whitemarkers=1;
    colors=[255 255 255; 255 0 0; 0 255 0; 0 0 255; 255 255 0;255 255 255;255 255 255;255 255 255;255 255 255;255 255 255;255 255 255;255 255 255;255 255 255;255 255 255;255 255 255;255 255 255;255 255 255];


    screenNumber=max(Screen('Screens'));

    [w, screenRect]=Screen('OpenWindow', screenNumber, 0,[],32,2);
    white=WhiteIndex(screenNumber);
    black=BlackIndex(screenNumber);
    %     gray=round((white+black)/2);
    gray=GrayIndex(screenNumber);
    Screen('FillRect',w, gray);
    Screen('Flip',w);
    ivx=iViewXInitDefaults;

    points=5;

    shift=[0 0];

    scale=25;
    scale=[scale scale];

    ivx=iViewXSetCalPoints(ivx, points, shift, scale);

    [dummy,dummy]=mkdir(myimgdir);
    fprintf('Generating calibration BMP using %d points.\n', ivx.nCalPoints);

    if 0==makehighcontrastim
        Screen('FillRect',w, gray);
    else
        Screen('FillRect',w, black);
    end

    for i=1:ivx.nCalPoints
        point=ivx.absCalPos(i,:);
        rect=CenterRectOnPoint(ivx.calRect, point(1), point(2));
        f=8;
        brect=ScaleRect(rect,f,f);
        brect=CenterRectOnPoint(brect, point(1), point(2));


        if 0==makecolorim
            mycrosscolor=ivx.calPointColour;
                mycolor=ivx.calPointColour;
        else
            mycrosscolor=squeeze(colors(i,:));
            if whitemarkers==1
                mycolor=ivx.calPointColour;
            else
                mycolor=squeeze(colors(i,:));

            end
           
        end
        
        Screen('DrawLine', w, mycrosscolor, brect(1), brect(2), brect(3), brect(4) ,8);
        Screen('DrawLine', w, mycrosscolor, brect(1), brect(4), brect(3), brect(2) ,8);
        Screen('FillOval',w, mycolor, rect);

        rect=CenterRectOnPoint(ivx.calRectCenter, point(1), point(2));
        Screen('FillOval',w, ivx.calPointColourCenter, rect);
    end

    %     Screen('Flip',w);
    % WaitSecs(1);

    % we need to get the image before the flip
    imageArray=Screen('GetImage', w );

    Screen('Flip',w);
    WaitSecs(1);

    Screen('FillRect',w, gray);
    Screen('Flip',w);

    WaitSecs(1);

    Screen('PutImage', w, imageArray);
    Screen('Flip',w);

    max(max(max(imageArray)));

    type='bmp';

    [h v]=WindowSize(w);

    Screen('CloseAll');
    imtype='XXX';
    if makecolorim==1
        if makehighcontrastim==1
            imtype='BCX';
        elseif makehighcontrastim==0
            imtype='GCX';
        end
    elseif makecolorim==0
        if makehighcontrastim==1
            imtype='BWX';
        elseif makehighcontrastim==0
            imtype='GWX';
        end
    end
    imgname=[num2str(ivx.nCalPoints) 'pt' num2str(h) 'x' num2str(v) 'off' num2str(shift(1)) 'x' num2str(shift(2)) 'sc' num2str(scale(1)) 'x' num2str(scale(2)) imtype ];
    imgpath=[myimgdir filesep imgname '.bmp'];
    fprintf('Saving BMP to %s\n', imgpath);
    imwrite(imageArray, imgpath, 'bmp');

    fprintf('\nEnd of image generation.\n');
catch
    %this "catch" section executes in case of an error in the "try" section
    %above.  Importantly, it closes the onscreen window if its open.
    %     pnet('closeall');
    Screen('CloseAll');    rethrow(lasterror);
end %try..catch..

