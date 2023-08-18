module main

import os

fn compile(nodes []Node, mut file os.File, mut proc_ptr &int, mut procs map[string]int) {
	for i in nodes { // Reserve function ptrs
		if i.nodetype == NodeType.proc {
			procs[i.arg.str_type] = proc_ptr
			proc_ptr++
		}
	}
	for i in nodes {
		if i.nodetype == NodeType.push {
			if i.arg.sel == TypeSel.s {
				file.write_string("ia0 \"${i.arg.str_type}\" ") or { panic(err) }
			} else {
				file.write_string('ia0 ${i.arg.int_type} ') or { panic(err) }
			}
		} else if i.nodetype == NodeType.plus {
			file.write_string('m+ ') or { panic(err) }
		} else if i.nodetype == NodeType.print {
			file.write_string('i1 ') or { panic(err) }
		} else if i.nodetype == NodeType.putc {
			file.write_string('i2 ') or { panic(err) }
		} else if i.nodetype == NodeType.equal {
			file.write_string('i8 ') or { panic(err) }
		} else if i.nodetype == NodeType.proc {
			file.write_string('ia3 ${procs[i.arg.str_type]} ;; ${i.arg.str_type}\n') or {
				panic(err)
			}
			compile(parse(i.children), mut file, mut proc_ptr, mut procs)
			file.write_string('\ni14 ;; End of ${i.arg.str_type}\n') or { panic(err) }
		} else if i.nodetype == NodeType.call {
			file.write_string('ia0 0 i13 ia4 ${procs[i.arg.str_type]} ;; Call ${i.arg.str_type}\n') or {
				panic(err)
			}
		}
	}
}
