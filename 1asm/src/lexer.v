module main

pub fn lex(contents string) []Node {
	mut nodes := []Node{}
	nums := ['1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-']
	mut i := 0
	mut single_char_toks := {
		'+': NodeType.plus
		'.': NodeType.print
		'{': NodeType.open_curly
		'}': NodeType.close_curly
	}
	mut string_toks := {
		'dup':    NodeType.dup
		'proc':   NodeType.proc
		'call':   NodeType.call
		'swap':   NodeType.swap
		'free':   NodeType.free
		'set':    NodeType.set
		'get':    NodeType.get
		'alloc':  NodeType.alloc
		'if':     NodeType.ifstmt
		'!if':    NodeType.notifstmt
		'==':     NodeType.equal
		'getc':   NodeType.getc
		'flush':  NodeType.flush
		'openfd': NodeType.fdopen
		'fputc':  NodeType.fputc
		'fputs':  NodeType.fputs
		'fclose': NodeType.fclose
		'fopen':  NodeType.fopen
		'null?':  NodeType.isnull
		'exit':   NodeType.exit
	}
	mut linenum := 0
	mut charnum := 0
	for i < contents.len {
		stri := contents[i].ascii_str()
		if nums.contains(stri) {
			mut buff := '' // Buffer for storing the integer
			inital_i := i // Save the initial value of i for validating integers.
			for i < contents.len && nums.contains(contents[i].ascii_str()) {
				if contents[i].ascii_str() == '-' && i != inital_i {
					err('Invalid integer', linenum, charnum, true)
					exit(1)
				}
				buff += contents[i].ascii_str()
				inc_i(mut i, mut charnum)
			}
			i--
			nodes << Node{
				nodetype: NodeType.push
				arg: Type{
					sel: TypeSel.i
					int_type: buff.int()
				}
				chr: charnum
				ln: linenum
			}
		} else if stri == '\n' {
			linenum++
			charnum = -1
		} else if stri == '"' {
			mut buff := ''
			inc_i(mut i, mut charnum)
			for i < contents.len && contents[i].ascii_str() != '"' {
				if contents[i].ascii_str() == '\n' {
					err('Unexpected newline on string.', linenum, charnum, true)
				}
				buff += contents[i].ascii_str()
				if contents[i].ascii_str() == '\\' {
					if i + 1 < contents.len && contents[i + 1].ascii_str() == '"' {
						buff += '"'
						inc_i(mut i, mut charnum)
					}
				}
				inc_i(mut i, mut charnum)
			}
			nodes << Node{
				nodetype: NodeType.push
				arg: Type{
					sel: TypeSel.s
					str_type: buff
				}
				chr: charnum
				ln: linenum
			}
		} else if single_char_toks.keys().contains(contents[i].ascii_str()) {
			nodes << Node{
				nodetype: single_char_toks[contents[i].ascii_str()]
				chr: charnum
				ln: linenum
			}
		} else if stri == ' ' || stri == '\t' {
		} else {
			mut ident := ''
			for i < contents.len && ![' ', '\n'].contains(contents[i].ascii_str()) {
				ident += contents[i].ascii_str()
				inc_i(mut i, mut charnum)
			}
			if ident == 'putc' {
				nodes << Node{
					nodetype: NodeType.putc
					chr: charnum
					ln: linenum
				}
			} else {
				if string_toks.keys().contains(ident) {
					nodes << Node{
						nodetype: string_toks[ident]
					}
				} else {
					nodes << Node{
						nodetype: NodeType.ident
						arg: Type{
							sel: TypeSel.s
							str_type: ident
						}
						chr: charnum
						ln: linenum
					}
				}
			}
			if i < contents.len && contents[i].ascii_str() == '\n' {
				linenum++
				charnum = -1
			}
		}

		inc_i(mut i, mut charnum)
	}
	return nodes
}
