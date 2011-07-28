char vertex_shader[] =
"attribute vec2 position;\n"
"attribute vec2 v_texcoord0;\n"
"attribute vec2 v_texcoord1;\n"

"uniform sampler2D texture0;\n"
"uniform sampler2D texture1;\n"

"varying vec2 texcoord0;\n"
"varying vec2 texcoord1;\n"

"void main(void) {\n"
"  texcoord0 = v_texcoord0;\n"
"  texcoord1 = v_texcoord1;\n"
"  gl_Position = vec4(position.x, position.y, 0, 0);\n"
"}\n";

char divide_op_old[] = "!!ARBfp1.0\n"
"OPTION NV_fragment_program2;\n"
"PARAM c[1] = { program.local[0] };\n"
"TEMP RC;\n"
"TEMP HC;\n"
"OUTPUT oCol = result.color;\n"
"TEX   R0.x, fragment.texcoord[0], texture[0], RECT;\n"
"RCPR  R0.y, R0.x;\n"
"TEX   R0.x, fragment.position, texture[0], RECT;\n"
"MULR  oCol.x, R0, R0.y;\n"
"END\n"
"# 4 instructions, 1 R-regs, 0 H-regs\n";

char divide_op[] = 

"varying vec2 texcoord0;\n"
"uniform sampler2D texture0;\n"

"void main(void) {\n"
"  \n"
"  vec4 t1 = texture2D(texture0, texcoord0);\n"
"  vec4 t2 = texture2D(texture0, vec2(gl_FragCoord.x, gl_FragCoord.y));\n"

"  gl_FragColor = vec4(t2.x / t1.x, 0, 0, 0);\n"
"}\n";


char copy_op_old[] = "!!ARBfp1.0\n"
"OPTION NV_fragment_program2;\n"
"PARAM c[1] = { program.local[0] };\n"
"TEMP RC;\n"
"TEMP HC;\n"
"OUTPUT oCol = result.color;\n"
"TEX   oCol.x, fragment.position, texture[0], RECT;\n"
"END\n";

char copy_op[] = 

"varying vec2 texcoord0;\n"
"uniform sampler2D texture0;\n"

"void main(void) {\n"
"  gl_FragColor = texture2D(texture0, texcoord0);\n"
"}\n";


char swaprow_op_old[] = "!!ARBfp1.0\n"
"OPTION NV_fragment_program2;\n"
"PARAM c[1] = { program.local[0] };\n"
"TEMP R0;\n"
"TEMP RC;\n"
"TEMP HC;\n"
"OUTPUT oCol = result.color;\n"
"MOVR  R0.y, c[0].x;\n"
"MOVR  R0.x, fragment.position;\n"
"TEX   oCol.x, R0, texture[0], RECT;\n"
"END\n";

char swaprow_op[] = 

"varying vec2 texcoord0;\n"
"uniform sampler2D texture0;\n"

"uniform float y;\n"

"void main(void) {\n"
"  vec2 coord = texcoord0;\n"
"  coord.y = y;\n"
"  gl_FragColor = texture2D(texture0, texcoord0);\n"
"}\n";


char swapcol_op_old[]="!!ARBfp1.0\n"
"OPTION NV_fragment_program2;\n"
"PARAM c[1] = { program.local[0] };\n"
"TEMP R0;\n"
"TEMP RC;\n"
"TEMP HC;\n"
"OUTPUT oCol = result.color;\n"
"MOVR  R0.y, fragment.position;\n"
"MOVR  R0.x, c[0];\n"
"TEX   oCol.x, R0, texture[0], RECT;\n"
"END\n";

char swapcol_op[] = 

"varying vec2 texcoord0;\n"
"uniform sampler2D texture0;\n"

"uniform float y;\n"

"void main(void) {\n"
"  vec2 coord = texcoord0;\n"
"  coord.y = y;\n"
"  gl_FragColor = texture2D(texture0, texcoord0);\n"
"}\n";

char quadtree_op_old[]="!!ARBfp1.0\n"
"OPTION NV_fragment_program2;\n"
"PARAM c[1] = { program.local[0] };\n"
"TEMP R0;\n"
"TEMP R1;\n"
"TEMP RC;\n"
"TEMP HC;\n"
"OUTPUT oCol = result.color;\n"
"TEX   R0.x, fragment.position, texture[1], RECT;\n"
"TEX   R1.x, c[0].x, texture[0], RECT;\n"
"SLTRC HC.x, |R0|, R1;\n"
"KIL   GT.x;\n"
"MOVR  oCol.x, |R0|;\n"
"END\n";

char quadtree_op[] = 

"varying vec2 texcord0;\n"
"uniform sampler2D texture0;\n"
"uniform sampler2D texture1;\n"

"uniform vec4 c;\n"

"void main(void) {\n"
"  vec4 r0 = texture2D(texture1, vec2(gl_FragCoord.x, gl_FragCoord.y));\n"
"  vec4 r1 = texture2D(texture0, vec2(c.x, c.y));\n"
"  gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
"  // FIXME \n"
"}\n";


char row_op_old[] = "!!ARBfp1.0\n"
"OPTION NV_fragment_program2;\n"
"PARAM c[1] = { program.local[0] };\n"
"TEMP R0;\n"
"TEMP R1;\n"
"TEMP R2;\n"
"TEMP RC;\n"
"TEMP HC;\n"
"OUTPUT oCol = result.color;\n"
"TEX   R0.x, fragment.texcoord[0], texture[0], RECT;\n"
"TEX   R2.x, fragment.position, texture[0], RECT;\n"
"TEX   R1.x, fragment.texcoord[1], texture[0], RECT;\n"
"MADR  oCol, -R0.x, R1.x, R2.x;\n"
"END\n";

char row_op[] = 

"varying vec2 texcoord0;\n"
"varying vec2 texcoord1;\n"
"uniform sampler2D texture0;\n"
"uniform sampler2D texture1;\n"

"void main(void) {\n"
"  vec4 r0 = texture2D(texture0, texcoord0);\n"
"  vec4 r2 = texture2D(texture0, vec2(gl_FragCoord.x, gl_FragCoord.y));\n"
"  vec4 r1 = texture2D(texture0, texcoord1);\n"
"  gl_FragColor = vec4(r1.x + r2.x - r0.x, 0.0, 0.0, 0.0);\n"
"}\n";


char max_op_old[] = "!!ARBfp1.0\n"
"OPTION NV_fragment_program2;\n"
"PARAM c[1] = { program.local[0] };\n"
"TEMP R0;\n"
"TEMP R1;\n"
"TEMP RC;\n"
"TEMP HC;\n"
"OUTPUT oCol = result.color;\n"
"TEX   R1.x, fragment.texcoord[0], texture[0], RECT;\n"
"TEX   R0.x, fragment.position, texture[0], RECT;\n"
"MAXR  oCol.x, |R0|, |R1|;\n"
"END\n";

//FIXME: are those abs's?\n"

char max_op[] = 

"varying vec2 texcoord0;\n"
"uniform sampler2D texture0;\n"

"void main(void) {\n"
"  vec4 r1 = texture2D(texture0, texcoord0);\n"
"  vec4 r0 = texture2D(texture0, vec2(gl_FragCoord.x, gl_FragCoord.y));\n"
"  gl_FragColor = max(r0, r1);\n"
"}\n";

