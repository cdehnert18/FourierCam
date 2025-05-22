#version 320 es
precision highp float;
precision highp int;

out vec4 FragColor;

in vec2 v_texcoord; // normalized coords (0..1)
uniform sampler2D inputTexture;
uniform int width;
uniform int height;

const float PI = 3.14159265358979323846;

void main() {
    // int u = int(v_texcoord.x * float(width));
    // int v = int(v_texcoord.y * float(height));

    int u = int(v_texcoord.x * float(width)) - (width / 2);
    int v = int(v_texcoord.y * float(height)) - (height / 2);


    vec2 sumR = vec2(0.0); // Real + Imag parts
    vec2 sumG = vec2(0.0);
    vec2 sumB = vec2(0.0);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            vec2 coord = vec2(float(x) / float(width), float(y) / float(height));
            vec3 color = texture(inputTexture, coord).rgb;
            // float intensity = dot(color, vec3(0.299, 0.587, 0.114)); // Grayscale

            float angle = 2.0 * PI * ((float(u) * float(x) / float(width)) + (float(v) * float(y) / float(height)));
            sumR += color.r * vec2(cos(-angle), sin(-angle)); // e^(-i*angle)
            sumG += color.g * vec2(cos(-angle), sin(-angle));
            sumB += color.b * vec2(cos(-angle), sin(-angle));
        }
    }

    // Normalize
    float factor = float(width * height);
    sumR /= factor;
    sumG /= factor;
    sumB /= factor;

    // Shift
    float shift = mod(float(u + v), 2.0) == 0.0 ? 1.0 : -1.0;
    sumR *= shift;
    sumG *= shift;
    sumB *= shift;

    // Compute magnitude
    float magnitudeR = log(1.0 + length(sumR));
    float magnitudeG = log(1.0 + length(sumG));
    float magnitudeB = log(1.0 + length(sumB));

    float valueR = (magnitudeR / log(256.0)) * 255.0;
    float valueG = (magnitudeG / log(256.0)) * 255.0;
    float valueB = (magnitudeB / log(256.0)) * 255.0;
    
    valueR = clamp(valueR, 0.0, 255.0);
    valueG = clamp(valueG, 0.0, 255.0);
    valueB = clamp(valueB, 0.0, 255.0);

    FragColor = vec4(valueR, valueG, valueB, 1.0);
}
