module main

fn until_closed(contents []Node, mut ptr &int) []Node {
	mut node_buff := []Node{}
	ptr++
	mut open := 1
	for i in contents[*ptr..] {
		if i.nodetype == NodeType.open_curly {
			open++
		} else if i.nodetype == NodeType.close_curly {
			open--
			if open <= 0 {
				break
			}
		}
		node_buff << i
		ptr++
	}
	if open != 0 {
		err('End of file before closing block', 0, 0, true)
	}
	return node_buff[..node_buff.len]
}

pub fn parse(contents []Node) []Node { // Parser "preprocessor"
	mut nodes := []Node{}
	mut i := 0
	for i < contents.len {
		if contents[i].nodetype == NodeType.proc {
			if i + 1 > contents.len {
				err('End of file before proc name', contents[i].ln, contents[i].chr, true)
			}
			if contents[i + 1].nodetype != NodeType.ident {
				err('Expected ident for proc name', contents[i].ln, contents[i].chr, true)
			}
			proc_name := contents[i + 1].arg.str_type
			i += 2
			nodes << Node{
				nodetype: NodeType.proc
				arg: Type{
					sel: TypeSel.s
					str_type: proc_name
				}
				children: until_closed(contents, mut &i)
			}
		} else if contents[i].nodetype == NodeType.call {
			if i + 1 > contents.len {
				err('End of file before call proc name', contents[i].ln, contents[i].chr,
					true)
			}
			if contents[i + 1].nodetype != NodeType.ident {
				err('Expected ident for call proc name', contents[i].ln, contents[i].chr,
					true)
			}
			proc_name := contents[i + 1].arg.str_type
			i += 1
			nodes << Node{
				nodetype: NodeType.call
				arg: Type{
					sel: TypeSel.s
					str_type: proc_name
				}
			}
		} else {
			nodes << contents[i]
		}
		i++
	}
	return nodes
}
