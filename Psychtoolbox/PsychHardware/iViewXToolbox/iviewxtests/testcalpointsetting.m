% test calibration point setting
clear ivx;

ivx=initIViewXDefaults;

points=5;
shift=[0 0];
scale=[0 0];

ivx=iViewXSetCalPoints(ivx, points, shift, scale);

ivx.absCalPos

plot(ans(:,1),ans(:,2), 'r+')
hold on
scale=[-50 -50];

ivx=iViewXSetCalPoints(ivx, points, shift, scale);

ivx.absCalPos

plot(ans(:,1),ans(:,2), 'bo')

shift=[10 10];

ivx=iViewXSetCalPoints(ivx, points, shift, scale);

ivx.absCalPos

plot(ans(:,1),ans(:,2), 'g+')
hold off
