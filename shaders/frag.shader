#version 330 core


out vec4 FragColor;
uniform bool  wireframe;


void main()
{
    if (wireframe) {
        FragColor = vec4(0.10f, 0.85f, 0.60f, 1.0);
        return;
    }

    FragColor = vec4(0.20f, 0.35f, 0.55f, 1.0);
}
