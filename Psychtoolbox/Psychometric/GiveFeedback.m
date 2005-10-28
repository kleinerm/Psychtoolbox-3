function GiveFeedback(correct)
% GiveFeedback(correct)
%
% Give auditory feedback about correctness of respose.  One beep for
% correct, two beeps for incorrect.  Some labs (e.g. dgp) prefer
% a short beep for correct, no sound for incorrect.  One could
% easily add a flag to this routine to allow this behavior.
%
% 3/5/97  	dhb  Wrote it
% 1/25/00 	emw  Added platform conditionals
% 3/8/2000	dgp  Fixed platform conditionals
% 4/14/00   dhb  Fix call to Snd for windows.
% 4/13/02	dgp	 Eliminate obsolete calls to SndPlay. Just call Snd on both platforms.
%				 It's important to specify the sample rate, because the default is
%				 platform-dependent.
% 11/15/03  dhb  Wait for sound to complete before returning.  Failure to do so
%				 was causing problems when Snd('Close') was called by calling
%				 routine.

if correct
      Snd('Play',sin(1:3:200),8192);
else
      Snd('Play',sin(1:3:200),8192);
      WaitSecs(0.1);
      Snd('Play',sin(1:3:200),8192);
end
Snd('Wait');
