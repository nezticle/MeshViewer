VARYING vec2 texCoord;

void MAIN()
{
    FRAGCOLOR = vec4(texture(uvTexture, texCoord).rgb, 1.0);
}
