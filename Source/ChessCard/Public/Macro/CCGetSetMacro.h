#pragma once


// Declare GetSet Macro
#define DECLARE_GETTER(FuncName, VariableName, Type) \
Type Get##FuncName() const { return VariableName; };

#define DECLARE_SETTER(FuncName, VariableName, Type) \
void Set##FuncName(Type In##FuncName) { VariableName = In##FuncName; };

#define DECLARE_GETTER_IS_BOOL(DesiredName, VariableName) \
bool Is##DesiredName() const { return VariableName; };\

#define DECLARE_GETTER_HAS_BOOL(DesiredName, VariableName) \
bool Has##DesiredName() const { return VariableName; };\

#define DECLARE_GETTER_REF(FuncName, VariableName, Type) \
Type& Get##FuncName() { return VariableName; };

#define DECLARE_GETTER_PTR(FuncName, VariableName, Type) \
Type* Get##FuncName() const { return VariableName; };

#define DECLARE_GETTER_SETTER(FuncName, VariableName, Type) \
Type Get##FuncName() const { return VariableName; };\
void Set##FuncName(Type In##FuncName) { VariableName = In##FuncName; };

#define DECLARE_GETTER_SETTER_REF(FuncName, VariableName, Type) \
Type& Get##FuncName() { return VariableName; };\
void Set##FuncName(Type& In##FuncName) { VariableName = In##FuncName; };

#define DECLARE_GETTER_SETTER_PTR(FuncName, VariableName, Type) \
Type* Get##FuncName() { return VariableName; };\
void Set##FuncName(Type* In##FuncName) { VariableName = In##FuncName; };

#define DECLARE_GETTER_SETTER_IS_BOOL(DesiredName, VariableName) \
bool Is##DesiredName() const { return VariableName; };\
void SetIs##DesiredName(bool In##DesiredName) { VariableName = In##DesiredName; };

#define DECLARE_GETTER_SETTER_HAS_BOOL(DesiredName, VariableName) \
bool Has##DesiredName() const { return VariableName; };\
void SetHas##DesiredName(bool In##DesiredName) { VariableName = In##DesiredName; };