#version 450

layout(set = 0, binding = 0) uniform sampler2D screenTex;

layout(location = 0) out vec4 outColor;

void main() {
    vec2 uv = gl_FragCoord.xy / vec2(1920.0, 1080.0); // na start OK
    outColor = texture(screenTex, uv);
}