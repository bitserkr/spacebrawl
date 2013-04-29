/* Start Header ----------------------------------------------------------------

Copyright (C) 2011 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
File Name:      phong.vs
Purpose:        Vertex shader code for Phong lighting.
Language:       C++, Microsoft C/C++ Compiler
Platform:       Microsoft C/C++ Optimizing Compiler
                Version 16.00.30319.01 for 80x86
                Requires Microsoft Windows
Project:        anggono.paul_CS300_1
Author:         Paul Anggono, anggono.paul, 250002409
Creation date:  2011-09-27

- End Header -----------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * Varying vars -- Interpolated fragment-wise across triangle */
varying vec3 v3_pos;       /* Fragment position       */
varying vec2 v2_texCoords; /* Fragment texture coords */
varying vec3 v3_normal;    /* Normal at fragment      */

/*------------------------------------------------------------------------------
 * Vertex shader code */
void main()
{
	gl_Position   =  gl_ModelViewProjectionMatrix * gl_Vertex;
    v3_pos        = (gl_ModelViewMatrix * gl_Vertex).xyz;
	gl_FrontColor =  gl_Color;
	v2_texCoords  =  gl_MultiTexCoord0.st;
	v3_normal     =  gl_NormalMatrix * gl_Normal;
}
