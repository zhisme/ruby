/**********************************************************************

  type_system.h - Static type system for primitive types

  Copyright (C) 2025 Research Project

  This file implements a parse-time static type system for Ruby
  primitive types as a research project to evaluate the impact
  of static typing on Ruby's design philosophy.

**********************************************************************/

#ifndef RUBY_TYPE_SYSTEM_H
#define RUBY_TYPE_SYSTEM_H 1

#include "ruby/internal/config.h"
#include "node.h"

/* Primitive type enumeration */
typedef enum {
    PRIM_TYPE_UNKNOWN = 0,  /* Type not known or not annotated */
    PRIM_TYPE_INTEGER,      /* Integer */
    PRIM_TYPE_FLOAT,        /* Float */
    PRIM_TYPE_STRING,       /* String */
    PRIM_TYPE_SYMBOL,       /* Symbol */
    PRIM_TYPE_TRUE,         /* TrueClass */
    PRIM_TYPE_FALSE,        /* FalseClass */
    PRIM_TYPE_NIL           /* NilClass */
} primitive_type_t;

/* Type information structure */
typedef struct rb_type_info {
    primitive_type_t type;
    int lineno;
    int column;
} rb_type_info_t;

/* Parameter type information */
typedef struct rb_param_type {
    ID param_name;              /* Parameter identifier */
    primitive_type_t type;      /* Parameter type */
    struct rb_param_type *next; /* Next parameter in list */
} rb_param_type_t;

/* Method type signature */
typedef struct rb_method_signature {
    ID method_name;                    /* Method identifier */
    rb_param_type_t *param_types;      /* List of parameter types */
    primitive_type_t return_type;      /* Return type */
    int has_return_type;               /* Whether return type is specified */
    int lineno;                        /* Line number of definition */
} rb_method_signature_t;

/* Type system functions */

/* Convert type enum to string name */
const char *primitive_type_name(primitive_type_t type);

/* Parse type name string to type enum */
primitive_type_t primitive_type_from_name(const char *name);

/* Create a new type info structure */
rb_type_info_t *rb_type_info_new(primitive_type_t type, int lineno, int column);

/* Create a new parameter type */
rb_param_type_t *rb_param_type_new(ID param_name, primitive_type_t type);

/* Append parameter type to list */
rb_param_type_t *rb_param_type_append(rb_param_type_t *list, rb_param_type_t *param);

/* Create a new method signature */
rb_method_signature_t *rb_method_signature_new(ID method_name, int lineno);

/* Set return type for method signature */
void rb_method_signature_set_return_type(rb_method_signature_t *sig, primitive_type_t type);

/* Add parameter type to method signature */
void rb_method_signature_add_param(rb_method_signature_t *sig, ID param_name, primitive_type_t type);

/* Find parameter type in signature */
primitive_type_t rb_method_signature_get_param_type(rb_method_signature_t *sig, ID param_name);

/* Free method signature */
void rb_method_signature_free(rb_method_signature_t *sig);

/* Type inference from AST nodes */
primitive_type_t rb_infer_node_type(NODE *node);

/* Check if two types are compatible */
int rb_types_compatible(primitive_type_t expected, primitive_type_t actual);

/* Type checking error messages */
void rb_type_error_mismatch(struct parser_params *p, const char *context,
                           primitive_type_t expected, primitive_type_t actual,
                           int lineno, int column);

void rb_type_error_return(struct parser_params *p, ID method_name,
                         primitive_type_t expected, primitive_type_t actual,
                         int lineno, int column);

void rb_type_error_param(struct parser_params *p, ID method_name, ID param_name,
                        primitive_type_t expected, primitive_type_t actual,
                        int lineno, int column);

#endif /* RUBY_TYPE_SYSTEM_H */
