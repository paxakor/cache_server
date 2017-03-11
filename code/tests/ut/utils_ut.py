from random import randint as ri
import test

N, L = 2048, 16

def rng_str(a, b):
    return ''.join([chr(c) for c in range(ord(a), ord(b))])
letters = rng_str('a','b') + rng_str('A', 'B') + rng_str('0','9')

def word():
    return ''.join(test.sample(letters, ri(1, L)))

def split_test(N, L):
    N = ri(1, N)
    def seed_gen():
        def space():
            spaces = " \t"
            return ''.join(test.sample(spaces, ri(1, L)))
        return [word() for _ in range(N)], [space() for _ in range(N)]

    def data_gen(seed):
        parts = list()
        for i in range(len(seed[0]) + len(seed[1])):
            parts.append(seed[i % 2][i // 2])
        return ''.join(parts)

    def solver(seed):
        return seed[0]

    def decoder(out):
        return (out if out[-1] != '\n' else out[:-1]).split('\n')

    args = ['./ut/utils', 'space']
    res = test.test(seed_gen, data_gen, args, solver, decoder)
    if res != True:
        print(res, file = open("utils.log", 'w'))
        print('utils.split_test: error')
        exit(1)

split_test(N, L)


def split_by_pattern_test(N, L):
    N = ri(1, N)
    def seed_gen():
        return [word() for _ in range(N)], word()

    def data_gen(seed):
        return seed[1] + ' ' + seed[1] + seed[1].join(seed[0])

    def solver(seed):
        return list(filter(lambda x: len(x),
            seed[1].join(seed[0]).split(seed[1])))

    def decoder(out):
        return (out if out[-1] != '\n' else out[:-1]).split('\n')

    args = ['./ut/utils', 'pattern']
    res = test.test(seed_gen, data_gen, args, solver, decoder)
    if res != True:
        print(res, file = open("utils.log", 'w'))
        print('utils.split_by_pattern_test: error')
        exit(1)

split_by_pattern_test(N, L)
