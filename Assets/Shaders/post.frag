#version 450

layout(set = 0, binding = 0)
uniform sampler2D sceneTexture;

layout(set = 0, binding = 1)
uniform sampler2D depthTexture;

layout(location = 0) in vec2 inUV;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants {
    int hdrEnable;
    float exposure;
    int dithering;
} pc;

// Dithering - Interleaved Gradient Noise
float IGN(vec2 pixel) {
    return fract(52.9829189 * fract(dot(pixel, vec2(0.06711056, 0.00583715))));
}

void main() {

    vec3 color = texture(sceneTexture, inUV).rgb;

    // HDR
    if (pc.hdrEnable != 0) {

        // EXPOSURE
        color *= pc.exposure;

        // TONEMAPPING HDR --> SDR
        color = color / (1.0 + color);

    } else {

        // HDR VALUES NORMALIZATION
        float maxValue = max(color.r, max(color.g, color.b));
        if (maxValue > 1.0) color /= maxValue;

    }

    // DITHERING
    if (pc.dithering != 0) {
        float noise = IGN(gl_FragCoord.xy);
        color += (noise - 0.5) / 512.0;
    }

    outColor = vec4(color, 1.0);

}
