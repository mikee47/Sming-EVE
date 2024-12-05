'''Take a look at the 'display_list.csv' and 'coprocessor.csv' files

These are provided with the EVE asset builder.

'''
# import binascii

def dump_display_list():
    '''Parse 'display_list.csv' obtained from the EVE Asset Builder tool.

    Each line has 8 comma-separated fields. As far as I can tell it breaks down like this:

    0   Command code (8-bit value)
    1   Command name
    2   Parameters (A) MSB bit numbers (space-separated)
    3   Parameters (A) names (space-separated)
    4   Parameters (B) MSB bit numbers (space-separated)
    5   Parameters (B) names (space-separated)
    6   Parameters (A) values list
    7   Parameters (B) values list

    (A) is for FT81x, (B) is for BT81x

    However, there are inconsistencies so if used would definitely need thorough checking.
    Providing parameter argument values is useful.
    Also a better way to identify bit positions as it's easy to check against datasheet.

    However, despite being less verbose it's also less readable.

    grep finds no references to this file in the software so it's probably out of date.
    It's an interesting format though. Certainly more compact than using python.

    '''

    def print_params(bits, names, arg_lists):
        if bits == '' and names == '':
            return
        if bits == '-1':
            print('   NOT SUPPORTED')
            return
        bits = bits.split(' ')
        names = names.split(' ')
        args_map = {}
        if arg_lists:
            arg_lists = arg_lists.split('|')
            for arg_list in arg_lists:
                arg_list = arg_list.split(' ')
                num = int(arg_list[0])
                arg_list = [(int(arg_list[i]), arg_list[i+1]) for i in range(1, len(arg_list), 2)]
                args_map[int(num)] = arg_list
        start_bit = 0
        for i, (end_bit, name) in enumerate(zip(bits, names)):
            print(f'  {start_bit}-{end_bit}: {name}')
            if args := args_map.get(i+1):
                for k, v in args:
                    print(f'    {k} = {v}')
            start_bit = int(end_bit) + 1


    with open('display_list.csv', 'r') as f:
        for line in f:
            line = line.rstrip()
            fields = line.split(',')
            assert len(fields) == 8
            print()
            print(fields[0], fields[1])
            print('FT81x')
            print_params(fields[2], fields[3], fields[6])
            print('BT81x')
            print_params(fields[4], fields[5], fields[7])


if __name__ == '__main__':
    dump_display_list()
