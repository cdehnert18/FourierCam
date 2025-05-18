#version 320 es
precision mediump float;

in vec2 v_texcoord;
out vec4 outColor;

uniform sampler2D tex;

void main() {
    outColor = texture(tex, v_texcoord);
}