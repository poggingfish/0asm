module main

import os

fn main() {
	if os.args.len != 2 {
		println('Usage: 1asm <file>')
		exit(1)
	}
	mut file := os.create('./out.0asm')!
	file_name := os.args[1]
	file_content := os.read_file(file_name)!
	lexed := lex(file_content)
	nodes := parse(lexed)
	mut proc_ptr := 1 // Reserve 1 proc for end
	mut procs := map[string]int{}
	compile(nodes, mut file, mut &proc_ptr, mut procs)
	file.write_string('\nia3 0\n') or { panic(err) }
	file.close()
}
