function TextToStuffColorMismatchTest
% TextToStuffColorMismatchTest - Test if text is drawn
% in the color it was requested to be drawn.
%
% This demonstrates a bug in the OSX CoreText renderer,
% as used in released PTB versions since at least
% 30th December 2013.
%
% History:
%
% 16-Jun-2015  mk  Derived from code provided by user shandelman116.
% 14-Jul-2015  mk  Adapt to new situation that renderer 0 == CoreText on OSX.

%Setup
Screen('Preference', 'DefaultTextYPositionIsBaseline', 0);
Screen('Preference', 'TextAlphaBlending', 0);
Screen('Preference', 'TextAntiAliasing', 0);
if IsOSX
    % Renderer 0 = CoreText on OSX.
    Screen('Preference','TextRenderer', 0);
end

[window, wsize] = Screen('OpenWindow', 0, 0);

centerX = wsize(3)/2;
centerY = wsize(4)/2;

% Place cross hair cursor at screen center:
ShowCursor('CrossHair');
SetMouse(centerX, centerY);

Screen('TextFont', window, 'Arial');
Screen('TextSize', window, 126);

fixcross = sprintf('+');
greys = 0:15:255;

crosslines = [
    -30, 30, 0 0;
    0, 0, -30, 30;
    ];

crossLineCols = [];
crossTextCols = [];
crossLineCols2 = [];
crossTextCols2 = [];

%Drawing fixation cross as lines then as text
for grey = greys
    Screen('DrawLines', window, crosslines, 5, grey, [centerX, centerY]);
    Screen('Flip', window);
    crossLine = Screen('GetImage', window, [], [], [], 1);

    [dummy dummy textbounds] = DrawFormattedText(window, fixcross, 'center', 'center', grey);
    Screen('FrameRect', window, 255, textbounds);
    Screen('Flip', window);
    crossText = Screen('GetImage', window, [], [], [], 1);

    %Compile list of the color used for each type from a center pixel
    crossLineCols = [crossLineCols, crossLine(centerY, centerX)];
    crossTextCols = [crossTextCols, crossText(centerY, centerX)];
end

if 0
    % Now testing TextRenderer = 2 - FTGL plugin
    Screen('Preference','TextRenderer', 2);

    % Need to remove black for this part to work
    greys(1) = 1;

    for grey = greys
        Screen('DrawLines', window, crosslines, 5, grey, [centerX, centerY]);
        Screen('Flip', window);
        crossLine2 = Screen('GetImage', window, [], [], [], 1);
        DrawFormattedText(window, fixcross, 'center', 'center', grey);
        Screen('Flip', window);
        crossText2 = Screen('GetImage', window, [], [], [], 1);
        [rows, cols] = find(crossText2); %Need to use this since the centering is too off to pull the center pixel values
        %Compile list of the color used for each type from a center pixel
        crossLineCols2 = [crossLineCols2, crossLine2(centerY, centerX)];
        crossTextCols2 = [crossTextCols2, crossText2(rows(1), cols(1))];
    end
end

ShowCursor('Arrow');
sca;

% Restore to standard HQ OS specific text renderer 1:
Screen('Preference','TextRenderer', 1);

% Checking for TextRenderer 1
figure
plot(crossLineCols, crossTextCols, '*-');

if ~isequal(crossLineCols, crossTextCols)
    fprintf('\n\n\nDANGER WILL ROBINSON! Mismatch between requested and rendered text colors on text renderer!!!\n');
    hold on;
    plot([1/255:0.01:1] * 255, 255 * ([1/255:0.01:1].^(1/1.2)))
    hold off;

    crossTextCols
    crossLineCols
end

if ~isempty(crossTextCols2)
    %Checking with TextRenderer 2

    figure
    plot(crossLineCols2, crossTextCols2, '*-');

    if ~isequal(crossLineCols2, crossTextCols2)
        fprintf('\n\n\nDANGER WILL ROBINSON! Mismatch between requested and rendered text colors on text renderer 2!!!\n');
        crossTextCols2
        crossLineCols2
    end
end
