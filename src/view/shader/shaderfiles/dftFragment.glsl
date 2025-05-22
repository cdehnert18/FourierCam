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
    int u = int(v_texcoord.x * float(width));
    int v = int(v_texcoord.y * float(height));

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

    // Compute magnitude
    float magnitudeR = length(sumR);
    float magnitudeG = length(sumG);
    float magnitudeB = length(sumB);


    // neu
    // magnitudeR /= float(width * height);
    // magnitudeG /= float(width * height);
    // magnitudeB /= float(width * height);


    float log_magR = log(1.0 + magnitudeR);
    float log_magG = log(1.0 + magnitudeG);
    float log_magB = log(1.0 + magnitudeB);

    // neu
    int valueR = int((magnitudeR / log(256.0)) * 255.0);
    int valueG = int((magnitudeG / log(256.0)) * 255.0);
    int valueB = int((magnitudeB / log(256.0)) * 255.0);
    if (valueR > 255) valueR = 255;
    if (valueR < 0) valueR = 0;
    if (valueG > 255) valueG = 255;
    if (valueG < 0) valueG = 0;
    if (valueB > 255) valueB = 255;
    if (valueB < 0) valueB = 0;

    // Optional DC shift: multiply by (-1)^(u+v) to center the zero freq
    float shift = mod(float(u + v), 2.0) == 0.0 ? 1.0 : -1.0;
    log_magR *= shift;
    log_magG *= shift;
    log_magB *= shift;

    //FragColor = vec4(log_magR, log_magG, log_magB, 1.0);
    FragColor = vec4(valueR, valueG, valueB, 1.0);
}
