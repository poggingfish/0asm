module main

import os

fn main() {
	if os.args.len != 2{
		println("Usage: 1asm <file>")
		exit(1)
	}
	mut file := os.create("./out.0asm")!
	file_name := os.args[1]
	file_content := os.read_file(file_name)!
	nodes := parse(file_content)
	compile(nodes, mut file)
	file.close()
}