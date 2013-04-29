/* Start Header ----------------------------------------------------------------

Copyright (C) 2011 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
File Name:      phong.fs
Purpose:        Fragment shader code for Phong lighting.
Language:       C++, Microsoft C/C++ Compiler
Platform:       Microsoft C/C++ Optimizing Compiler
                Version 16.00.30319.01 for 80x86
                Requires Microsoft Windows
Project:        anggono.paul_CS300_1
Author:         Paul Anggono, anggono.paul, 250002409
Creation date:  2011-09-27

- End Header -----------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * Constants */
#define MAX_NUM_LIGHT_SRC 8
/* Light consts to be set by C program */
/* const uniform int  constLightType_Point; */
uniform int constLightType_Dir;
uniform int constLightType_Spot;

/*------------------------------------------------------------------------------
 * Local typedefs/structs */
struct LightContext
{
	int  type;
	vec3 position;
	vec3 direction;
	vec4 I_ambient;
	vec4 I_diffuse;
	vec4 I_specular;
	vec3 distAttCoeff;
	vec3 spotAttCoeff;
};

struct MaterialContext
{
	vec4  K_ambient;
	vec4  K_diffuse;
	vec4  K_specular;
	vec4  K_emissive;
	float shinePower;
};

struct FogContext
{
	vec4  color;
    float nearDist;
    float farDist;
};

/*------------------------------------------------------------------------------
 * Local prototypes */
float ComputeDistanceAtt(vec3 attValues, float distance);
float ComputeSpotlightEffect(vec3 spotlightValues, float cosOfAngle);
float ComputeFogCoeff(float nearDist, float farDist, float obj);
vec4  ComputeFragColor(LightContext light, MaterialContext material, vec3 fragNormal, vec3 fragPos);

/*------------------------------------------------------------------------------
 * Uniform vars */

/* Texture map samplers */
uniform sampler2D TexAmbient;  /* Ambient coefficient */
uniform sampler2D TexDiffuse;  /* Diffuse coefficient */
uniform sampler2D TexSpecular; /* Specular coefficient */
uniform sampler2D TexEmissive; /* Emissive coefficient */
uniform bool      isUseTextures;
uniform int         materialType;
uniform bool      useVertexColor;

/* Lighting params */
uniform vec4         I_globAmbient; /* Global ambience */
uniform LightContext lights[MAX_NUM_LIGHT_SRC];
uniform int          numLightSrc;
uniform FogContext   fog;
uniform MaterialContext surfaceClr;

/*------------------------------------------------------------------------------
 * Varying vars -- Interpolated fragment-wise across triangle */
varying vec3 v3_pos;       /* Fragment position       */
varying vec2 v2_texCoords; /* Fragment texture coords */
varying vec3 v3_normal;    /* Normal at fragment      */

/*------------------------------------------------------------------------------
 * Fragment shader code */
void main()
{
    MaterialContext mat;
	if (isUseTextures)
    {
        mat.K_ambient  = texture2D(TexAmbient,  v2_texCoords.st);
		mat.K_diffuse  = texture2D(TexDiffuse,  v2_texCoords.st);
		mat.K_specular = texture2D(TexSpecular, v2_texCoords.st);
        mat.K_emissive = texture2D(TexEmissive, v2_texCoords.st);
		mat.shinePower = mat.K_specular.a * 255.0;
    }
	else
	{
		mat.K_ambient = surfaceClr.K_ambient;
		mat.K_diffuse = surfaceClr.K_diffuse;
		mat.K_specular = surfaceClr.K_specular;
		mat.K_emissive = surfaceClr.K_emissive;
		mat.shinePower = mat.K_specular.a * 255.0;
	}
	// else
	// {
		// mat.K_ambient  = vec4(1.0, 1.0, 1.0, 0.16); /* White */
		// mat.K_diffuse  = vec4(1.0, 1.0, 1.0, 0.16); /* White */
		// mat.K_specular = vec4(1.0, 1.0, 1.0, 1.0); /* White */
		// mat.K_emissive = vec4(0.0, 0.0, 0.0, 0.0); /* Black */
		// mat.shinePower = 255.0;
	// }
    
	/*
	 * Lightsource-independent portion of Phong lighting.
	 * Fragment takes initial colour of global ambience on material plus any
	 * emissive property.
	 */
    vec4 fragColour = (I_globAmbient * mat.K_ambient) + mat.K_emissive;
	
	/* Iterate through active lightsources and compute colour intensities for each */
	for (int i = 0; i < numLightSrc ; ++i)
	{
        fragColour += ComputeFragColor(lights[i], mat, v3_normal, v3_pos);
	}
    
	float fogCoeff = ComputeFogCoeff(fog.nearDist, fog.farDist, v3_pos.z);
	
	switch (materialType)
	{
	case 4:
	default:
		if (useVertexColor)
			gl_FragColor = gl_Color;
		else
			gl_FragColor = ((1.0 - fogCoeff) * fragColour) + (fogCoeff * fog.color);
		break;
	
	case 0:
		gl_FragColor = mat.K_ambient;
		break;
	
	case 1:
		gl_FragColor = mat.K_diffuse;
		break;
	
	case 2:
		gl_FragColor = mat.K_specular;
		break;
	
	case 3:
		gl_FragColor = mat.K_emissive;
		break;
	}
}

/**
* \fn    float ComputeDistanceAtt(vec3 attValues, float dist)
* \brief Computes distance attenuation coefficient.
*
* \param[in] attValues (vec3)
*   Vector of attenuation coefficients. The x component holds the constant
*   coefficient; y component holds linear coefficient; z component holds
*   quadratic coefficient.
*
* \param[in] dist (float)
*   Distance between light and fragment.
*
* \return (float)
*   Distance attenuation coefficient.
*/
float ComputeDistanceAtt(vec3 attValues, float dist)
{
	return min(1.0, 1.0 / (attValues.x + attValues.y * dist + attValues.z * dist * dist));
}

/**
* \fn    float ComputeSpotlightEffect(vec3 spotlightValues, float cosOfAngle)
* \brief Computes spotlight attenuation coefficient.
*
* \param[in] spotlightValues (vec3)
*   The x component holds the cosine of the inner angle of the spotlight.
*   The y component holds the cosine of the outer angle of the spotlight.
*   The z component holds the spotlight falloff value.
*
* \param[in] cosOfAngle (float)
*   Cosine of the angle between spotlight's direction and vector from spotlight
*   to fragment.
*
* \return (float)
*   Spotlight attenuation coefficient.
*/
float ComputeSpotlightEffect(vec3 spotlightValues, float cosOfAngle)
{
	if (cosOfAngle > spotlightValues.x)
		return 1.0;
	else if (cosOfAngle < spotlightValues.y)
		return 0.0;
	
	return pow(
		(cosOfAngle - spotlightValues.y) / (spotlightValues.x - spotlightValues.y),
		spotlightValues.z); 
}

float ComputeFogCoeff(float nearDist, float farDist, float fragZ)
{
	// Negate fragZ because OpenGL camera faces -z axis
	return min(
		max(-fragZ - nearDist, 0.0) /* Minimum coeff is 0 */ / (farDist - nearDist),
		1.0); // Cap fog coeff at 1.0
}

vec4 ComputeFragColor(LightContext light, MaterialContext material, vec3 fragNormal, vec3 fragPos)
{
	vec4 fragColor;
	vec3 v3_light   = /* Vector from fragment to light */
		(light.type == constLightType_Dir) ?
		-(light.direction) :
		(light.position - fragPos);
	//vec3 v3_light   = light.position - fragPos;
	vec3 v3_uLight  = normalize(v3_light);
	vec3 v3_uNormal = normalize(fragNormal);
	
	/*--------------------------------------------------------------------------
	 * Compute distance attenuation
	 */
	float distAtt = 1.0;
	if (light.type != constLightType_Dir)
		distAtt = ComputeDistanceAtt(light.distAttCoeff, length(v3_light));
	
	/*--------------------------------------------------------------------------
	 * Ambient component
	 */
	fragColor = distAtt * (light.I_ambient * material.K_ambient);
	
	float dotProd_N_L = dot(v3_uNormal, v3_uLight);
	if (dotProd_N_L > 0.0)
	{
		/*----------------------------------------------------------------------
		 * Diffuse component
		 */
		vec4 diffuse = light.I_diffuse * material.K_diffuse * max(dotProd_N_L, 0.0);
		
		/*----------------------------------------------------------------------
		 * Specular component
		 */
		vec3  v3_uReflect = normalize((2.0 * dotProd_N_L) * v3_uNormal - v3_uLight); /* Reflected light vector */
		float dotProd_R_V = dot(v3_uReflect, normalize(-fragPos));
		float specCoeff   = pow(max(0.0, dotProd_R_V), material.shinePower);
		vec4  specular    = light.I_specular * vec4(material.K_specular.rgb, 1.0) * specCoeff;
		
		/*----------------------------------------------------------------------
		 * Compute spotlight effect
		 */
		float spotAtt= 1.0;
		if (light.type == constLightType_Spot)
		{
			float dotProd_lightDir_light = dot(-v3_uLight, normalize(light.direction));
			spotAtt = ComputeSpotlightEffect(light.spotAttCoeff, dotProd_lightDir_light);
		}
		
		/*----------------------------------------------------------------------
		 * Combine components
		 */
		fragColor += distAtt * spotAtt * (diffuse + specular);
	}
	
	return fragColor;
}
