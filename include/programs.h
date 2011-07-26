char divide_op[] = "!!ARBfp1.0\n"
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

char copy_op[] = "!!ARBfp1.0\n"
"OPTION NV_fragment_program2;\n"
"PARAM c[1] = { program.local[0] };\n"
"TEMP RC;\n"
"TEMP HC;\n"
"OUTPUT oCol = result.color;\n"
"TEX   oCol.x, fragment.position, texture[0], RECT;\n"
"END\n";


char swaprow_op[] = "!!ARBfp1.0\n"
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


char swapcol_op[]="!!ARBfp1.0\n"
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

char quadtree_op[]="!!ARBfp1.0\n"
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


char row_op[] = "!!ARBfp1.0\n"
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


char max_op[] = "!!ARBfp1.0\n"
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
