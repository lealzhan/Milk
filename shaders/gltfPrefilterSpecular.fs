#version 400 compatibility

#define UX3D_MATH_PI 3.1415926535897932384626433832795
#define UX3D_MATH_INV_PI (1.0 / 3.1415926535897932384626433832795)

const int const_smaples = 4096;

// layout(push_constant, std140) uniform _u_parameter {
//     layout(offset = 64) uint samples;
//     float roughness;
// } u_parameter;

// layout (binding = 0) uniform samplerCube u_cubeMap;

// layout (location = 0) in vec3 v_normal;

// layout (location = 0) out vec4 ob_fragColor;
uniform float u_roughness;
uniform samplerCube environmentMap;
in vec3 WorldPos;
out vec4 ob_fragColor;

// vec2 randomHammersley(uint i, uint n)
// {
//     return vec2(float(i) / float(n), bitfieldReverse(i) * 2.3283064365386963e-10);
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


vec3 renderGetGGXWeightedVector(vec2 e, vec3 normal, float roughness)
{
    float alpha = roughness * roughness;

    float phi = 2.0 * UX3D_MATH_PI * e.y;
    float cosTheta = sqrt((1.0 - e.x) / (1.0 + (alpha*alpha - 1.0) * e.x));
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

float ndfTrowbridgeReitzGGX(float NdotH, float roughness)
{
    float alpha = roughness * roughness;
    
    float alpha2 = alpha * alpha;
    
    float divisor = NdotH * NdotH * (alpha2 - 1.0) + 1.0;
        
    return alpha2 / (UX3D_MATH_PI * divisor * divisor); 
}

vec4 renderCookTorrance(vec2 randomPoint, vec3 N, float roughness)
{
    vec3 H = renderGetGGXWeightedVector(randomPoint, N, roughness);

    // Note: reflect takes incident vector.
    // Note: N = V
    vec3 V = N;
    
    vec3 L = normalize(reflect(-V, H));
    
    float NdotL = dot(N, L);

    if (NdotL > 0.0)
    {
        float lod = 0.0;

        if (roughness > 0.0)
        {    
            // Mipmap Filtered Samples
            // see https://github.com/derkreature/IBLBaker
            // see https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch20.html
            
            float cubeWidth = float(textureSize(environmentMap, 0).x);
        
            float VdotH = dot(V, H);
            float NdotH = dot(N, H);
        
            float D = ndfTrowbridgeReitzGGX(NdotH, roughness);
            float pdf = max(D * NdotH / (4.0 * VdotH), 0.0);
            
            float solidAngleTexel = 4.0 * UX3D_MATH_PI / (6.0 * cubeWidth * cubeWidth);
            float solidAngleSample = 1.0 / (const_smaples * pdf);
            
            lod = 0.5 * log2(solidAngleSample / solidAngleTexel);
        }
	    
        return vec4(textureLod(environmentMap, L, lod).rgb * NdotL, NdotL);
    }
	
    return vec4(0.0, 0.0, 0.0, 0.0);
}

void main(void)
{
    vec3 N = normalize(WorldPos);

    vec4 colorCookTorrance = vec4(0.0, 0.0, 0.0, 0.0);

    for (uint sampleIndex = 0; sampleIndex < const_smaples; sampleIndex++)
    {
        vec2 randomPoint = randomHammersley(sampleIndex, const_smaples);

        // N = V
        colorCookTorrance += renderCookTorrance(randomPoint, N, u_roughness);
    }
    
    ob_fragColor = vec4(colorCookTorrance.rgb / colorCookTorrance.w, 1.0);
}