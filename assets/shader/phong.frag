#ifdef ANDROID
precision mediump float;
#endif

#ifdef WIN

#endif

uniform vec3 cameraPosition;

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

void main()
{
    float ambient = 0.5;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(-1.0, 1.0, -1.0));

    float diffuse = max(dot(norm, lightDir), 0.0);

    vec3 albedo = vec3(0.2, 0.2, 0.2);
    vec3 result = albedo * (ambient + diffuse);
    FragColor = vec4(result, 1.0);
}