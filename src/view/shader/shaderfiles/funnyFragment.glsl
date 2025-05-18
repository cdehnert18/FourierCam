#version 320 es
precision mediump float;

in vec2 v_texcoord;
out vec4 outColor;

uniform sampler2D tex;
uniform float time; // Pass the current time in seconds

void main() {
    // Apply a sine-based wave distortion
    float wave = sin(v_texcoord.y * 40.0 + time * 5.0) * 0.02;
    vec2 distorted_texcoord = vec2(v_texcoord.x + wave, v_texcoord.y);

    // Fetch the color from the distorted coordinates
    vec4 color = texture(tex, distorted_texcoord);

    // Optional: Add a slight color tint for fun
    color.rgb *= vec3(0.90 + 0.1 * sin(time * 2.0), 0.90 + 0.1 * sin(time * 2.0), 0.9 + 0.1 * cos(time * 2.0));

    outColor = color;
}
