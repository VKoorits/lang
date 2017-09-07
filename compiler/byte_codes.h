#ifndef _BYTE_CODES_H_
#define _BYTE_CODES_H_
//OP_CODES


#define _EXIT (0)
#define _NEW_VAR (1)
#define _DELETE_VAR (2)
#define _PUSH (3)
#define _PUSH_CONST (4)
#define _STORE (5)
#define _BINARY_ADD (6)
#define _BINARY_SUB (7)
#define _MULTIPLY (8)
#define _DIVISION (9)
#define _MOD (10)
#define _INT_DIVISION (11)
#define _POWER (12)
#define _LESS (13)
#define _MORE (14)
#define _JUMP_IF_NOT (15)
#define _GOTO (17)
#define _CALL_STD (18)


static const char EXIT = _EXIT;
static const char NEW_VAR = _NEW_VAR;
static const char DELETE_VAR = _DELETE_VAR;
static const char PUSH = _PUSH;
static const char PUSH_CONST = _PUSH_CONST;
static const char STORE = _STORE;
static const char BINARY_ADD = _BINARY_ADD;
static const char BINARY_SUB = _BINARY_SUB;
static const char MULTIPLY = _MULTIPLY;
static const char DIVISION = _DIVISION;
static const char MOD = _MOD;
static const char INT_DIVISION = _INT_DIVISION;
static const char POWER = _POWER;
static const char LESS = _LESS;
static const char MORE = _MORE;
static const char JUMP_IF_NOT = _JUMP_IF_NOT;
static const char GOTO = _GOTO;
static const char CALL_STD = _CALL_STD;







#endif //_BYTE_CODES_H_
