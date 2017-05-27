#! /usr/bin/python3
# Copyright 2017, Pavel Korozevtsev.

from os import remove
from sys import argv
from threading import Thread 
import subprocess as sp

def join_page_name(host, page):
    return host + '_' + page

def join_page_name_proxy(host, page, thread_id):
    return join_page_name(host, page) + '_' + str(thread_id)

def join_url(host, page):
    return host + '/' + page

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

def check(page, thread_id):
    fa = join_page_name(*page)
    fb = join_page_name_proxy(*page, thread_id)
    if open(fa).read() != open(fb).read():
        print(fa, '!=', fb)

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
    for i in range(count):
        for page in pages:
            remove(join_page_name_proxy(*page, i))
    for page in pages:
        remove(join_page_name(*page))

def main(args):
    pages = [("eax.me", "autotools"),
             ("eax.me", "llvm"),
             ("eax.me", "lxc"),
             ("eax.me", "gdb")]
    if len(args) < 3:
        print('Usage: {} port thread_count [iterations = 1]'.format(args[0]))
        return 1
    for_all(get_original, pages)

    get_via_proxy.port = args[1]
    threads = run_threads(pages, args)
    for t in threads: t.join()
    remove_files(pages, len(threads))
    return 0

if __name__ == '__main__':
    exit(main(argv))
