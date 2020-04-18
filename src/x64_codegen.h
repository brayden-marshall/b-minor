#ifndef X64_CODEGEN_H
#define X64_CODEGEN_H

int scratch_alloc();

void scratch_free(int r);

const char* scratch_name(int r);

int label_create();

const char* label_name(int label);

#endif
