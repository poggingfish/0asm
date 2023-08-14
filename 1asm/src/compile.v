module main

import os

fn compile(nodes []Node, mut file os.File) {
	for i in nodes{
		if i.nodetype == NodeType.push {
			if i.arg.sel == TypeSel.s{
				file.write_string("ia0 \"${i.arg.str_type}\" ") or { panic(err) }
			} else {
				file.write_string("ia0 ${i.arg.int_type} ") or { panic(err) }
			}
		} else if i.nodetype == NodeType.plus {
			file.write_string("m+ ") or { panic(err) }
		} else if i.nodetype == NodeType.print {
			file.write_string("i1 ") or { panic(err) }
		}
	}
}