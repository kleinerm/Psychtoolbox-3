function isImplanted=IsStartUpdateImplantedInFinish

% isImplanted=IsStartUpdateImplantedInFinish
%
% OS X: ___________________________________________________________________
%
% Return TRUE if both the finish.m file is on the MATLAB path and if it 
% includes a call to StartUpdateProcess.
%
% The OS X update process causes suspension of MATLAB for up to about 13ms
% at 30-second intervals.  Psychtoolbox functions Rush and Priority
% automatically kill and restart the update process when priority is,
% respectively, raised above 0 and lowered back to 0.  To insure that the
% update process is restored when MATLAB exits we add a call to
% StartUpdateProcess to the finish.m file which MATLAB calls before
% exiting. If multiple finish.m files exist on the MATLAB path then  MATLAB
% seems to invoke only the first one which it finds in the path search
% order as reported by the "path" command or as shown in the "set path"
% window.
%
% The Psychtoolbox includes the file:
%   Psychtoolbox/PsychInitialize/finish.m
%
% Because it is possible that users could modify path settings to include a
% custom finish.m file ahead of the Psychtoolbox finish.m file in the
% MATLAB path, we check that the finish.m file includes the
% StartUpdateProcess call. 
%
% Neither Rush not Priority will raise the priorty above 0 if
% IsStartUpdatedImplantedInFinsh returns FALSE.  This helps is a precaution
% against exiting MATLAB and leaving the update process shut down. However,
% it is not foolproof.  If MATLAB crashes while priority is >0 then it will
% not restart update on quitting.  
%
% If you have your own finish.m file which you want to use, then place it
% ahead of the Psychtoolbox finish.m file on MATLAB's search path and add
% to your file the line:
%   StartUpdateProcess;     %Restore the system update process.
%
% OS 9: ___________________________________________________________________
% 
% IsStartUpdateImplantedInFinish does not exist in OS 9. 
% 
% WINDOWS:_________________________________________________________________
%
% IsStartUpdateImplantedInFinish does not exist in Windows.
%
% _________________________________________________________________________
%
% see also: finish, StartUpdateProcess, KillUpdateProcess, Priority, Rush.


commandString='StartUpdateProcess';
commentChar='%';

pth=which('finish.m');
if isempty(pth)
    isImplanted=0;
else
%check that the finish file contains the StartUpdateProcessCall and don't
%be fooled by commented-out linese.
%we implement these as a series of filters.
    %read the file contents and breack into lines
    f=fopen(pth);
    cDoubles=fread(f);
    fclose(f);
    cChars=char(cDoubles');
    cLines=BreakLines(cChars);
    %find the indices of those lines which contain the the string
    %"StartUpdateProcess".  These are all candidates but some
    % might just be comments.
    occuranceIndices=[];
    for i =1:length(cLines)
        if ~isempty(strfind(cLines{i}, commandString))
            occuranceIndices=[occuranceIndices i];
        end
    end
    %build a cell array of the line fragments before the comment character.
    nonCommentFragments={};
    for i=length(occuranceIndices)
        commentCharIndices=find(cLines{occuranceIndices(i)}==commentChar);
        if isempty(commentCharIndices);
            %add the line to the list of candidate fragments if there is no
            %comment character
            nonCommentFragments{i}=cLines{occuranceIndices(i)};
        else
            %add to the list of candidate fragments the portion of the line
            %ahead of the comment character if there is one. 
            temp=cLines{occuranceIndices(i)};
            nonCommentFragments{i}=temp(1:commentCharIndices(1));
        end
    end
    %search the list of candidate fragments for the command string.  This
    %is imperfect because it can be fooled if the command is a substring of
    %a longer command.  But its probably good enough.
    numFoundCalls=0;
    foundCallsLineIndices=[];
    for i=length(occuranceIndices)
        foundIndices=strfind(nonCommentFragments{i},commandString );
        if ~isempty(foundIndices)
            numFoundCalls = numFoundCalls+1;
            foundCallsLineIndices = [foundCallsLineIndices i];
        end
    end
    if numFoundCalls > 1
        % we can't print multiline warnings using warning so just print the
        % the word "warning"
        fprintf('Warning: The finish.m script:\n');
        fprintf(['\t' pth '\n']);
        fprintf(['seems to call "' commandString '" ' int2str(numFoundCalls) ' times.  Only one call is necessary\n']);
     end
    isImplanted = numFoundCalls;
 end       
    
        
    

