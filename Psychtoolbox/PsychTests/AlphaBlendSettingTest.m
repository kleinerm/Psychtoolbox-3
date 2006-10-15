function failFlag=AlphaBlendSettingTest(screenNumber)

% failFlag=AlphaBlendSettingTest([screenNumber])
%
% Test that Screen('BlendFunction') recalls the same alpha blending values
% as previoulsy set.
%
% If no return argument is provided, then TestAlphaBlendSetting issues an error
% when a test fails.  If a return argument is supplied then it signals a
% failed test only by returning true, without issuing an error.  
%
% See also: AlphaBlendingTest, PsychAlphaBlending


% HISTORY
%
% mm/dd/yy
%
% 1/28/05   awi  Wrote it.



if nargin==0
    screenNumber=max(Screen('Screens'));
end
exitOnError=nargout < 1;
failFlag=0;

background=127;
w=Screen('OpenWindow', screenNumber, [], [], [], 2);
Screen('FillRect', w, background);
wRect=Screen('Rect',w);




%*************** test that we can set and read all allowable blending factors and combinations.

sourceBlendFactors={ 'GL_ZERO', 'GL_ONE', 'GL_DST_COLOR', 'GL_ONE_MINUS_DST_COLOR', 'GL_SRC_ALPHA', 'GL_ONE_MINUS_SRC_ALPHA', 'GL_DST_ALPHA', 'GL_ONE_MINUS_DST_ALPHA', 'GL_SRC_ALPHA_SATURATE'};
destinationBlendFactors={'GL_ZERO','GL_ONE', 'GL_SRC_COLOR', 'GL_ONE_MINUS_SRC_COLOR', 'GL_SRC_ALPHA', 'GL_ONE_MINUS_SRC_ALPHA', 'GL_DST_ALPHA', 'GL_ONE_MINUS_DST_ALPHA'};

numSourceFactors=length(sourceBlendFactors);
numDestinationFactors=length(destinationBlendFactors);
[initialSourceFactor, initialDestinationFactor]=Screen('BlendFunction', w);
for iSource=1:numSourceFactors
    for iDestination=1:numDestinationFactors
 
        if exitOnError
            fprintf('Setting blend function:\n');
            fprintf(['  source:      ' sourceBlendFactors{iSource} '\n']);
            fprintf(['  destination: ' destinationBlendFactors{iDestination} '\n']);
        end
        %test setting both source and destination
        Screen('BlendFunction', w, sourceBlendFactors{iSource}, destinationBlendFactors{iDestination});
        [sourceReport, destinationReport]=Screen('BlendFunction', w);
        if ~strcmp(sourceReport, sourceBlendFactors{iSource})
            if exitOnError
                error('Screen(''BlendFunction'') failed to read back previosly set source value');
            end
            failFlag=1;
        end
        if ~strcmp(destinationReport, destinationBlendFactors{iDestination})
            error('Screen(''BlendFunction'') failed to read back previosly set destination value');
        end
        %test setting only the destination
        Screen('BlendFunction', w, [], destinationBlendFactors{iDestination});
        [sourceReport, destinationReport]=Screen('BlendFunction', w);
        if ~strcmp(sourceReport, sourceBlendFactors{iSource})
            if exitOnError
                error('Screen(''BlendFunction'') failed to read back previosly set source value');
            end
            failFlag=1;
        end
        if ~strcmp(destinationReport, destinationBlendFactors{iDestination})
            if exitOnError
                error('Screen(''BlendFunction'') failed to read back previosly set destination value');
            end
            failFlag=1;
        end
        %test setting only the source using the empty matrix to indicate
        %default
        Screen('BlendFunction', w, sourceBlendFactors{iSource}, []);
        [sourceReport, destinationReport]=Screen('BlendFunction', w);
        if ~strcmp(sourceReport, sourceBlendFactors{iSource})
            if exitOnError
                error('Screen(''BlendFunction'') failed to read back previosly set source value');
            end
            failFlag=1;
        end
        if ~strcmp(destinationReport, destinationBlendFactors{iDestination})
            error('Screen(''BlendFunction'') failed to read back previosly set destination value');
        end
        %test setting only the source using the no argument to indicate
        %default
        Screen('BlendFunction', w, sourceBlendFactors{iSource});
        [sourceReport, destinationReport]=Screen('BlendFunction', w);
        if ~strcmp(sourceReport, sourceBlendFactors{iSource})
            if exitOnError
                error('Screen(''BlendFunction'') failed to read back previosly set source value');
            end
            failFlag=1;
        end
        if ~strcmp(destinationReport, destinationBlendFactors{iDestination})
            if exitOnError
                error('Screen(''BlendFunction'') failed to read back previosly set destination value');
            end
            failFlag=1;
        end
        if exitOnError
            fprintf('Passed');
            fprintf('\n\n');
        end
    end
end
Screen('BlendFunction', w, initialSourceFactor, initialDestinationFactor);
        
Screen('Close', w);        


