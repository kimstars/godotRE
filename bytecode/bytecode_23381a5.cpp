/*************************************************************************/
/*  bytecode_23381a5.cpp                                                 */
/*************************************************************************/

#include "core/io/marshalls.h"
#include "core/string/print_string.h"
#include "core/templates/rb_map.h"

#include "bytecode_23381a5.h"

static const char *func_names[] = {

	"sin",
	"cos",
	"tan",
	"sinh",
	"cosh",
	"tanh",
	"asin",
	"acos",
	"atan",
	"atan2",
	"sqrt",
	"fmod",
	"fposmod",
	"floor",
	"ceil",
	"round",
	"abs",
	"sign",
	"pow",
	"log",
	"exp",
	"is_nan",
	"is_inf",
	"ease",
	"decimals",
	"stepify",
	"lerp",
	"dectime",
	"randomize",
	"randi",
	"randf",
	"rand_range",
	"seed",
	"rand_seed",
	"deg2rad",
	"rad2deg",
	"linear2db",
	"db2linear",
	"max",
	"min",
	"clamp",
	"nearest_po2",
	"weakref",
	"funcref",
	"convert",
	"typeof",
	"type_exists",
	"char",
	"str",
	"print",
	"printt",
	"prints",
	"printerr",
	"printraw",
	"var2str",
	"str2var",
	"var2bytes",
	"bytes2var",
	"range",
	"load",
	"inst2dict",
	"dict2inst",
	"hash",
	"Color8",
	"ColorN",
	"print_stack",
	"instance_from_id",
};

enum Token {

	TK_EMPTY,
	TK_IDENTIFIER,
	TK_CONSTANT,
	TK_SELF,
	TK_BUILT_IN_TYPE,
	TK_BUILT_IN_FUNC,
	TK_OP_IN,
	TK_OP_EQUAL,
	TK_OP_NOT_EQUAL,
	TK_OP_LESS,
	TK_OP_LESS_EQUAL,
	TK_OP_GREATER,
	TK_OP_GREATER_EQUAL,
	TK_OP_AND,
	TK_OP_OR,
	TK_OP_NOT,
	TK_OP_ADD,
	TK_OP_SUB,
	TK_OP_MUL,
	TK_OP_DIV,
	TK_OP_MOD,
	TK_OP_SHIFT_LEFT,
	TK_OP_SHIFT_RIGHT,
	TK_OP_ASSIGN,
	TK_OP_ASSIGN_ADD,
	TK_OP_ASSIGN_SUB,
	TK_OP_ASSIGN_MUL,
	TK_OP_ASSIGN_DIV,
	TK_OP_ASSIGN_MOD,
	TK_OP_ASSIGN_SHIFT_LEFT,
	TK_OP_ASSIGN_SHIFT_RIGHT,
	TK_OP_ASSIGN_BIT_AND,
	TK_OP_ASSIGN_BIT_OR,
	TK_OP_ASSIGN_BIT_XOR,
	TK_OP_BIT_AND,
	TK_OP_BIT_OR,
	TK_OP_BIT_XOR,
	TK_OP_BIT_INVERT,
	//TK_OP_PLUS_PLUS,
	//TK_OP_MINUS_MINUS,
	TK_CF_IF,
	TK_CF_ELIF,
	TK_CF_ELSE,
	TK_CF_FOR,
	TK_CF_DO,
	TK_CF_WHILE,
	TK_CF_SWITCH,
	TK_CF_CASE,
	TK_CF_BREAK,
	TK_CF_CONTINUE,
	TK_CF_PASS,
	TK_CF_RETURN,
	TK_PR_FUNCTION,
	TK_PR_CLASS,
	TK_PR_EXTENDS,
	TK_PR_ONREADY,
	TK_PR_TOOL,
	TK_PR_STATIC,
	TK_PR_EXPORT,
	TK_PR_SETGET,
	TK_PR_CONST,
	TK_PR_VAR,
	TK_PR_ENUM,
	TK_PR_PRELOAD,
	TK_PR_ASSERT,
	TK_PR_YIELD,
	TK_PR_SIGNAL,
	TK_PR_BREAKPOINT,
	TK_PR_REMOTE,
	TK_PR_SYNC,
	TK_PR_MASTER,
	TK_PR_SLAVE,
	TK_BRACKET_OPEN,
	TK_BRACKET_CLOSE,
	TK_CURLY_BRACKET_OPEN,
	TK_CURLY_BRACKET_CLOSE,
	TK_PARENTHESIS_OPEN,
	TK_PARENTHESIS_CLOSE,
	TK_COMMA,
	TK_SEMICOLON,
	TK_PERIOD,
	TK_QUESTION_MARK,
	TK_COLON,
	TK_NEWLINE,
	TK_CONST_PI,
	TK_ERROR,
	TK_EOF,
	TK_CURSOR, //used for code completion
	TK_MAX
};

Error GDScriptDecomp_23381a5::decompile_buffer(Vector<uint8_t> p_buffer) {
	//Cleanup
	script_text = String();

	//Load bytecode
	Vector<StringName> identifiers;
	Vector<Variant> constants;
	VMap<uint32_t, uint32_t> lines;
	Vector<uint32_t> tokens;

	const uint8_t *buf = p_buffer.ptr();
	int total_len = p_buffer.size();
	ERR_FAIL_COND_V(p_buffer.size() < 24 || p_buffer[0] != 'G' || p_buffer[1] != 'D' || p_buffer[2] != 'S' || p_buffer[3] != 'C', ERR_INVALID_DATA);

	int version = decode_uint32(&buf[4]);
	if (version != bytecode_version) {
		ERR_FAIL_COND_V(version > bytecode_version, ERR_INVALID_DATA);
	}
	int identifier_count = decode_uint32(&buf[8]);
	int constant_count = decode_uint32(&buf[12]);
	int line_count = decode_uint32(&buf[16]);
	int token_count = decode_uint32(&buf[20]);

	const uint8_t *b = &buf[24];
	total_len -= 24;

	identifiers.resize(identifier_count);
	for (int i = 0; i < identifier_count; i++) {
		int len = decode_uint32(b);
		ERR_FAIL_COND_V(len > total_len, ERR_INVALID_DATA);
		b += 4;
		Vector<uint8_t> cs;
		cs.resize(len);
		for (int j = 0; j < len; j++) {
			cs.write[j] = b[j] ^ 0xb6;
		}

		cs.write[cs.size() - 1] = 0;
		String s;
		s.parse_utf8((const char *)cs.ptr());
		b += len;
		total_len -= len + 4;
		identifiers.write[i] = s;
	}

	constants.resize(constant_count);
	for (int i = 0; i < constant_count; i++) {
		Variant v;
		int len;
		Error err = decode_variant_2(v, b, total_len, &len);
		if (err) {
			error_message = RTR("Invalid constant");
			return err;
		}
		b += len;
		total_len -= len;
		constants.write[i] = v;
	}

	ERR_FAIL_COND_V(line_count * 8 > total_len, ERR_INVALID_DATA);

	for (int i = 0; i < line_count; i++) {
		uint32_t token = decode_uint32(b);
		b += 4;
		uint32_t linecol = decode_uint32(b);
		b += 4;

		lines.insert(token, linecol);
		total_len -= 8;
	}

	tokens.resize(token_count);

	for (int i = 0; i < token_count; i++) {
		ERR_FAIL_COND_V(total_len < 1, ERR_INVALID_DATA);

		if ((*b) & TOKEN_BYTE_MASK) { //little endian always
			ERR_FAIL_COND_V(total_len < 4, ERR_INVALID_DATA);

			tokens.write[i] = decode_uint32(b) & ~TOKEN_BYTE_MASK;
			b += 4;
		} else {
			tokens.write[i] = *b;
			b += 1;
			total_len--;
		}
	}

	//Decompile script
	String line;
	int indent = 0;

	Token prev_token = TK_NEWLINE;
	for (int i = 0; i < tokens.size(); i++) {
		switch (Token(tokens[i] & TOKEN_MASK)) {
			case TK_EMPTY: {
				//skip
			} break;
			case TK_IDENTIFIER: {
				uint32_t identifier = tokens[i] >> TOKEN_BITS;
				ERR_FAIL_COND_V(identifier >= (uint32_t)identifiers.size(), ERR_INVALID_DATA);
				line += String(identifiers[identifier]);
			} break;
			case TK_CONSTANT: {
				uint32_t constant = tokens[i] >> TOKEN_BITS;
				ERR_FAIL_COND_V(constant >= (uint32_t)constants.size(), ERR_INVALID_DATA);
				line += get_constant_string(constants, constant);
			} break;
			case TK_SELF: {
				line += "self";
			} break;
			case TK_BUILT_IN_TYPE: {
				line += get_type_name_v2(tokens[i] >> TOKEN_BITS);
			} break;
			case TK_BUILT_IN_FUNC: {
				line += func_names[tokens[i] >> TOKEN_BITS];
			} break;
			case TK_OP_IN: {
				_ensure_space(line);
				line += "in ";
			} break;
			case TK_OP_EQUAL: {
				_ensure_space(line);
				line += "== ";
			} break;
			case TK_OP_NOT_EQUAL: {
				_ensure_space(line);
				line += "!= ";
			} break;
			case TK_OP_LESS: {
				_ensure_space(line);
				line += "< ";
			} break;
			case TK_OP_LESS_EQUAL: {
				_ensure_space(line);
				line += "<= ";
			} break;
			case TK_OP_GREATER: {
				_ensure_space(line);
				line += "> ";
			} break;
			case TK_OP_GREATER_EQUAL: {
				_ensure_space(line);
				line += ">= ";
			} break;
			case TK_OP_AND: {
				_ensure_space(line);
				line += "and ";
			} break;
			case TK_OP_OR: {
				_ensure_space(line);
				line += "or ";
			} break;
			case TK_OP_NOT: {
				_ensure_space(line);
				line += "not ";
			} break;
			case TK_OP_ADD: {
				_ensure_space(line);
				line += "+ ";
			} break;
			case TK_OP_SUB: {
				_ensure_space(line);
				line += "- ";
				//TODO: do not add space after unary "-"
			} break;
			case TK_OP_MUL: {
				_ensure_space(line);
				line += "* ";
			} break;
			case TK_OP_DIV: {
				_ensure_space(line);
				line += "/ ";
			} break;
			case TK_OP_MOD: {
				_ensure_space(line);
				line += "% ";
			} break;
			case TK_OP_SHIFT_LEFT: {
				_ensure_space(line);
				line += "<< ";
			} break;
			case TK_OP_SHIFT_RIGHT: {
				_ensure_space(line);
				line += ">> ";
			} break;
			case TK_OP_ASSIGN: {
				_ensure_space(line);
				line += "= ";
			} break;
			case TK_OP_ASSIGN_ADD: {
				_ensure_space(line);
				line += "+= ";
			} break;
			case TK_OP_ASSIGN_SUB: {
				_ensure_space(line);
				line += "-= ";
			} break;
			case TK_OP_ASSIGN_MUL: {
				_ensure_space(line);
				line += "*= ";
			} break;
			case TK_OP_ASSIGN_DIV: {
				_ensure_space(line);
				line += "/= ";
			} break;
			case TK_OP_ASSIGN_MOD: {
				_ensure_space(line);
				line += "%= ";
			} break;
			case TK_OP_ASSIGN_SHIFT_LEFT: {
				_ensure_space(line);
				line += "<<= ";
			} break;
			case TK_OP_ASSIGN_SHIFT_RIGHT: {
				_ensure_space(line);
				line += ">>= ";
			} break;
			case TK_OP_ASSIGN_BIT_AND: {
				_ensure_space(line);
				line += "&= ";
			} break;
			case TK_OP_ASSIGN_BIT_OR: {
				_ensure_space(line);
				line += "|= ";
			} break;
			case TK_OP_ASSIGN_BIT_XOR: {
				_ensure_space(line);
				line += "^= ";
			} break;
			case TK_OP_BIT_AND: {
				_ensure_space(line);
				line += "& ";
			} break;
			case TK_OP_BIT_OR: {
				_ensure_space(line);
				line += "| ";
			} break;
			case TK_OP_BIT_XOR: {
				_ensure_space(line);
				line += "^ ";
			} break;
			case TK_OP_BIT_INVERT: {
				_ensure_space(line);
				line += "! ";
			} break;
			//case TK_OP_PLUS_PLUS: {
			//	line += "++";
			//} break;
			//case TK_OP_MINUS_MINUS: {
			//	line += "--";
			//} break;
			case TK_CF_IF: {
				if (prev_token != TK_NEWLINE)
					_ensure_space(line);
				line += "if ";
			} break;
			case TK_CF_ELIF: {
				line += "elif ";
			} break;
			case TK_CF_ELSE: {
				if (prev_token != TK_NEWLINE)
					_ensure_space(line);
				line += "else ";
			} break;
			case TK_CF_FOR: {
				line += "for ";
			} break;
			case TK_CF_DO: {
				line += "do ";
			} break;
			case TK_CF_WHILE: {
				line += "while ";
			} break;
			case TK_CF_SWITCH: {
				line += "swith ";
			} break;
			case TK_CF_CASE: {
				line += "case ";
			} break;
			case TK_CF_BREAK: {
				line += "break";
			} break;
			case TK_CF_CONTINUE: {
				line += "continue";
			} break;
			case TK_CF_PASS: {
				line += "pass";
			} break;
			case TK_CF_RETURN: {
				line += "return ";
			} break;
			case TK_PR_FUNCTION: {
				line += "func ";
			} break;
			case TK_PR_CLASS: {
				line += "class ";
			} break;
			case TK_PR_EXTENDS: {
				if (prev_token != TK_NEWLINE)
					_ensure_space(line);
				line += "extends ";
			} break;
			case TK_PR_ONREADY: {
				line += "onready ";
			} break;
			case TK_PR_TOOL: {
				line += "tool ";
			} break;
			case TK_PR_STATIC: {
				line += "static ";
			} break;
			case TK_PR_EXPORT: {
				line += "export ";
			} break;
			case TK_PR_SETGET: {
				line += " setget ";
			} break;
			case TK_PR_CONST: {
				line += "const ";
			} break;
			case TK_PR_VAR: {
				if (line != String())
					line += " ";
				line += "var ";
			} break;
			case TK_PR_ENUM: {
				line += "enum ";
			} break;
			case TK_PR_PRELOAD: {
				line += "preload";
			} break;
			case TK_PR_ASSERT: {
				line += "assert ";
			} break;
			case TK_PR_YIELD: {
				line += "yield ";
			} break;
			case TK_PR_SIGNAL: {
				line += "signal ";
			} break;
			case TK_PR_BREAKPOINT: {
				line += "breakpoint ";
			} break;
			case TK_PR_REMOTE: {
				line += "remote ";
			} break;
			case TK_PR_SYNC: {
				line += "sync ";
			} break;
			case TK_PR_MASTER: {
				line += "master ";
			} break;
			case TK_PR_SLAVE: {
				line += "slave ";
			} break;
			case TK_BRACKET_OPEN: {
				line += "[";
			} break;
			case TK_BRACKET_CLOSE: {
				line += "]";
			} break;
			case TK_CURLY_BRACKET_OPEN: {
				line += "{";
			} break;
			case TK_CURLY_BRACKET_CLOSE: {
				line += "}";
			} break;
			case TK_PARENTHESIS_OPEN: {
				line += "(";
			} break;
			case TK_PARENTHESIS_CLOSE: {
				line += ")";
			} break;
			case TK_COMMA: {
				line += ", ";
			} break;
			case TK_SEMICOLON: {
				line += ";";
			} break;
			case TK_PERIOD: {
				line += ".";
			} break;
			case TK_QUESTION_MARK: {
				line += "?";
			} break;
			case TK_COLON: {
				line += ":";
			} break;
			case TK_NEWLINE: {
				for (int j = 0; j < indent; j++) {
					script_text += "\t";
				}
				script_text += line + "\n";
				line = String();
				indent = tokens[i] >> TOKEN_BITS;
			} break;
			case TK_CONST_PI: {
				line += "PI";
			} break;
			case TK_ERROR: {
				//skip - invalid
			} break;
			case TK_EOF: {
				//skip - invalid
			} break;
			case TK_CURSOR: {
				//skip - invalid
			} break;
			case TK_MAX: {
				//skip - invalid
			} break;
		}
		prev_token = Token(tokens[i] & TOKEN_MASK);
	}

	if (!line.is_empty()) {
		for (int j = 0; j < indent; j++) {
			script_text += "\t";
		}
		script_text += line + "\n";
	}

	if (script_text == String()) {
		error_message = RTR("Invalid token");
		return ERR_INVALID_DATA;
	}

	return OK;
}
