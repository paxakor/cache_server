from random import randint as ri
import subprocess as sp

def run_prog(args, data):
    with sp.Popen(args, stdin = sp.PIPE, stdout = sp.PIPE) as prog:
        prog.stdin.write(bytes(data, 'utf-8'))
        out, err = prog.communicate()
        return out.decode("utf-8")

def test(seed_gen, gen, args, solver, decoder, n = 16):
    for _ in range(n):
        seed = seed_gen()
        data = gen(seed)
        ret = run_prog(args, data)
        ans = decoder(ret)
        correct = solver(seed)
        if correct != ans:
            return seed, correct, ans
    return True

def sample(seq, n):
    return [seq[ri(0, len(seq) - 1)] for _ in range(n)]
