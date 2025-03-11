//
// Created by prg6 on 11.03.2025.
//

#ifndef TESTANDROID_SHADER_BUILDER_H
#define TESTANDROID_SHADER_BUILDER_H


#include <stdint.h>

#define EXTEND_IMPORT_POINT_INDEX 0x1

#define SHADER_MAX_LENGTH 2048
#define SHADER_MAX_IODATA 64
#define SHADER_MAX_DEBUG_NAMES 64
#define SHADER_MAX_FUNCTIONS 64
#define SHADER_MAX_DECORATIONS 64

typedef enum{
    SHADER_TYPE_FRAGMENT,
    SHADER_TYPE_VERTEX,
    SHADER_TYPE_COMPUTED,
    SHADER_TYPE_GEOMETRY,
    SHADER_TYPE_TESELLATION_CONTROL,
    SHADER_TYPE_TESELLATION_EVALUATION,
} ShaderType;

typedef enum{
    SHADER_CONDITIONAL_TYPE_FLESS_THAN,
    SHADER_CONDITIONAL_TYPE_SLESS_THAN,
    SHADER_CONDITIONAL_TYPE_FGREAT_THAN,
    SHADER_CONDITIONAL_TYPE_SGREAT_THAN,
    SHADER_CONDITIONAL_TYPE_EQUAL,
    SHADER_CONDITIONAL_TYPE_NOT_EQUAL,
    SHADER_CONDITIONAL_TYPE_LOGICAL_NOT,
} ConditionalType;

typedef enum{
    SHADER_VARIABLE_TYPE_VOID,
    SHADER_VARIABLE_TYPE_BOOL,
    SHADER_VARIABLE_TYPE_INT,
    SHADER_VARIABLE_TYPE_FLOAT,
    SHADER_VARIABLE_TYPE_FLOAT_POINTER,
    SHADER_VARIABLE_TYPE_VECTOR,
    SHADER_VARIABLE_TYPE_MATRIX,
    SHADER_VARIABLE_TYPE_ARRAY,
    SHADER_VARIABLE_TYPE_IMAGE,
    SHADER_VARIABLE_TYPE_SAMPLED_IMAGE,
    SHADER_VARIABLE_TYPE_STRUCT,
    SHADER_VARIABLE_TYPE_VARIABLE,
    SHADER_VARIABLE_TYPE_CONSTANT,
    SHADER_VARIABLE_TYPE_CONSTANT_FALSE,
    SHADER_VARIABLE_TYPE_CONSTANT_TRUE,
    SHADER_VARIABLE_TYPE_CONSTANT_COMPOSITE,
    SHADER_VARIABLE_TYPE_POINTER,
    SHADER_VARIABLE_TYPE_FUNCTION,
    SHADER_VARIABLE_TYPE_UNIFORM,
    SHADER_VARIABLE_TYPE_UNIFORM_CONSTANT,
    SHADER_VARIABLE_TYPE_EXTENDED_IMPORT,
} ShaderVariableType;

typedef struct ShaderStructConstr{
    ShaderVariableType var_type;
    uint32_t size;
    uint32_t built_in;
    char name[32];
    struct ShaderStructConstr *str;
    uint32_t count;
    char name_str[32];
} ShaderStructConstr;

typedef enum{
    SHADER_OPERAND_TYPE_ACCESS,
    SHADER_OPERAND_TYPE_EXT_INST,
    SHADER_OPERAND_TYPE_FUNC_CALL,
    SHADER_OPERAND_TYPE_FUNCTION_PARAM,
    SHADER_OPERAND_TYPE_LOAD,
    SHADER_OPERAND_TYPE_STORE,
    SHADER_OPERAND_TYPE_BRANCH,
    SHADER_OPERAND_TYPE_BRANCH_CONDITIONAL,
    SHADER_OPERAND_TYPE_SELECTION_MERGE,
    SHADER_OPERAND_TYPE_LOOP,
    SHADER_OPERAND_TYPE_COMPOSITE_EXTRACT,
    SHADER_OPERAND_TYPE_COMPOSITE_CONSTRUCT,
    SHADER_OPERAND_TYPE_IMAGE_SAMLE_IMPLICIT_LOD,
    SHADER_OPERAND_TYPE_IMAGE_SAMLE_EXPLICIT_LOD,
    SHADER_OPERAND_TYPE_NEGATIVE,
    SHADER_OPERAND_TYPE_DOT,
    SHADER_OPERAND_TYPE_ADD,
    SHADER_OPERAND_TYPE_IADD,
    SHADER_OPERAND_TYPE_SUB,
    SHADER_OPERAND_TYPE_ISUB,
    SHADER_OPERAND_TYPE_MUL,
    SHADER_OPERAND_TYPE_DIV,
    SHADER_OPERAND_TYPE_MATTIMEMAT,
    SHADER_OPERAND_TYPE_MATTIMEVEC,
    SHADER_OPERAND_TYPE_MATTIMESCAL,
    SHADER_OPERAND_TYPE_VECTIMES,
    SHADER_OPERAND_TYPE_VECSHUFFLE,
    SHADER_OPERAND_TYPE_TRANSPOSE,
    SHADER_OPERAND_TYPE_VARIABLE,
    SHADER_OPERAND_TYPE_SLESSTHAN,
    SHADER_OPERAND_TYPE_FLESSTHAN,
    SHADER_OPERAND_TYPE_SGREATTHAN,
    SHADER_OPERAND_TYPE_FGREATTHAN,
    SHADER_OPERAND_TYPE_IEQUAL,
    SHADER_OPERAND_TYPE_INOTEQUAL,
    SHADER_OPERAND_TYPE_LOGICAL_NOT,
    SHADER_OPERAND_TYPE_CONVERT_FTOS,
    SHADER_OPERAND_TYPE_DPDX,
    SHADER_OPERAND_TYPE_DPDY,
    SHADER_OPERAND_TYPE_KILL,
    SHADER_OPERAND_TYPE_RETURN_VALUE
} ShaderOperandType;

typedef enum{
    SHADER_DATA_FLAG_UNSIGNED = 0x1,
    SHADER_DATA_FLAG_OUTPUT = 0x2,
    SHADER_DATA_FLAG_UNIFORM= 0x4,
    SHADER_DATA_FLAG_UNIFORM_CONSTANT = 0x8,
    SHADER_DATA_FLAG_SYSTEM = 0x10,
    SHADER_DATA_FLAG_FUNCTION = 0x20,
    SHADER_DATA_FLAG_LOAD = 0x40,
} ShaderDataFlags;

typedef enum{
    SHADER_DECOR_TYPE_BUILTIN = 0x1,
} ShaderDecorType;

typedef struct{
    ShaderVariableType type;
    uint32_t indx;
    uint32_t orig_indx;
    uint32_t binding;
    uint32_t descr_set;
    uint32_t location;
    ShaderDataFlags flags;
} InputOutputData;

typedef struct{
    uint32_t indx;
    uint32_t res_store_indx;
    ShaderOperandType op_type;
    uint32_t var_indx[16];
    uint32_t num_vars;
} ShaderOperand;

typedef struct ShaderVariable{
    ShaderVariableType type;
    uint32_t indx;
    uint32_t args[32];
    uint32_t num_args;
    uint32_t values[32];
    uint32_t num_values;
    ShaderDataFlags flags;
    uint32_t result_type_indx;
} ShaderVariable;

typedef struct VectorExtract{
    uint32_t elems[8];
    uint32_t size;
} VectorExtract;

typedef struct{
    ShaderVariableType type;
    uint32_t index;
} ShaderFuncParam;

typedef struct{
    uint32_t index;
    uint32_t num_label;
    ShaderOperand operands[256];
    uint32_t num_operands;
    int will_return;
} ShaderLabel;

typedef struct{
    uint32_t result_type_indx;
    uint32_t func_type_indx;
    uint32_t indx;
    uint32_t function_control;
    ShaderFuncParam params[16];
    uint32_t num_params;
    ShaderLabel labels[32];
    uint32_t num_labels;
    uint32_t args[10];
    uint32_t arg_indxs[10];
    uint32_t num_args;
} ShaderFunc;

typedef struct{
    uint32_t indx;
    char name[64];
    char child_name[16][64];
    uint32_t num_childs;
} ShaderDebugInfo;

typedef struct{
    uint32_t indx;
    uint32_t type;
    uint32_t val;
    void *str_point;
    uint32_t str_size;
    uint32_t args[10];
    uint32_t num_args;
} ShaderDecoration;

typedef struct{
    uint32_t code[SHADER_MAX_LENGTH];
    uint32_t size;
    InputOutputData ioData[SHADER_MAX_IODATA];
    uint32_t num_io_data;
    ShaderFunc functions[SHADER_MAX_FUNCTIONS];
    uint32_t num_functions;
    ShaderDebugInfo infos[SHADER_MAX_DEBUG_NAMES];
    uint32_t num_debug_infos;
    ShaderDecoration decors[SHADER_MAX_DECORATIONS];
    uint32_t num_decorations;
    ShaderType type;
    uint32_t current_index;
    uint32_t curr_descr_set;
    ShaderFunc *main_point_index;
    uint32_t gl_struct_indx;
    struct ChildStack *alloc_head;
} ShaderBuilder;

#endif //TESTANDROID_SHADER_BUILDER_H
