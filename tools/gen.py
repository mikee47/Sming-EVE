import eve
from eve import align
from sys import stdout
import os
import json

def camel(s: str) -> str:
	s = s.lower()
	for prefix in ['get', 'set', 'load', 'save', 'play', 'video']:
		if s.startswith(prefix):
			i = len(prefix)
			return s[:i] + s[i].upper() + s[i+1:]
	return s

def print_layout():
	for cmd in eve.CoprocessorCommands:
		assert isinstance(cmd.description, str), cmd.name
		print(f'CMD_{cmd.name}')
		print(f'  +0 FFFFFF{cmd.code:02x}')
		for param in cmd.params or []:
			bitcount = param.typedef.bitcount
			s = '' if bitcount % 8 == 0 else f': {bitcount}'
			if param.typedef is eve.Options:
				options = [opt.name for opt in eve.OptionDefs if cmd.name in opt.commands]
				s += '  // ' + ', '.join(options)
			print(f'  +{param.offset} {param.typedef.name} {param.name}{s}')
		print(f'  +{cmd.size}')


def generate_dlcmd_cpp():
	lines = []
	for cmd in sorted(eve.DisplayListCommands, key = lambda x: x.name):
		assert isinstance(cmd.description, str), cmd.name

		params = cmd.params or []

		if cmd.code & 0xC0:
			code_bits = 2
			code = cmd.code >> 6
		else:
			code_bits = 8
			code = cmd.code
		packing_bits = 32 - cmd.total_bits - code_bits

		def get_type(param):
			return 'int32_t' if param.typedef.signed else 'uint32_t'
		inst_def = [
			*(f'{get_type(param)} {param.name}: {param.bitcount};' for param in params),
			f'uint32_t unused: {packing_bits};' if packing_bits else None,
			f'uint8_t code: {code_bits};',
		]
		def cast_param(param):
			return param.name if param.typedef.is_int else f'{get_type(param)}({param.name})'
		inst = [cast_param(param) for param in params]
		if packing_bits:
			inst.append('0')
		inst.append(f'0x{code:02x}')
		inst = ', '.join(inst)
		code = [
			'struct Inst {',
			inst_def,
			'};',
			'static_assert(sizeof(Inst) == 4, "Bad Inst");',
			f'Inst inst{{{inst}}};',
			'write(&inst, sizeof(inst));'
		]
		name = cmd.name.lower()
		if name == 'return':
			name += '_'
		args = [f'{param.typedef.name} {param.name}' for param in reversed(params)]
		lines += [
			f'void {name}({", ".join(args)})',
			'{',
			code,
			'}',
			'',
		]

	return lines


def generate_cpcmd_cpp():
	lines = []
	for cmd in sorted(eve.CoprocessorCommands, key = lambda x: x.name):
		assert isinstance(cmd.description, str), cmd.name

		params = cmd.params or []

		# Deal with CString, not in structure but append separately
		cstring = None
		datablock = None
		if params:
			if params[-1].typedef is eve.CString:
				params = params[:-1]
				cstring = cmd.params[-1]
			elif isinstance(params[-1].typedef, eve.DataBlock):
				params = params[:-1]
				datablock = cmd.params[-1]
				datablock_length_param = datablock.typedef.length_param or f'{datablock.name}_length'


		args = [f'{param.typedef.name} {param.name}' for param in params]
		if cstring:
			args += [f'const String& {cstring.name}']
		elif datablock:
			lenstr = '' if datablock.typedef.length_param else f', unsigned {datablock_length_param}'
			args += [f'const void* {datablock.name}{lenstr}']

		inst_def = [
			'uint32_t code;',
			*(f'{param.basetype} {param.name};' for param in params)
		]
		def cast_param(param):
			return param.name if param.basetype == param.typedef.name else f'{param.basetype}({param.name})'
		inst = ', '.join([
			f'0x{0xffffff00 | cmd.code:08x}',
			*(cast_param(param) for param in params)
		])
		code = [
			'struct Inst {',
			inst_def,
			'};',
			'static_assert(sizeof(Inst) % 4 == 0, "Inst not aligned");',
			f'Inst inst{{{inst}}};',
			'write(&inst, sizeof(inst));'
		]
		if cstring:
			code += [f'write({cstring.name});']
		elif datablock:
			code += [
				f'write({datablock.name}, {datablock_length_param});',
				'align();'
			]
		lines += [
			f'void {cmd.name.lower()}({", ".join(args)})',
			'{',
			code,
			'}',
			'',
		]

	return lines


def generate_romfonts_cpp():
	LOG2_BPP = dict([
		(1, 0),
		(2, 1),
		(4, 2),
		(8, 3),
	])
	lines = []
	path = os.path.dirname(__file__)
	for handle in range(16, 35):
		filename = os.path.join(path, f'../resource/fonts/romfont{handle}.evf')
		with open(filename, 'r') as f:
			data = json.load(f)
		char_width = data['char_width']
		firstchar = -1
		lastchar = -1
		for i, c in enumerate(char_width):
			if c:
				lastchar = i
				if firstchar < 0:
					firstchar = i
		char_width = char_width[firstchar:lastchar+1]
		stride = data['stride']
		width = data['width']
		height = data['height']
		bitmap = data['bitmap']
		bitmap = int(bitmap, 0) + firstchar * stride * height
		numchars = 1 + lastchar - firstchar
		if handle in [17, 19]:
			firstchar += 128
		lines += [
			'{',
			[
				f'// Font {handle}',
				'.char_width = {' + ''.join(f'{w},' for w in char_width) + '},',
				f'.alpha = {LOG2_BPP[data['bpp']]},',
				f'.stride = {stride},',
				f'.width = {width},',
				f'.height = {height},',
				f'.descent = {data['descent']},',
				f'.firstchar = {firstchar},',
				f'.numchars = {numchars},',
				f'.bitmap = 0x{bitmap:x},',
			],
			'},'
		]
	return lines


def write_lines(f, items: list, indent: str = ''):
	for item in items:
		if item:
			if isinstance(item, str):
				f.write(f'{indent}{item}\n')
			else:
				write_lines(f, item, indent + '    ')
		elif item is not None:
			f.write('\n')


def main():
	header = [
		'/*',
		' *',
		' * This file is auto-generated.',
		' *',
		' */',
		''
	]

	with open('dlcmd.h', 'w') as f:
		write_lines(f, header + generate_dlcmd_cpp())

	with open('cpcmd.h', 'w') as f:
		write_lines(f, header + generate_cpcmd_cpp())

	with open('romfonts.cpp.txt', 'w') as f:
		write_lines(f, header + generate_romfonts_cpp())


if __name__ == '__main__':
    main()
