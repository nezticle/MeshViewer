VARYING vec2 texCoord;

void MAIN()
{
    texCoord = UV0;
    POSITION = MODELVIEWPROJECTION_MATRIX * vec4(VERTEX, 1.0);
}
