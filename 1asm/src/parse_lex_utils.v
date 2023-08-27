module main

pub enum TypeSel {
	i
	s
}

pub enum NodeType {
	push
	print
	putc
	plus
	open_curly
	close_curly
	ident
	equal
	proc
	call
	dup
	swap
	alloc
	free
	get
	set
	ifstmt
	notifstmt
	getc
	flush
	fdopen
	fputc
	fputs
	fclose
	fopen
	isnull
	exit
}

pub struct Type {
	sel      TypeSel
	int_type int
	str_type string
}

pub struct Node {
	nodetype NodeType
	arg      Type
	children []Node
	ln       int
	chr      int
}

pub fn inc_i(mut i &int, mut ctr &int) {
	i++
	ctr++
}

pub fn err(text string, linenum int, charnum int, fatal bool) {
	println('${['[INFO]', '[ERR]'][int(fatal)]} ${text} at ${linenum + 1}:${charnum - 1}')
	if fatal {
		exit(1)
	}
}
