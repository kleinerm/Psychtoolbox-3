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

#ifdef PSYCHCV_USE_OPENCV

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "remove_corneal_reflection.h"

// Includes from PTB:
//
// It is important that these are included *last*, so remapping of some functions,
// e.g., printf() -> mexFunction() works correctly!
#include <Psych.h>
#include <PsychCV.h>

void remove_corneal_reflection(IplImage *image, IplImage *threshold_image, int sx, int sy, int window_size, int 
biggest_crr, int& crx, int& cry, int& crr)
{
  int crar = -1;	//corneal reflection approximate radius
  crx = cry = crar = -1;

  float angle_delta = 1*PI/180;
  int angle_num = (int)(2*PI/angle_delta);
  printf("(corneal reflection) sx:%d; sy:%d\n", sx, sy);
  double *angle_array = (double*)malloc(angle_num*sizeof(double));
  double *sin_array = (double*)malloc(angle_num*sizeof(double));
  double *cos_array = (double*)malloc(angle_num*sizeof(double));
  for (int i = 0; i < angle_num; i++) {
    angle_array[i] = i*angle_delta;
    sin_array[i] = sin(angle_array[i]);
    cos_array[i] = cos(angle_array[i]);
  }

  locate_corneal_reflection(image, threshold_image, sx, sy, window_size, (int)(biggest_crr/2.5), crx, cry, crar);
  crr = fit_circle_radius_to_corneal_reflection(image, crx, cry, crar, (int)(biggest_crr/2.5),  sin_array, cos_array, angle_num);
  crr = (int)(2.5*crr);
  interpolate_corneal_reflection(image, crx, cry, crr, sin_array, cos_array, angle_num);

  free(angle_array);
  free(sin_array);
  free(cos_array);
}

void locate_corneal_reflection(IplImage *image, IplImage *threshold_image, int sx, int sy, int window_size, int 
biggest_crar, int &crx, int &cry, int &crar)
{
  if (window_size%2 == 0) {
    printf("Error! window_size should be odd!\n");
  }

  int r = (window_size-1)/2;
  int startx = MAX(sx-r, 0);
  int endx = MIN(sx+r, image->width-1);
  int starty = MAX(sy-r, 0);
  int endy = MIN(sy+r, image->height-1);
  cvSetImageROI(image, cvRect(startx, starty, endx-startx+1, endy-starty+1));
  cvSetImageROI(threshold_image, cvRect(startx, starty, endx-startx+1, endy-starty+1));

  double min_value, max_value;
  CvPoint min_loc, max_loc; //location
  cvMinMaxLoc(image, &min_value, &max_value, &min_loc, &max_loc);

  int threshold, i;
  CvSeq* contour=NULL;
  CvMemStorage* storage = cvCreateMemStorage(0);
  double *scores = (double*)malloc(sizeof(double)*((int)max_value+1));
  memset(scores, 0, sizeof(double)*((int)max_value+1));
  int area, max_area, sum_area;
  for (threshold = (int)max_value; threshold >= 1; threshold--) {
    cvThreshold(image, threshold_image, threshold, 1, CV_THRESH_BINARY);
    cvFindContours(threshold_image, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);    
    max_area = 0;
    sum_area = 0;
    CvSeq *max_contour = contour;
    for( ; contour != 0; contour = contour->h_next) {
      area = contour->total + (int)(fabs(cvContourArea(contour, CV_WHOLE_SEQ)));
      sum_area += area;
      if (area > max_area) {
        max_area = area;
        max_contour = contour;
      }
    }
    if (sum_area-max_area > 0) {
      scores[threshold-1] = max_area / (sum_area-max_area);
      //printf("max_area: %d, max_contour: %d, sum_area: %d; scores[%d]: %lf\n", 
      //        max_area, max_contour->total, sum_area, threshold-1, scores[threshold-1]);      
    }
    else
      continue;

    if (scores[threshold-1] - scores[threshold] < 0) {
      //found the corneal reflection
      crar = (int)sqrt(max_area / PI);
      int sum_x = 0;
      int sum_y = 0;
      CvPoint *point;
      for (i = 0; i < max_contour->total; i++) {
        point = CV_GET_SEQ_ELEM(CvPoint, max_contour, i);
        sum_x += point->x;
        sum_y += point->y;
      }
      crx = sum_x/max_contour->total;
      cry = sum_y/max_contour->total;
      break;
    }
  }
  /*/ printf("(corneal reflection) max_value = %lf; threshold = %d\n", max_value, threshold);
  printf("(corneal reflection) Scores:\n");
  for (int i = (int)max_value; i >= threshold-1; i--) {
    printf("%6.2lf", scores[i]);
  }
  printf("\n");*/
    
  free(scores);
  cvReleaseMemStorage(&storage);
  cvResetImageROI(image);
  cvResetImageROI(threshold_image);

  if (crar > biggest_crar) {
    printf("(corneal) size wrong! crx:%d, cry:%d, crar:%d (should be less than %d)\n", crx, cry, crar, biggest_crar);
    cry = crx = -1;
    crar = -1;
  }

  if (crx != -1 && cry != -1) {
    printf("(corneal) startx:%d, starty:%d, crx:%d, cry:%d, crar:%d\n", startx, starty, crx, cry, crar);
    crx += startx;
    cry += starty;
  }
        
}

int fit_circle_radius_to_corneal_reflection(IplImage *image, int crx, int cry, int crar, int biggest_crar, double *sin_array, double *cos_array, int array_len)
{
  if (crx == -1 || cry == -1 || crar == -1)
    return -1;

  double *ratio = (double*)malloc((biggest_crar-crar+1)*sizeof(double));
  int i, r, r_delta=1; 
  int x, y, x2, y2;
  double sum, sum2;
  for (r = crar; r <= biggest_crar; r++) {
    sum = 0;
    sum2 = 0;
    for (i = 0; i < array_len; i++) {
      x = (int)(crx + (r+r_delta)*cos_array[i]);
      y = (int)(cry + (r+r_delta)*sin_array[i]);
      x2 = (int)(crx + (r-r_delta)*cos_array[i]);
      y2 = (int)(cry + (r+r_delta)*sin_array[i]);
      if ((x >= 0 && y >=0 && x < image->width && y < image->height) &&
          (x2 >= 0 && y2 >=0 && x2 < image->width && y2 < image->height)) {
        sum += *(image->imageData+y*image->width+x);
        sum2 += *(image->imageData+y2*image->width+x2);
      }
    }
    ratio[r-crar] = sum / sum2;
    if (r - crar >= 2) {
      if (ratio[r-crar-2] < ratio[r-crar-1] && ratio[r-crar] < ratio[r-crar-1]) {
        free(ratio);
        return r-1;
      }
    } 
  }
  
  free(ratio);
  printf("ATTN! fit_circle_radius_to_corneal_reflection() do not change the radius\n");
  return crar;
}

void interpolate_corneal_reflection(IplImage *image, int crx, int cry, int crr, double *sin_array, double *cos_array, 
int array_len)
{
  if (crx == -1 || cry == -1 || crr == -1)
    return;

  if (crx-crr < 0 || crx+crr >= image->width || cry-crr < 0 || cry+crr >= image->height) {
    printf("Error! Corneal reflection is too near the image border\n");
    return;
  }

  int i, r, r2,  x, y;
  UINT8 *perimeter_pixel = (UINT8*)malloc(array_len*sizeof(int));
  int sum=0, pixel_value;
  double avg;
  for (i = 0; i < array_len; i++) {
    x = (int)(crx + crr*cos_array[i]);
    y = (int)(cry + crr*sin_array[i]);
    perimeter_pixel[i] = (UINT8)(*(image->imageData+y*image->width+x));
    sum += perimeter_pixel[i];
  }
  avg = sum*1.0/array_len;

  for (r = 1; r < crr; r++) {
    r2 = crr-r;
    for (i = 0; i < array_len; i++) {
      x = (int)(crx + r*cos_array[i]);
      y = (int)(cry + r*sin_array[i]);
      *(image->imageData+y*image->width+x) = (UINT8)((r2*1.0/crr)*avg + (r*1.0/crr)*perimeter_pixel[i]);
    }
    //printf("r=%d: %d (avg:%lf, end:%d)\n", r, (UINT8)((r2*1.0/crr)*avg + (r*1.0/crr)*perimeter_pixel[i-1]),
    //       avg, perimeter_pixel[i-1]);
  }
  free(perimeter_pixel);
}

#endif
