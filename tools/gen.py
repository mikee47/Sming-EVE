import eve
from eve import align


def main():
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

if __name__ == '__main__':
    main()
