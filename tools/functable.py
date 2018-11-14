from sys import argv
from math import sin, pi

def main():
    print("#ifndef EMPR_GENERATED")
    print(const_table("double", "SIN_DOUBLE", table_func(lambda x: sin(x*pi/128), 0, 256, 1)))
    print(const_table("uint16_t", "SIN_UINT16", table_func(lambda x: int((sin(x*pi/128)+1)*32767.5), 0, 256, 1)))
    print("#endif")

def table_func(func, start, end, step):
    return [func(x) for x in range(start, end, step)]

def const_table(type_s, name_s, table):
    return "const {} GENERATED_{}[{}] = {{{}}};".format(
        type_s, name_s.upper(), len(table),
        ",".join(map(str,table))
    )

if __name__ == '__main__':
    main()