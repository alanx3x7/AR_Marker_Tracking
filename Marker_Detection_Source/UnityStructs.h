/*  EN.601.654 Augmented Reality
 *	Final Project Marker Detection Code
 *	Structs required for Unity C# integration
 *	Alan Lai, alai13@jhu.edu
 *	2020/05/02
 */

#pragma once


/*  Structure that holds pixel values as RGBA */
struct Color32
{
	uchar r;				// Value of R
	uchar g;				// Value of G
	uchar b;				// Value of B
	uchar a;				// Value of alpha
};


/*  Structure that holds characteristics of a detected marker */
struct Marker2
{
	int id;					// Marker ID
	float distance;			// Distance from marker to camera
	float center_x;			// x coordinate of marker center
	float center_y;			// y coordinate of marker center
	float translate_x;		// x coordinate of translation of marker
	float translate_y;		// y coordinate of translation of marker
	float translate_z;		// z coordinate of translation of marker
	float rotate_11;		// Value in row 1 column 1 of rotation matrix
	float rotate_12;		// Value in row 1 column 2 of rotation matrix
	float rotate_13;		// Value in row 1 column 3 of rotation matrix
	float rotate_21;		// Value in row 2 column 1 of rotation matrix
	float rotate_22;		// Value in row 2 column 2 of rotation matrix
	float rotate_23;		// Value in row 2 column 3 of rotation matrix
	float rotate_31;		// Value in row 3 column 1 of rotation matrix
	float rotate_32;		// Value in row 3 column 2 of rotation matrix
	float rotate_33;		// Value in row 3 column 3 of rotation matrix
};