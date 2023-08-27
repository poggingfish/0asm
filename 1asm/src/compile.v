module main

import os

fn compile(nodes []Node, mut file os.File, mut proc_ptr &int, mut procs map[string]int, first bool) {
	for i in nodes { // Reserve function ptrs
		if i.nodetype == NodeType.proc {
			procs[i.arg.str_type] = proc_ptr
			proc_ptr++
		}
	}
	if first {
		file.write_string('ia4 ${int(procs['Main']).str()}\n') or { panic(err) }
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
			compile(parse(i.children), mut file, mut proc_ptr, mut procs, false)
			file.write_string('\ni14 ;; End of ${i.arg.str_type}\n') or { panic(err) }
		} else if i.nodetype == NodeType.call {
			file.write_string('ia0 0 i13 ia4 ${procs[i.arg.str_type]} ;; Call ${i.arg.str_type}\n') or {
				panic(err)
			}
		} else if i.nodetype == NodeType.dup {
			file.write_string('i11 ') or { panic(err) }
		} else if i.nodetype == NodeType.swap {
			file.write_string('i7 ') or { panic(err) }
		} else if i.nodetype == NodeType.alloc {
			file.write_string("ia15 \"${i.arg.str_type}\" ") or { panic(err) }
		} else if i.nodetype == NodeType.free {
			file.write_string("ia16 \"${i.arg.str_type}\" ") or { panic(err) }
		} else if i.nodetype == NodeType.set {
			file.write_string("ia5 \"${i.arg.str_type}\" ") or { panic(err) }
		} else if i.nodetype == NodeType.get {
			file.write_string("ia6 \"${i.arg.str_type}\" ") or { panic(err) }
		} else if i.nodetype == NodeType.equal {
			file.write_string('i8 ') or { panic(err) }
		} else if i.nodetype == NodeType.ifstmt {
			ifproc := proc_ptr++
			continue_proc := proc_ptr++
			file.write_string('ia9 ${int(ifproc)} ia4 ${int(continue_proc)} ia3 ${int(ifproc)} ;; If statement starts here\n') or {
				panic(err)
			}
			compile(parse(i.children), mut file, mut proc_ptr, mut procs, false)
			file.write_string('ia3 ${int(continue_proc)} ;; If statement ends here\n') or {
				panic(err)
			}
		} else if i.nodetype == NodeType.notifstmt {
			ifproc := proc_ptr++
			continue_proc := proc_ptr++
			file.write_string('ia10 ${int(ifproc)} ia4 ${int(continue_proc)} ia3 ${int(ifproc)} ;; !If statement starts here\n') or {
				panic(err)
			}
			compile(parse(i.children), mut file, mut proc_ptr, mut procs, false)
			file.write_string('ia3 ${int(continue_proc)} ;; !If statement ends here\n') or {
				panic(err)
			}
		} else if i.nodetype == NodeType.getc {
			file.write_string('i21 ') or { panic(err) }
		} else if i.nodetype == NodeType.flush {
			file.write_string('i22 ') or { panic(err) }
		} else if i.nodetype == NodeType.fdopen {
			file.write_string('i23 ') or { panic(err) }
		} else if i.nodetype == NodeType.fputc {
			file.write_string('i24 ') or { panic(err) }
		} else if i.nodetype == NodeType.fputs {
			file.write_string('i25 ') or { panic(err) }
		} else if i.nodetype == NodeType.fclose {
			file.write_string('i26 ') or { panic(err) }
		} else if i.nodetype == NodeType.fopen {
			file.write_string('i27 ') or { panic(err) }
		} else if i.nodetype == NodeType.isnull {
			file.write_string('i28 ') or { panic(err) }
		} else if i.nodetype == NodeType.exit {
			file.write_string('i29 ') or { panic(err) }
		} else if i.nodetype == NodeType.stacktwopush {
			file.write_string('i30 ') or { panic(err) }
		} else if i.nodetype == NodeType.stacktwopop {
			file.write_string('i31 ') or { panic(err) }
		} else if i.nodetype == NodeType.ident {
			if procs.keys().contains(i.arg.str_type) {
				file.write_string('ia0 0 i13 ia4 ${procs[i.arg.str_type]} ;; Call ${i.arg.str_type}\n') or {
					panic(err)
				}
			} else {
				err('Unexpected ident', i.ln, i.chr, true)
			}
		}
	}
}
