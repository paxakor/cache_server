#! /usr/bin/python3
# Copyright 2017, Pavel Korozevtsev.

from os import remove
from sys import argv
from threading import Thread 
import subprocess as sp

errors = 0

def join_url(host, page):
    return host + page

def join_page_name(host, page):
    return join_url(host, page).replace('/', '_')

def join_page_name_proxy(host, page, thread_id):
    return join_page_name(host, page) + '_' + str(thread_id)

def run_wget(args):
    args = ['wget'] + args
    sp.run(args, stdout=sp.DEVNULL, stderr=sp.DEVNULL, check=True)

def get_original(page):
    url = join_url(*page)
    fname = join_page_name(*page)
    run_wget([url, '-O', fname])

def get_via_proxy(page, thread_id):
    url = join_url(*page)
    fname = join_page_name_proxy(*page, thread_id)
    proxy_args = ['-e', 'use_proxy=on', '-e',
        'http_proxy=127.0.0.1:' + get_via_proxy.port]
    run_wget(proxy_args + [url, '-O', fname])

def for_all(f, xs, *args):
    for x in xs:
        f(x, *args)

def equal_str(lines_a, lines_b, magic):
    if len(lines_a) > len(lines_b):
        lines_a, lines_b = lines_b, lines_a
    fails = len(lines_b) - len(lines_a)
    lines_b = lines_b[:-fails]
    for la, lb in zip(lines_a, lines_b):
        if la != lb:
            fails += 1
    return fails / len(lines_a) < magic

def check(page, thread_id):
    global errors
    fa = join_page_name(*page)
    fb = join_page_name_proxy(*page, thread_id)
    if not equal_str(open(fa).readlines(), open(fb).readlines(), 0.3):
        print(fa, '!=', fb)
        errors += 1
    else:
        remove(fb)

def thread_main(iterations, pages, thread_id):
    for _ in range(iterations):
        for_all(get_via_proxy, pages, thread_id)
        for_all(check, pages, thread_id)

def run_threads(pages, args):
    iters = 1 if len(args) < 4 else int(args[3])
    threads = []
    for i in range(int(args[2])):
        threads.append(Thread(target=thread_main, args=(iters, pages, i)))
        threads[-1].start()
    return threads

def remove_files(pages, count):
    for page in pages:
        remove(join_page_name(*page))

def main(args):
    global errors
    pages = [
                ("eax.me", "/llvm"),
                ("ejudge.cs.hse.ru", "/"),
                ("en.cppreference.com", "/w/cpp/utility/hash"),
                ("www.consultant.ru", "/about"),
            ]
    if len(args) < 3:
        print('Usage: {} port thread_count [iterations = 1]'.format(args[0]))
        return 1
    for_all(get_original, pages)

    get_via_proxy.port = args[1]
    threads = run_threads(pages, args)
    for t in threads: t.join()
    if errors == 0:
        remove_files(pages, len(threads))
    return 0

if __name__ == '__main__':
    exit(main(argv))
