% LineStippleDemo
%
% Demostrate dashed and dotted lines in using Screen commands "LineStipple"
% and "DrawLine".
%
%
% HISTORY
%   11/19/06  dhb  Remove OSX from name.  No info present about who wrote
%                  in the first place.

% Open a window
s=max(Screen('Screens'));
w=Screen('OpenWindow', s);
[wWidth, wHeight]=Screen('WindowSize', w);
Screen('FillRect', w, 180);

% Make up test values
numDemoLines=10;
lineWidths{1}=6;
lineWidths{2}=3;
xPositions=round(linspace(0, wWidth, numDemoLines+2));
xPositions=xPositions(2:end-1);
demoLines{1}=[1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0];
demoLines{2}=[1 1 1 1 0 0 0 0 1 1 1 1 0 0 0 0];  
demoLines{3}=[1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0];
demoLines{4}=[1 1 1 1 0 0 0 1 1 1 1 0 0 0 0 0];
myColors{1}=[255 0 0];
myColors{2}=[0 255 0];
myColors{3}=[0 0 255];
myColors{4}=0;

stretchFactors{1}=1;
stretchFactors{2}=1;
stretchFactors{3}=1;


%Screen 'LineStipple' reads back the current state.
[isStippleEnabled, stippleStretchFactor, stipplePattern]=Screen('LineStipple',w);
enabledStrings={'No', 'Yes'};
fprintf('Screen ''LineStipple'' reads back the current stippling state for a window.\n');
fprintf('  Reading back the default state, unchanged after opening a new window:\n');
fprintf(['    enabled:          ' enabledStrings{isStippleEnabled+1} '\n']);
fprintf(['    stretch factor:   ' num2str(stippleStretchFactor) '\n']); 
fullPatternImage=repmat('_', 1, 16);
patternImage=fullPatternImage;
patternImage(~stipplePattern)=' ';
fprintf(['    pattern:          ' patternImage '\n']);
fprintf(['    full comparison:  ' fullPatternImage  '\n']);
fprintf('\n');

%Screen 'LineStipple' sets a new state.
newStippleEnabled=1;
newStippleStretchFactor=3;
newStipplePattern=demoLines{2};
fprintf('Screen ''LineStipple'' also sets a new stipple state for a window.\n');
fprintf('  Setting the new stipple state:\n');
Screen('LineStipple',w, newStippleEnabled, newStippleStretchFactor, newStipplePattern);
fprintf(['    enabled:          ' enabledStrings{newStippleEnabled+1} '\n']);
fprintf(['    stretch factor:   ' num2str(newStippleStretchFactor) '\n']); 
fullPatternImage=repmat('_', 1, 16);
patternImage=fullPatternImage;
patternImage(~newStipplePattern)=' ';
fprintf(['    pattern:          ' patternImage '\n']);
fprintf(['    full comparison:  ' fullPatternImage  '\n']);
fprintf('\n');

%Screen 'LineStipple' reads back the new state.
[isStippleEnabled, stippleStretchFactor, stipplePattern]=Screen('LineStipple',w);
fprintf('  Reading back changed state, modified after opening a new window:\n');
fprintf(['    enabled:          ' enabledStrings{isStippleEnabled+1} '\n']);
fprintf(['    stretch factor:   ' num2str(stippleStretchFactor) '\n']); 
fullPatternImage=repmat('_', 1, 16);
patternImage=fullPatternImage;
patternImage(~stipplePattern)=' ';
fprintf(['    pattern:          ' patternImage '\n']);
fprintf(['    full comparison:  ' fullPatternImage  '\n']);
fprintf('\n');

fprintf('  Using Screen ''LineStipple'' and ''DrawLine'' to draw stippled lines... ');
Screen('LineStipple',w, 0); 
Screen('DrawLine', w, myColors{1}, xPositions(1), 0,  xPositions(1), wHeight, lineWidths{1});

Screen('LineStipple',w, 1, stretchFactors{1}, demoLines{1});
Screen('DrawLine', w, myColors{1}, xPositions(2), 0,  xPositions(2), wHeight, lineWidths{2});

Screen('LineStipple',w, 1, stretchFactors{1}, demoLines{2});
Screen('DrawLine', w, myColors{4}, xPositions(3), 0,  xPositions(3), wHeight, lineWidths{1});

Screen('LineStipple',w, 1, stretchFactors{1}, demoLines{3});
Screen('DrawLine', w, myColors{3}, xPositions(4), 0,  xPositions(4), wHeight, lineWidths{2});

Screen('LineStipple',w, 1, stretchFactors{2}, demoLines{1});
Screen('DrawLine', w, myColors{2}, xPositions(5), 0,  xPositions(5), wHeight, lineWidths{1});

Screen('LineStipple',w, 1, stretchFactors{2}, demoLines{2});
Screen('DrawLine', w, myColors{1}, xPositions(6), 0,  xPositions(6), wHeight, lineWidths{2});

Screen('LineStipple',w, 1, stretchFactors{2}, demoLines{3});
Screen('DrawLine', w, myColors{1}, xPositions(7), 0,  xPositions(7), wHeight, lineWidths{1});

Screen('LineStipple',w, 1, stretchFactors{3}, demoLines{1});
Screen('DrawLine', w, myColors{2}, xPositions(8), 0,  xPositions(8), wHeight, lineWidths{2});

Screen('LineStipple',w, 1, stretchFactors{3}, demoLines{2});
Screen('DrawLine', w, myColors{3}, xPositions(9), 0,  xPositions(9), wHeight, lineWidths{1});

Screen('LineStipple',w, 1, stretchFactors{2}, demoLines{4});
Screen('DrawLine', w, myColors{4}, xPositions(10), 0,  xPositions(10), wHeight, lineWidths{2});

Screen('Flip', w);
fprintf('Done.\n');
fprintf('\n');
WaitSecs(6);
Screen('CloseAll');



