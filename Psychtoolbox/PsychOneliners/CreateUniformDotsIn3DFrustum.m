function [x,y,z] = CreateUniformDotsIn3DFrustum(ndots,FOV,aspectr,depthrangen,depthrangef)
% [x,y,z] = CreateUniformDotsIn3DFrustum(ndots,FOV,aspectr,depthrangen,depthrangef)
%
% Sample dots in frustum uniformly
%
% z is not sampled from a uniform distribution, but from a parabolic
% distribution as the area of crosssections of the frustum is a quadratic
% function of the depth plane's depth ( (z*tan(FOV/2))^2 * aspectr )
%
% Here, I use Inverse transform sampling to transform a uniform random
% variable into the quadratic shape random variable
% see Luc Devroye. Non-Uniform Random Variate Generation. New York:
% Springer-Verlag, 1986. Chapter 2
% (http://cg.scs.carleton.ca/~luc/chapter_two.pdf)
% compile following in latex to see full derivation:
% ----
% \documentclass[12pt,a4paper]{minimal}
% \usepackage{amsmath}        % math
% 
% \begin{document}
% \textbf{Derivation}:\\
% Use pdf related to cross-section surface of frustum:
% \(\left(z \cdot \tan\left(\frac{FOV}{2}\right)\right)^2 \cdot aspectr\)\\\
% \(z_1\) is the distance of the near depth plane\\
% \(z_2\) is the distance of the far depth plane\\
% \(y\) is a uniform random variable\\
% Given: \(F(z_2)=1\) and \(F(z_1)=0\).
% \begin{align}
%     F(z) &= \int\limits^z_{z_1} k z^2 \, \mathrm{d}z\\
%     F(z) &= \frac{k}{3} \left(z^3 - z_1^3\right)\\
%     k    &= \frac{3}{z_2^3-z_1^3}\\
%     F(z) &= \frac{z^3-z_1^3}{z_2^3-z_1^3}\\
% \end{align}
% Substitute \(y\) for \(F(z)\) and factor out \(z\):
% \begin{align}
%     z^3  &= y\left(z_2^3-z_1^3\right) + z_1^3\\
%     z    &= \sqrt[3]{y\left(z_2^3-z_1^3\right) + z_1^3}
% \end{align}
% 
% \end{document}
% ----

% 2008       DN  Wrote it.
% 2009-06-06 DN  Changed input check to allow for vector near and far
%                depthrange and allowed near and far depthrange to be
%                the same, to place dots at exactly that depth

psychassert(all(depthrangen<=depthrangef),'Near clipping plane should be closer than far clipping plane');

u   = RandLim([1,ndots],0,1);                                           % get uniform random variable
z   = -(u.*(depthrangef.^3-depthrangen.^3)+depthrangen.^3).^(1/3);      % transform to parabolar distribution (negate as depth postiion is a negative number

yrs = -z*tand(FOV/2);
y   = RandLim([1,ndots],-yrs,yrs);
xrs = yrs * aspectr;
x   = RandLim([1,ndots],-xrs,xrs);

% for a uniform sample over z:
% z   = -RandLim([1,ndots],depthrangen,depthrangef);
