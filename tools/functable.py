from sys import argv
from math import sin, pi

def main():
    print("#ifndef EMPR_GENERATED")
    # print(const_table("double", "SIN_DOUBLE", table_func(lambda x: sin(x*pi/128), 0, 256, 1)))
    # print(const_table("uint16_t", "SIN_UINT16", table_func(lambda x: int((sin(x*pi/128)+1)*32767.5), 0, 256, 1)))
    print(const_table("int16_t", "SIN_INT16", table_func(lambda x: int(sin(x*pi/128)*32767), 0, 256, 1)))
    print(const_table("int16_t", "TRI_INT16", table_func(
        lambda x: int(
            (8*x) if 0 <= x < 64 else 
            (1024-8*x) if 64 <= x < 192 else 
            (8*x-2048)
        ), 0, 256, 1)
    ))
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