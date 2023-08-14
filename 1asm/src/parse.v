module main

pub enum TypeSel {
	i
	s
}

pub enum NodeType {
	push
	print

	plus
}

pub struct Type {
	sel TypeSel
	int_type int
	str_type string
}

pub struct Node {
	nodetype NodeType
	arg Type
	ln int
	chr int
}

pub fn inc_i(mut i &int, mut ctr &int){
	i++
	ctr++
}

pub fn err(text string, linenum int, charnum int, fatal bool){
	if !fatal {
		print("[WARN] ")
	}
	else {
		print("[ERR] ")
	}
	println("${text} at ${linenum+1}:${charnum-1}")
	if fatal {
		exit(1)
	}
}

pub fn parse(contents string) []Node {
	mut nodes := []Node{}
	nums := ['1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-']
	mut i := 0
	mut single_char_toks := {
		"+": NodeType.plus
		".": NodeType.print
	}
	mut linenum := 0
	mut charnum := 0
	for i < contents.len {
		stri := contents[i].ascii_str()
		if nums.contains(stri) {
			mut buff := "" // Buffer for storing the integer
			inital_i := i // Save the initial value of i for validating integers.
			for i < contents.len && nums.contains(contents[i].ascii_str()) {
				if contents[i].ascii_str() == '-' && i != inital_i{ // Check if there is a minus symbol at a invalid place
					err("Invalid integer", linenum, charnum, true)
					exit(1)
				}
				buff += contents[i].ascii_str()
				inc_i(mut i, mut charnum)
			}
			i--
			nodes << Node {nodetype: NodeType.push, arg: Type{sel: TypeSel.i, int_type: buff.int()}, chr: charnum, ln: linenum}
		} else if stri == "\n"{
			linenum++
			charnum = -1
		} else if stri == "\""{
			mut buff := ""
			inc_i(mut i, mut charnum)
			for i < contents.len && contents[i].ascii_str() != "\""{
				if contents[i].ascii_str() == "\n"{
					err("Unexpected newline on string.", linenum, charnum, true)
				}
				buff += contents[i].ascii_str()
				if contents[i].ascii_str() == "\\"{
					if i+1 < contents.len && contents[i+1].ascii_str() == "\""{
						buff += "\""
						i++
					}
				}
				inc_i(mut i, mut charnum)
			}
			nodes << Node {nodetype: NodeType.push, arg: Type{sel: TypeSel.s, str_type: buff}, chr: charnum, ln: linenum}
		} else if single_char_toks.keys().contains(contents[i].ascii_str()){
			nodes << Node{ nodetype: single_char_toks[contents[i].ascii_str()], chr: charnum, ln: linenum }
		}

		inc_i(mut i, mut charnum)
	}
	return nodes
}
