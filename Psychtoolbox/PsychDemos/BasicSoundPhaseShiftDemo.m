function BasicSoundPhaseShiftDemo(showit, targetChannel)
% BasicSoundPhaseShiftDemo([showit=1][, targetChannel=1])
%
% Demonstrates how one can play back a phase-shifted sine tone, with dynamically
% adjustable phase shift, free of audible artifacts during phase shift change.
%
% This uses PsychPortAudio's realtime mixing and some trigonometric math to
% synthesize a cosine wave of selectable phase in realtime from the weighted sum
% of a pair of a cosine + sine wave with different relative amplitudes.
%
% The sine wave is put into one sound channel. A cosine wave of same frequency
% (but 90 degrees phase-shifted) is put in another sound channel. Both sound
% channels are mixed together in realtime by PsychPortAudio, with controllable
% relative volume (== amplitude == mix-weight) for each of the two channels, and
% the result of this two-channel weighted mix is output to the 2nd audio output
% channel of the actual soundcard, creating a synthesized phase-shifted cosine
% wave of same frequency. Changing the relative channel volumes changes the phase
% shift of the cosine output wave in realtime, without artifacts / discontinuities.
% The proper relative volume levels are computed from the desired phase shift and
% set in the internal subfunction updatePhase() by the magic of math.
%
% A second reference sine wave is output to the 1st audio channel of the soundcard,
% if 'targetChannel' == 1, so one can drive two speakers / transducers, one outputting
% a phase-shifted sine wave, relative to th other speaker / transducer. For testing
% purposes, one can set 'targetChannel' == 2 to mix the reference sine-wave with the
% phase-shifted cosine wave both into output speaker channel 2, to simulate potential
% constructive / destructive interference between reference and phase-shifted sine
% wave. Setting 'showit' == 1 will capture the audio signal send to both soundcard
% channel and visualize it in a Psychtoolbox onscreen window, for illustrative purposes
% and for basic debugging. If you want to run a real auditory experiments, you'd use
% 'targetChannel' == 1 and some external microphones and oscillograph to check for
% proper audio output independently of PsychPortAudio, Psychtoolbox and your computer.
%
% One use case for this are studies as described under this link...
% https://psychtoolbox.discourse.group/t/can-i-change-the-phase-of-a-sine-wave-with-psychportaudio/4086
% ...more specifically experiments like this one about air conduction and bone conduction
% of sound:
%
% https://www.researchgate.net/publication/6535047_Simultaneous_cancellation_of_air_and_bone_conduction_tones_at_two_frequencies_Extension_of_the_famous_experiment_by_von_Bekesy
%
% Have a look at BasicAMAndMixScheduleDemo on how to apply amplitude modulation (AM)
% of signals by attaching AM modulator slave devices to the pafixedsine and pashiftsine
% slave devices used here. A suitable time series of AM envelope values would allow to
% gate the output sine waves.
%
% This demo was sponsored by a paid support request. Thanks!
%
% Usage:
%
% ESCAPE key ends the demo.
%
% Left and right cursor keys allow to shift phase interactively.
%
% Optional parameters:
%
% 'showit' If set to 1, visualize actual output signals, channel 1 in red,
%          channel 2 in green. Defaults to 1 for showing output.
%
% 'targetChannel' To which channel should the fixed phase reference signal be
%                 output? 1 = channel 1 (default). 2 = channel 2 will output into
%                 the same channel as the phase-shifted signal, so both signals
%                 will show constructive or destructive interference, depending
%                 on phase-shift of the 2nd signal which always goes to channel 2.

% History:
% 29-Oct-2021 mk  Written.

  if nargin < 1 || isempty(showit)
    showit = 1;
  end

  if nargin < 2 || isempty(targetChannel)
    targetChannel = 1;
  else
    if ~ismember(targetChannel, [1,2])
      error('targetChannel argument must be 1 or 2.');
    end
  end

  % Setup defaults:
  PsychDefaultSetup(2);

  % Define control keys:
  ESCAPE = KbName('ESCAPE');
  leftArrow = KbName('LeftArrow');
  rightArrow = KbName('RightArrow');

  % Perform basic initialization of the sound driver:
  InitializePsychSound;

  % Number of physical channels to use on the real soundcard:
  nrchannels = 2;

  % Frequency of waves:
  freq = 500;

  % Initial phase shift in degrees of cosine wave:
  phase = 90;

  % Open and start physical sound card for playback (1) in master mode (8) = (1+8),
  % with low-latency and high timing precision (1), with auto-selected default []
  % samplingRate for device, to consume and output mixed sound from slaves to
  % 'nrchannels' physical output channels:
  pamaster = PsychPortAudio('Open', [], 1 + 8, 1, [], nrchannels);

  % Retrieve auto-selected samplingRate:
  status = PsychPortAudio('GetStatus', pamaster);
  samplingRate = status.SampleRate;

  % Compute minimum length 'wavedur' seconds of a sound buffer with one or
  % more repetitions of the freq Hz sine wave. If you wanted sound signals
  % of defined length, e.g., also to allow things like applying an AM
  % envelope function to it, by use of AM modulator slave devices, you
  % could just set wavedur to the desired sound duration of the total sound
  % vector. This here is just for memory efficiency...
  %
  % For the minimum duration 'wavedur', make sure to increase wavedur to
  % generate multiple period repetitions of the (co)sine wave ih order to
  % make it fit an integral number of samples, in case one period would
  % need a non-integral number of samples. If nothing else, it may help
  % visualization or debugging / reasoning about it: The if statement is
  % executed, e.g., for combinatios of 'freq' 500 Hz and samplingRate of
  % 44100 samples/sec, where one period of a sine wave would require 88.2
  % samples, ie a non-integral number. Repeating the wave 5x by increasing
  % wavedur * 5, ends up with 88.2 * 5 = 441 samples creating one sound
  % playback buffer with a even number of samples.
  wavedur = 1 / freq;
  nsamples = wavedur * samplingRate;
  if rem(nsamples, 1)
    wavedur = wavedur * 1 / rem(nsamples, 1);
  end

  % Define input vector 'support' for the sin() and cos() functions. Playback of
  % a 'freq' Hz pure sine tone at a sampling rate of 'samplingRate'. We create a
  % waveform of 'wavedur' duration, so that full periods of the sine / cosine fit
  % nicely for inifinitely looped playback, without discontinuities at the beginning
  % and end of the vector. If you also wanted to amplitude modulate the signals with
  % some envelope function, you'd have to create a longer 'support' vector, which
  % repeats the waves for not just one period, but often enough to cover the whole
  % signal duration for the amplitude envelope:
  support = 2 * pi * freq * (0:round(wavedur * samplingRate-1)) / samplingRate;

  if showit
    paoutputcapture = PsychPortAudio('OpenSlave', pamaster, 2 + 64);
    PsychPortAudio('GetAudioData', paoutputcapture, 1);
    PsychPortAudio('Start', paoutputcapture);
    win = PsychImaging('OpenWindow', 0, 0, [0, 0, Screen('Windowsize', 0), 200], [], [], [], [], [], kPsychGUIWindow + kPsychGUIWindowWMPositioned);
  end

  % Start master, wait (1) for start, return sound onset time in startTime.
  % Slaves can be independently controlled in their timing, volume, content thereafter:
  startTime = PsychPortAudio('Start', pamaster, [], [], 1);

  % Create slave device for infinite duration fixed sine tone of freq Hz playback (1).
  % It will output one audio channel (1) via audio channel 'targetChannel' (targetChannel)
  % of the real soundcard:
  pafixedsine = PsychPortAudio('OpenSlave', pamaster, 1, 1, targetChannel);

  % Sine tone, freq Hz:
  PsychPortAudio('FillBuffer', pafixedsine, 0.5 * sin(support));

  % Start playback with infinite (0) repetition of the 1 second sound signal,
  % at time startTime + 1 second:
  PsychPortAudio('Start', pafixedsine, 0, startTime + 1);

  % Create slave device for infinite duration phase-shiftable cosine tone of freq Hz playback (1).
  % It will output via audio channel 2 of the real soundcard. For this, we let the pashiftsine
  % slave device have two (2) sound channels, both feeding their sound as a mix into the 2nd channel
  % of the pamaster ([2, 2]):
  pashiftsine = PsychPortAudio('OpenSlave', pamaster, 1, 2, [2, 2]);

  % First pashiftsine channel for the mix has a cos() wave, second channel has a sin() wave,
  % 90 degrees phase-shifted. Both waves will be mixed together and output via the 2nd
  % master channel, but with different amplitude / volume. The weighted sum of both 90 degrees
  % shifted waves yields a (co)sine wave of 'freq' Hz, with a phase shifted according to the
  % relative amplitude of both waves. See subfunction updatePhase() for the math behind this:
  srcmixtones = [cos(support) ; sin(support)];
  PsychPortAudio('FillBuffer', pashiftsine, srcmixtones);

  % updatePhase() computes proper relative volumes / amplitudes for the two waves and
  % assign it as channel-volumes to the pashiftsine virtual audio device, so the mix
  % results in a 'phase' shifted cosine wave of 'freq' Hz:
  updatePhase(phase, pashiftsine);

  % Start infinite playback of the sound wave, with a peak volume of 50% aka 0.5,
  % at time startTime + 1 second:
  PsychPortAudio('Volume', pashiftsine, 0.5);
  PsychPortAudio('Start', pashiftsine, 0, startTime + 1);

  % Loop for keyboard checking and phase adjustment:
  while 1
    [down, ~, keys] = KbCheck(-1);
    if down
      if keys(ESCAPE)
        break;
      end

      if keys(rightArrow)
        % Phase-shift increase by 1 degree:
        phase = mod(phase + 1, 360);
        updatePhase(phase, pashiftsine);
      end

      if keys(leftArrow)
        % Phase-shift decrease by 1 degree:
        phase = mod(phase - 1, 360);
        updatePhase(phase, pashiftsine);
      end
    end

    if showit
      % Get exactly 0.1 seconds of captured sound:
      recorded = PsychPortAudio('GetAudioData', paoutputcapture, [], 0.1, 0.1);

      % Plot both audio tracks into the window:
      xpos = 1:size(recorded, 2);
      xpos = [xpos xpos(end:-1:1)];
      recorded = [recorded(:, 1:end) recorded(:, end:-1:1)];
      Screen('FramePoly', win, [1 0 0], [xpos' , [100 + recorded(1,:) * 100]']);
      Screen('FramePoly', win, [0 1 0], [xpos' , [100 + recorded(2,:) * 100]']);

      % Show it: Do not sync us to stimulus onset, so we won't slow down to display
      % refresh rate.
      Screen('Flip', win, [], [], 1);
    else
      % Nap a bit, so phase doesn't change that quickly - 50 msecs should do:
      WaitSecs('YieldSecs', 0.050);
    end
  end

  % Stop and close down everything audio related:
  PsychPortAudio('Close');
  if showit
    % Close window:
    sca;
  end

  % Optional plotting code:
  % close all;
  % plot(1:length(recorded), recorded(1,:), 'r', 1:length(recorded), recorded(2,:), 'b');

  % Done, bye bye.
end

function updatePhase(phase, pahandle)
  % Convert phase in degrees into radians:
  shift = phase * pi / 180;

  % Compute relative amplitudes / volumes for both waves, for a resulting
  % wave of amplitude 1.0 and 'phase' degrees (aka shift radians) shift:
  %
  % From https://cpb-us-e1.wpmucdn.com/cobblearning.net/dist/d/1007/files/2013/03/Linear-Combinations-and-Sum-Difference-1xsp3e8.pdf
  % we learn the following trick:
  %
  % Given a1 and a2, there is the following equivalence for the weighted sum of
  % a sine and cosine wave of identical frequency x = 2*pi*freq*t:
  %
  % a1 * cos(x) + a2 * sin(x) = A * cos(x - shift) with
  % A = sqrt(a1^2 + a2^2); and shift = arctan(a2 / a1)
  %
  % so for a desired 'shift' it follows that
  % a2 / a1 = tan(shift)  and as we know that tan(shift) = sin(shift) / cos(shift)
  %
  % therefore:
  %
  % a2 = sin(shift), and a1 = cos(shift), and
  % A = sqrt(sin(shift)^2 + cos(shift)^2) = sqrt(1) = 1, ie. A = 1.
  %
  a1 = cos(shift);
  a2 = sin(shift);

  % Assign new volumes / amplitudes atomically. As channel 1 of pahandle contains
  % a cos(x) wave and channel 2 contains a sin(x) wave, the mixing in PsychPortAudio
  % will create the weighted sum a1 * cos(x) + a2 * sin(x), which is equivalent
  % to an audio signal of cos(x - shift), ie. a cosine wave with 'shift' phase shift:
  PsychPortAudio('Volume', pahandle, [], [a1, a2]);
  fprintf('New phase: %f degrees.\n', phase);
end
