/*
 *    PsychRects.c
 *
 *    PLATFORMS:
 *
 *      All.
 *
 *    AUTHORS:
 *
 *      Allen Ingling           awi     Allen.Ingling@nyu.edu
 *      Mario Kleiner           mk      mario.kleiner.de@gmail.com
 *
 *    HISTORY:
 *
 *      02/11/03                awi     Wrote it.
 *      01/20/09                mk      Modify behaviour of PsychGetCenterFromRectAbs(): No longer floor() the size of the rectangle
 *                                      before calculation of center: This removes information about rects with fractional locations
 *                                      for no reason. Function is currently used by FillOval, FrameOval, Frame/Fill/xxxArc(), where
 *                                      it has harmful effect on accuracy and consistency.
 *
 *    DESCRIPTION:
 *
 *      Helper to deal with common operations on rectangles.
 */

//begin include once
#ifndef PSYCH_IS_INCLUDED_PsychRects
#define PSYCH_IS_INCLUDED_PsychRects

//includes
#include "Screen.h"

#define kPsychLeft      0
#define kPsychTop       1
#define kPsychRight     2
#define kPsychBottom    3
typedef double PsychRectType[4];

//Accessors for PsychRectType
void            PsychMakeRect(double *psychRect, double left, double top, double right, double bottom);
psych_bool      IsPsychRectEmpty(PsychRectType rect);
psych_bool      ValidatePsychRect(PsychRectType rect);
double          PsychGetWidthFromRect(const double *rect);
double          PsychGetHeightFromRect(const double *rect);
void            PsychCenterRect(const double *innerRect, const double *outerRect, double *newRect);
void            PsychCenterRectInRect(const double *innerRect, const double *outerRect, double *newRect);
void            PsychCopyRect(double *toRect, const double *fromRect);
void            PsychGetCenterFromRectAbsolute(const double *rect, double *rectCenterX, double *rectCenterY);
void            PsychGetManhattanDistance(double x1, double y1, double x2, double y2, double *deltaX, double *deltaY);
void            PsychNormalizeRect(double *rect, double *normRect);
void            PsychInvertRectY(double *rectB, double *rectA, double *frameA);
psych_bool      PsychMatchRect(double *rectA, double *RectB);
void            PsychFindEnclosingTextureRect(double *rectA, double *enclosingRect);
//end include once
#endif
