#include <stdlib.h>
#include <stdio.h>

#include "x64_codegen.h"

#define X64_NUM_REGISTERS 7

//
// scratch register stuff
//

struct ScratchTable {
    const char* name[X64_NUM_REGISTERS];
    int in_use[X64_NUM_REGISTERS];
};

struct ScratchTable scratch_table = {
    { "%rbx", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15" },
    { 0, 0, 0, 0, 0, 0, 0 },
};

int scratch_alloc() {
    for (int r = 0; r < X64_NUM_REGISTERS; r++) {
        if (!scratch_table.in_use[r]) {
            return r;
        }
    }
    printf("Error: All registers are in use.\n");
    exit(1);
}

void scratch_free(int r) {
    if (r < 0 || r >= X64_NUM_REGISTERS) {
        printf("Error: Register value passed to scratch_free (%d) is not a valid register.\n", r);
        exit(1);
    }
    scratch_table.in_use[r] = 0;
}

const char* scratch_name(int r) {
    if (r < 0 || r >= X64_NUM_REGISTERS) {
        printf("Error: Register value passed to scratch_name (%d) is not a valid register.\n", r);
        exit(1);
    }
    return scratch_table.name[r];
}

//
// label stuff
//

int current_label_num = 0;

int label_create() {
    return current_label_num++;
}

const char* label_name(int label) {
    char* temp = 0;
    sprintf(temp, ".L%d", label);
    return temp;
}
