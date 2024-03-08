uniform sampler2D 	source;
uniform vec2 offset;

void main()
{
	vec4 color = vec4(0.0);
    color += texture2D(source, gl_TexCoord[0].xy - 5.0 * offset) * 0.089215;
    color += texture2D(source, gl_TexCoord[0].xy - 4.0 * offset) * 0.090224;
	color += texture2D(source, gl_TexCoord[0].xy - 3.0 * offset) * 0.091017;
	color += texture2D(source, gl_TexCoord[0].xy - 2.0 * offset) * 0.091588;
	color += texture2D(source, gl_TexCoord[0].xy - offset) * 0.091932;
	color += texture2D(source, gl_TexCoord[0].xy) * 0.092047;
	color += texture2D(source, gl_TexCoord[0].xy + offset) * 0.091932;
    color += texture2D(source, gl_TexCoord[0].xy + 2.0 * offset) * 0.091588;
    color += texture2D(source, gl_TexCoord[0].xy + 3.0 * offset) * 0.091017;
    color += texture2D(source, gl_TexCoord[0].xy + 4.0 * offset) * 0.090224;
    color += texture2D(source, gl_TexCoord[0].xy + 5.0 * offset) * 0.089215;
	gl_FragColor = color;
}
