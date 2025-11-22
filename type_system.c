/**********************************************************************

  type_system.c - Static type system implementation for primitive types

  Copyright (C) 2025 Research Project

**********************************************************************/

#include "ruby/internal/config.h"
#include "type_system.h"
#include "node.h"
#include "rubyparser.h"
#include "internal/parse.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Convert type enum to human-readable string */
const char *
primitive_type_name(primitive_type_t type)
{
    switch (type) {
      case PRIM_TYPE_INTEGER:
        return "Integer";
      case PRIM_TYPE_FLOAT:
        return "Float";
      case PRIM_TYPE_STRING:
        return "String";
      case PRIM_TYPE_SYMBOL:
        return "Symbol";
      case PRIM_TYPE_TRUE:
        return "TrueClass";
      case PRIM_TYPE_FALSE:
        return "FalseClass";
      case PRIM_TYPE_NIL:
        return "NilClass";
      case PRIM_TYPE_UNKNOWN:
      default:
        return "Unknown";
    }
}

/* Parse type name string to type enum */
primitive_type_t
primitive_type_from_name(const char *name)
{
    if (strcmp(name, "Integer") == 0) return PRIM_TYPE_INTEGER;
    if (strcmp(name, "Float") == 0) return PRIM_TYPE_FLOAT;
    if (strcmp(name, "String") == 0) return PRIM_TYPE_STRING;
    if (strcmp(name, "Symbol") == 0) return PRIM_TYPE_SYMBOL;
    if (strcmp(name, "TrueClass") == 0) return PRIM_TYPE_TRUE;
    if (strcmp(name, "FalseClass") == 0) return PRIM_TYPE_FALSE;
    if (strcmp(name, "NilClass") == 0) return PRIM_TYPE_NIL;
    return PRIM_TYPE_UNKNOWN;
}

/* Create a new type info structure */
rb_type_info_t *
rb_type_info_new(primitive_type_t type, int lineno, int column)
{
    rb_type_info_t *info = (rb_type_info_t *)malloc(sizeof(rb_type_info_t));
    if (!info) return NULL;

    info->type = type;
    info->lineno = lineno;
    info->column = column;
    return info;
}

/* Create a new parameter type */
rb_param_type_t *
rb_param_type_new(ID param_name, primitive_type_t type)
{
    rb_param_type_t *param = (rb_param_type_t *)malloc(sizeof(rb_param_type_t));
    if (!param) return NULL;

    param->param_name = param_name;
    param->type = type;
    param->next = NULL;
    return param;
}

/* Append parameter type to list */
rb_param_type_t *
rb_param_type_append(rb_param_type_t *list, rb_param_type_t *param)
{
    if (!list) return param;

    rb_param_type_t *current = list;
    while (current->next) {
        current = current->next;
    }
    current->next = param;
    return list;
}

/* Create a new method signature */
rb_method_signature_t *
rb_method_signature_new(ID method_name, int lineno)
{
    rb_method_signature_t *sig = (rb_method_signature_t *)malloc(sizeof(rb_method_signature_t));
    if (!sig) return NULL;

    sig->method_name = method_name;
    sig->param_types = NULL;
    sig->return_type = PRIM_TYPE_UNKNOWN;
    sig->has_return_type = 0;
    sig->lineno = lineno;
    return sig;
}

/* Set return type for method signature */
void
rb_method_signature_set_return_type(rb_method_signature_t *sig, primitive_type_t type)
{
    if (!sig) return;
    sig->return_type = type;
    sig->has_return_type = 1;
}

/* Add parameter type to method signature */
void
rb_method_signature_add_param(rb_method_signature_t *sig, ID param_name, primitive_type_t type)
{
    if (!sig) return;

    rb_param_type_t *param = rb_param_type_new(param_name, type);
    if (!param) return;

    sig->param_types = rb_param_type_append(sig->param_types, param);
}

/* Find parameter type in signature */
primitive_type_t
rb_method_signature_get_param_type(rb_method_signature_t *sig, ID param_name)
{
    if (!sig || !sig->param_types) return PRIM_TYPE_UNKNOWN;

    rb_param_type_t *current = sig->param_types;
    while (current) {
        if (current->param_name == param_name) {
            return current->type;
        }
        current = current->next;
    }
    return PRIM_TYPE_UNKNOWN;
}

/* Free method signature */
void
rb_method_signature_free(rb_method_signature_t *sig)
{
    if (!sig) return;

    rb_param_type_t *current = sig->param_types;
    while (current) {
        rb_param_type_t *next = current->next;
        free(current);
        current = next;
    }
    free(sig);
}

/* Infer type from AST node (simple literal inference only) */
primitive_type_t
rb_infer_node_type(NODE *node)
{
    if (!node) return PRIM_TYPE_UNKNOWN;

    switch (nd_type(node)) {
      case NODE_INTEGER:
        return PRIM_TYPE_INTEGER;

      case NODE_FLOAT:
        return PRIM_TYPE_FLOAT;

      case NODE_STR:
      case NODE_DSTR:
        return PRIM_TYPE_STRING;

      case NODE_SYM:
      case NODE_DSYM:
        return PRIM_TYPE_SYMBOL;

      case NODE_TRUE:
        return PRIM_TYPE_TRUE;

      case NODE_FALSE:
        return PRIM_TYPE_FALSE;

      case NODE_NIL:
        return PRIM_TYPE_NIL;

      /* For now, we don't infer types across method calls or complex expressions */
      default:
        return PRIM_TYPE_UNKNOWN;
    }
}

/* Check if two types are compatible */
int
rb_types_compatible(primitive_type_t expected, primitive_type_t actual)
{
    /* Unknown types are always compatible (untyped code) */
    if (expected == PRIM_TYPE_UNKNOWN || actual == PRIM_TYPE_UNKNOWN) {
        return 1;
    }

    /* Otherwise, must be exact match for primitives */
    return expected == actual;
}

/* Type error functions removed - error reporting is done directly in compile.c */
