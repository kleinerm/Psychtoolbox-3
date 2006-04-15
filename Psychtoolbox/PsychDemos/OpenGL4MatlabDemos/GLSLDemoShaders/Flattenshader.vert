varying vec4 diffuse,ambientGlobal,ambient;
varying vec3 normal,lightDir,halfVector;
varying float dist;

void main()
{	
    vec4 ecPos;
    vec3 aux;

    normal = normalize(gl_NormalMatrix * gl_Normal);

    vec4 va = ftransform();
	vec4 v = gl_Vertex;
    
	v.y = v.y * (1.0 + sin(va.x * 6.0)*0.3);	
	gl_Position = gl_ModelViewProjectionMatrix * v;
    ecPos = gl_ModelViewMatrix * v;

    /* these are the new lines of code to compute the light's direction */
    aux = vec3(gl_LightSource[0].position-ecPos);
    lightDir = normalize(aux);
    dist = length(aux);

    halfVector = normalize(gl_LightSource[0].halfVector.xyz);

    /* Compute the diffuse, ambient and globalAmbient terms */
    diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;

    /* The ambient terms have been separated since one of them */
    /* suffers attenuation */
    ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
    ambientGlobal = gl_LightModel.ambient * gl_FrontMaterial.ambient;
}