char divide_op_old[] = "!!ARBfp1.0\n"
"OPTION NV_fragment_program2;\n"
"PARAM c[1] = { program.local[0] };\n"
"TEMP R0;\n"
"TEMP RC;\n"
"TEMP HC;\n"
"OUTPUT oCol = result.color;\n"
"TEX   R0.x, fragment.texcoord[0], texture[0], RECT;\n"
"RCPR  R0.y, R0.x;\n"
"TEX   R0.x, fragment.position, texture[0], RECT;\n"
"MULR  oCol.x, R0, R0.y;\n"
"END\n"
"# 4 instructions, 1 R-regs, 0 H-regs\n";

char divide_op[] = "

varying vec2 texcoord0;
uniform sampler2D texture0;

void main(void) {
  
  vec4 t1 = texture2D(texture0, texcoord0);
  vec4 t2 = texture2D(texture0, gl_FragCoord);

  gl_FragColor = vec4(t2.x / t1.x, 0, 0, 0);
}

";

char copy_op_old[] = "!!ARBfp1.0\n"
"OPTION NV_fragment_program2;\n"
"PARAM c[1] = { program.local[0] };\n"
"TEMP RC;\n"
"TEMP HC;\n"
"OUTPUT oCol = result.color;\n"
"TEX   oCol.x, fragment.position, texture[0], RECT;\n"
"END\n";

char copy_op[] = "

varying vec2 texcoord0;
uniform sampler2D texture0;

void main(void) {
  gl_FragColor = texture2D(texture0, texcoord0);
}

";


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

char swaprow_op[] = "

varying vec2 texcoord0;
uniform sampler2D texture0;

uniform float y;

void main(void) {
  vec2 coord = texcoord0;
  coord.y = y;
  gl_FragColor = texture2D(texture0, texcoord0);
}

";


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

char swapcol_op[] = "

varying vec2 texcoord0;
uniform sampler2D texture0;

uniform float y;

void main(void) {
  vec2 coord = texcoord0;
  coord.y = y;
  gl_FragColor = texture2D(texture0, texcoord0);
}

";

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

char quadtree_op[] = "

varying vec2 texcord0;
uniform sampler2D texture0;
uniform sampler2D texture1;

uniform vec4 c;

void main(void) {
  vec4 r0 = texture2D(texture1, gl_FragCoord);
  vec4 r1 = texture2D(texture0, c);

  
}

";


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

char row_op[] = "

varying vec2 texcoord0;
varying vec2 texcoord1;
uniform sampler2D texture0;
uniform sampler2D texture1;

void main(void) {
  vec4 r0 = texture2D(texture0, texcoord0);
  vec4 r2 = texture2D(texture0, gl_FragCoord);
  vec4 r1 = texture2D(texture0, texcoord1);
  gl_FragColor = r1.x + r2.x - r0.x;
}

";


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
//FIXME: are those abs's?
char max_op[] = "

varying vec2 texcoord0;
uniform sampler2D texture0;

void main(void) {
  vec4 r1 = texture2D(texcoord0, texture0);
  vec4 r0 = texture2D(gl_FragCoord, texture0);
  gl_FragColor = max(r0, r1);
}

";


