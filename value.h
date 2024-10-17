//> Chunks of Bytecode value-h
#ifndef clox_value_h
#define clox_value_h
//> Optimization include-string

#include <string.h>
//< Optimization include-string

#include "common.h"

//> Strings forward-declare-obj
typedef struct Obj Obj;
//> forward-declare-obj-string
typedef struct ObjString ObjString;
//< forward-declare-obj-string

//< Optimization nan-boxing
//> Types of Values value-type
typedef enum
{
  VAL_BOOL,
  VAL_NIL, // [user-types]
  VAL_NUMBER,

  VAL_OBJ
} ValueType;

//< Types of Values value-type
/* Chunks of Bytecode value-h < Types of Values value
typedef double Value;
*/
//> 带标签的联合体
typedef struct
{
  ValueType type;
  union
  {
    bool boolean;
    double number;

    Obj *obj;

  } as; // [as]
} Value;
//< 带标签的联合体
//> Types of Values is-macros
#define IS_OBJ(value)     ((value).type == VAL_OBJ)

#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_NUMBER(value)  ((value).type == VAL_NUMBER)
//> Strings is-obj
#define IS_OBJ(value) ((value).type == VAL_OBJ)
//< Strings is-obj
//< Types of Values is-macros
//> Types of Values as-macros

//> Strings as-obj
#define AS_OBJ(value) ((value).as.obj)

#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)


#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL ((Value){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})
//> Strings obj-val
/*
    定义了一个宏
    生成一个ValueType = VAL_OBJ的Value
*/
#define OBJ_VAL(object) ((Value){VAL_OBJ, {.obj = (Obj *)object}})



//< Optimization end-if-nan-boxing
//> value-array

typedef struct
{
  int capacity;
  int count;
  Value *values;
} ValueArray;
//< value-array
//> array-fns-h

//> Types of Values values-equal-h
bool valuesEqual(Value a, Value b);
//< Types of Values values-equal-h
void initValueArray(ValueArray *array);
void writeValueArray(ValueArray *array, Value value);
void freeValueArray(ValueArray *array);
//< array-fns-h
//> print-value-h
void printValue(Value value);
//< print-value-h

#endif
