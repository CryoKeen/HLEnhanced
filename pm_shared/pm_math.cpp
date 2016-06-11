/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
// pm_math.c -- math primitives

#include "mathlib.h"
#include "vector.h"
#include "const.h"
#include <math.h>

// up / down
#define	PITCH	0
// left / right
#define	YAW		1
// fall over
#define	ROLL	2 

#define DotProduct(x,y) ((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])

#pragma warning(disable : 4244)

float	anglemod(float a)
{
	a = (360.0/65536) * ((int)(a*(65536/360.0)) & 65535);
	return a;
}

void AngleVectors (const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up)
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;
	
	angle = angles[YAW] * (M_PI*2 / 360);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[PITCH] * (M_PI*2 / 360);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[ROLL] * (M_PI*2 / 360);
	sr = sin(angle);
	cr = cos(angle);

	if (forward)
	{
		forward[0] = cp*cy;
		forward[1] = cp*sy;
		forward[2] = -sp;
	}
	if (right)
	{
		right[0] = (-1*sr*sp*cy+-1*cr*-sy);
		right[1] = (-1*sr*sp*sy+-1*cr*cy);
		right[2] = -1*sr*cp;
	}
	if (up)
	{
		up[0] = (cr*sp*cy+-sr*-sy);
		up[1] = (cr*sp*sy+-sr*cy);
		up[2] = cr*cp;
	}
}

void AngleVectorsTranspose (const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up)
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;
	
	angle = angles[YAW] * (M_PI*2 / 360);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[PITCH] * (M_PI*2 / 360);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[ROLL] * (M_PI*2 / 360);
	sr = sin(angle);
	cr = cos(angle);

	if (forward)
	{
		forward[0]	= cp*cy;
		forward[1]	= (sr*sp*cy+cr*-sy);
		forward[2]	= (cr*sp*cy+-sr*-sy);
	}
	if (right)
	{
		right[0]	= cp*sy;
		right[1]	= (sr*sp*sy+cr*cy);
		right[2]	= (cr*sp*sy+-sr*cy);
	}
	if (up)
	{
		up[0]		= -sp;
		up[1]		= sr*cp;
		up[2]		= cr*cp;
	}
}

#ifndef DISABLE_VEC_FUNCS
void AngleMatrix (const vec3_t angles, float (*matrix)[4] )
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;
	
	angle = angles[YAW] * (M_PI*2 / 360);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[PITCH] * (M_PI*2 / 360);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[ROLL] * (M_PI*2 / 360);
	sr = sin(angle);
	cr = cos(angle);

	// matrix = (YAW * PITCH) * ROLL
	matrix[0][0] = cp*cy;
	matrix[1][0] = cp*sy;
	matrix[2][0] = -sp;
	matrix[0][1] = sr*sp*cy+cr*-sy;
	matrix[1][1] = sr*sp*sy+cr*cy;
	matrix[2][1] = sr*cp;
	matrix[0][2] = (cr*sp*cy+-sr*-sy);
	matrix[1][2] = (cr*sp*sy+-sr*cy);
	matrix[2][2] = cr*cp;
	matrix[0][3] = 0.0;
	matrix[1][3] = 0.0;
	matrix[2][3] = 0.0;
}

void AngleIMatrix (const vec3_t angles, float matrix[3][4] )
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;
	
	angle = angles[YAW] * (M_PI*2 / 360);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[PITCH] * (M_PI*2 / 360);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[ROLL] * (M_PI*2 / 360);
	sr = sin(angle);
	cr = cos(angle);

	// matrix = (YAW * PITCH) * ROLL
	matrix[0][0] = cp*cy;
	matrix[0][1] = cp*sy;
	matrix[0][2] = -sp;
	matrix[1][0] = sr*sp*cy+cr*-sy;
	matrix[1][1] = sr*sp*sy+cr*cy;
	matrix[1][2] = sr*cp;
	matrix[2][0] = (cr*sp*cy+-sr*-sy);
	matrix[2][1] = (cr*sp*sy+-sr*cy);
	matrix[2][2] = cr*cp;
	matrix[0][3] = 0.0;
	matrix[1][3] = 0.0;
	matrix[2][3] = 0.0;
}
#endif

void NormalizeAngles( float *angles )
{
	int i;
	// Normalize angles
	for ( i = 0; i < 3; i++ )
	{
		if ( angles[i] > 180.0 )
		{
			angles[i] -= 360.0;
		}
		else if ( angles[i] < -180.0 )
		{
			angles[i] += 360.0;
		}
	}
}

/*
===================
InterpolateAngles

Interpolate Euler angles.
FIXME:  Use Quaternions to avoid discontinuities
Frac is 0.0 to 1.0 ( i.e., should probably be clamped, but doesn't have to be )
===================
*/
void InterpolateAngles( float *start, float *end, float *output, float frac )
{
	int i;
	float ang1, ang2;
	float d;
	
	NormalizeAngles( start );
	NormalizeAngles( end );

	for ( i = 0 ; i < 3 ; i++ )
	{
		ang1 = start[i];
		ang2 = end[i];

		d = ang2 - ang1;
		if ( d > 180 )
		{
			d -= 360;
		}
		else if ( d < -180 )
		{	
			d += 360;
		}

		output[i] = ang1 + d * frac;
	}

	NormalizeAngles( output );
}
 
#ifndef DISABLE_VEC_FUNCS
float Length( const vec3_t v )
{
	int		i;
	float	length = 0.0f;

	for( i = 0; i< 3; i++ )
		length += v[ i ] * v[ i ];
	length = sqrt( length );		// FIXME

	return length;
}
#endif

/*
===================
AngleBetweenVectors

===================
*/
float AngleBetweenVectors( const vec3_t v1, const vec3_t v2 )
{
	float angle;
	float l1 = Length( v1 );
	float l2 = Length( v2 );

	if ( !l1 || !l2 )
		return 0.0f;

	angle = acos( DotProduct( v1, v2 ) ) / (l1*l2);
	angle = ( angle  * 180.0f ) / M_PI;

	return angle;
}

#ifndef DISABLE_VEC_FUNCS
void VectorTransform (const vec3_t in1, float in2[3][4], vec3_t out)
{
	out[0] = DotProduct(in1, in2[0]) + in2[0][3];
	out[1] = DotProduct(in1, in2[1]) + in2[1][3];
	out[2] = DotProduct(in1, in2[2]) + in2[2][3];
}

void VectorMA (const vec3_t veca, float scale, const vec3_t vecb, vec3_t vecc)
{
	vecc[0] = veca[0] + scale*vecb[0];
	vecc[1] = veca[1] + scale*vecb[1];
	vecc[2] = veca[2] + scale*vecb[2];
}
#endif

#ifndef DISABLE_VEC_FUNCS
void CrossProduct (const vec3_t v1, const vec3_t v2, vec3_t cross)
{
	cross[0] = v1[1]*v2[2] - v1[2]*v2[1];
	cross[1] = v1[2]*v2[0] - v1[0]*v2[2];
	cross[2] = v1[0]*v2[1] - v1[1]*v2[0];
}
#endif

float Distance(const float* v1, const float* v2)
{
	Vector d;
	VectorSubtract(v2,v1,d);
	return Length(d);
}

#ifndef DISABLE_VEC_FUNCS
float VectorNormalize (vec3_t v)
{
	float	length, ilength;

	length = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
	length = sqrt (length);		// FIXME

	if (length)
	{
		ilength = 1/length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}
		
	return length;

}

void VectorScale (const vec3_t in, vec_t scale, vec3_t out)
{
	out[0] = in[0]*scale;
	out[1] = in[1]*scale;
	out[2] = in[2]*scale;
}
#endif


int Q_log2(int val)
{
	int answer=0;
	while (val>>=1)
		answer++;
	return answer;
}

void VectorMatrix( vec3_t forward, vec3_t right, vec3_t up)
{
	vec3_t tmp;

	if (forward[0] == 0 && forward[1] == 0)
	{
		right[0] = 1;	
		right[1] = 0; 
		right[2] = 0;
		up[0] = -forward[2]; 
		up[1] = 0; 
		up[2] = 0;
		return;
	}

	tmp[0] = 0; tmp[1] = 0; tmp[2] = 1.0;
	CrossProduct( forward, tmp, right );
	VectorNormalize( right );
	CrossProduct( right, forward, up );
	VectorNormalize( up );
}


#ifndef DISABLE_VEC_FUNCS
void VectorAngles( const vec3_t forward, vec3_t angles )
{
	float	tmp, yaw, pitch;
	
	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 90;
		else
			pitch = 270;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrt (forward[0]*forward[0] + forward[1]*forward[1]);
		pitch = (atan2(forward[2], tmp) * 180 / M_PI);
		if (pitch < 0)
			pitch += 360;
	}
	
	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}
#endif
