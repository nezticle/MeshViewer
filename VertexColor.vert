VARYING vec4 vertexColor;

void MAIN()
{
    vertexColor = COLOR;
    POSITION = MODELVIEWPROJECTION_MATRIX * vec4(VERTEX, 1.0);
}
