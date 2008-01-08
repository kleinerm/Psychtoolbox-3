/*
 *
 * cvEyeTracker is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * cvEyeTracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cvEyeTracker; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * cvEyeTracker - Version 1.2.5
 * Part of the openEyes ToolKit -- http://hcvl.hci.iastate.edu/openEyes
 * Release Date:
 * Authors : Dongheng Li <dhli@iastate.edu>
 *           Derrick Parkhurst <derrick.parkhurst@hcvl.hci.iastate.edu>
 *           Jason Babcock <babcock@nyu.edu>
 *           David Winfield <dwinfiel@iastate.edu>
 * Copyright (c) 2004-2006
 * All Rights Reserved.
 *
 */


#ifndef _REMOVE_CORNEAL_REFLECTION_H
#define _REMOVE_CORNEAL_REFLECTION_H

#include "cv.h"

#define UINT8 unsigned char
#ifndef PI
#define PI 3.141592653589
#endif


void remove_corneal_reflection(IplImage *image, IplImage *threshold_image, int sx, int sy, int window_size, 
     int biggest_crr, int &crx, int &cry, int& crr);

void locate_corneal_reflection(IplImage *image, IplImage *threshold_image, int sx, int sy, int window_size, 
     int biggest_crar, int &crx, int &cry, int &crar);

int fit_circle_radius_to_corneal_reflection(IplImage *image, int cx, int cy, int crar, int biggest_crar, 
    double *sin_array, double *cos_array, int array_len);

void interpolate_corneal_reflection(IplImage *image, int cx, int cy, int crr, double *sin_array, 
    double *cos_array, int array_len);

#endif
