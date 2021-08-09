#version 400 compatibility

//#define UX3D_MATH_PI 3.1415926535897932384626433832795
#define UX3D_MATH_PI 3.141592653
#define UX3D_MATH_INV_PI (1.0 / 3.1415926535897932384626433832795)
//#define UX3D_MATH_INV_PI 0.31830988618

#define UX3D_MATH_TWO_PI (2.0 * UX3D_MATH_PI)
#define UX3D_MATH_HALF_PI (0.5 * UX3D_MATH_PI)

const int const_smaples = 4096;

// layout(push_constant, std140) uniform _u_parameter {
//     layout(offset = 64) uint samples;
//     uint padding;
// } u_parameter;

// layout (binding = 0) uniform samplerCube u_cubeMap;

// layout (location = 0) in vec3 v_normal;

// layout (location = 0) out vec4 ob_fragColor;

uniform samplerCube environmentMap;
uniform int hdr;
uniform int p20;

in vec3 WorldPos;
out vec4 ob_fragColor;

// Hejl Richard tone map
// see: http://filmicworlds.com/blog/filmic-tonemapping-operators/
vec3 toneMapHejlRichard(vec3 color)
{
	return (color*(6.2*color + .5)) / (color*(6.2*color + 1.7) + 0.06);
}

// vec2 randomHammersley(uint i, uint n)
// {
//     double temp = 2.3283064365386963e-10;
//     return vec2(float(i) / float(n), float( double(bitfieldReverse(i) * temp)) ) ;
// }

// ----------------------------------------------------------------------------
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// efficient VanDerCorpus calculation.
float RadicalInverse_VdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
vec2 randomHammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

vec3 renderGetLambertWeightedVector(vec2 e, vec3 normal)
{
    float phi = 2.0f * UX3D_MATH_PI * e.y;
    float cosTheta = 1.0f - e.x;
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    // Note: Polar Coordinates
    // x = sin(theta)*cos(phi)
    // y = sin(theta)*sin(phi)
    // z = cos(theta)

    vec3 H = normalize(vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta));
    
    //
    
    vec3 bitangent = vec3(0.0, 1.0, 0.0);

    float NdotB = dot(normal, bitangent);

    if (NdotB == 1.0)
    {
        bitangent = vec3(0.0, 0.0, -1.0);
    }
    else if (NdotB == -1.0)
    {
        bitangent = vec3(0.0, 0.0, 1.0);
    }

    vec3 tangent = cross(bitangent, normal);
    bitangent = cross(normal, tangent);
    
    //
    
    return normalize(tangent * H.x + bitangent * H.y + normal * H.z);
}

vec4 renderLambert(vec2 randomPoint, vec3 N)
{
    vec3 H = renderGetLambertWeightedVector(randomPoint, N);
    
    // Note: reflect takes incident vector.
    // Note: N = V
    vec3 V = N;

    vec3 L = normalize(reflect(-V, H));
        
    float NdotL = dot(N, L);
        
    if (NdotL > 0.0)
    {   
        // Mipmap Filtered Samples 
        // see https://github.com/derkreature/IBLBaker
        // see https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch20.html
        
        float cubeWidth = float(textureSize(environmentMap, 0).x);

        float pdf = max(NdotL * UX3D_MATH_INV_PI, 0.0);
            
        float solidAngleTexel = 4.0f * UX3D_MATH_PI / (6.0f * cubeWidth * cubeWidth);
        float solidAngleSample = 1.0f / (float(const_smaples) * pdf);
            
        float lod = 0.5f * log2(solidAngleSample / solidAngleTexel);
        
        return vec4(texture(environmentMap, H, lod).rgb, 1.0);
    }
    
    return vec4(0.0, 0.0, 0.0, 0.0);
}

void main(void)
{
    vec3 N = normalize(WorldPos);//v_normal);
	if(p20!=0)
		N = mat3(1,0,0, 0,0,1, 0,1,0) * N; //gaode coordinate transform
		
    //ob_fragColor = texture(environmentMap, N, 0); return;//test
    vec4 colorLambert = vec4(0.0, 0.0, 0.0, 0.0);

    for (uint sampleIndex = 0; sampleIndex < const_smaples; sampleIndex++)
    {
        vec2 randomPoint = randomHammersley(sampleIndex, const_smaples);
        if(randomPoint.x>1 || randomPoint.x<0)
        {
            ob_fragColor = vec4(1,0,0,1); return;
        }
        if(randomPoint.y>1 || randomPoint.y<0)
        {
            ob_fragColor = vec4(0,0,1,1); return;
        }

        // N = V
        colorLambert += renderLambert(randomPoint, N);
    }
    
    ob_fragColor = vec4(colorLambert.rgb / colorLambert.w, 1.0);
	if (hdr==0) { ob_fragColor.rgb = toneMapHejlRichard(ob_fragColor.rgb); }
}