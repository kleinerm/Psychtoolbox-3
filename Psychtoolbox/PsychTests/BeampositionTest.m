function BeampositionTest(n, screenid)


AssertOpenGL;

if nargin < 1
   n = 10000;
end

if nargin < 2
   screenid = max(Screen('Screens'));
end

screenid

w=Screen('OpenWindow', screenid, 0);
Screen('Flip',w);
beampos = zeros(n, 1);
t = zeros(n, 1);
for i=1:n
   beampos(i) = Screen('GetWindowInfo', w, 1);
   t(i) = GetSecs;
end

while Screen('GetWindowInfo', w, 1) < 500; end;
ta=GetSecs;
while Screen('GetWindowInfo', w, 1)< 515; end;
te=GetSecs;

tel = (te - ta) * 1000


Screen('CloseAll');

figure;
plot(beampos);
figure;
plot(diff(t));
figure;
hist(beampos, 1500);
figure;
plot(t, beampos);

return;


