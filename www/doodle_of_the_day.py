#!/usr/bin/env python
import os
import glob
import random
from subprocess import call
import argparse
import json
from operator import itemgetter
from jinja2 import *

def parse_logfile(logfolder):

    globpath = os.path.join(logfolder, '*.png')
    print (globpath)
    images = glob.iglob(globpath)
    images = [(os.path.basename(i), os.path.getctime(i)) for i in images]
    images.sort(key=itemgetter(1),reverse=True)

    template = Template( open("of_the_day.html").read() )
    handle = open(os.path.join(logfolder,"spiral_day.html"), "w")
    handle.write( template.render(images=images[:16]) )
    print(images[:16])

if __name__ == '__main__':
    parse = argparse.ArgumentParser()
    parse.add_argument("--dst", help="destination for the new doodle")
    parse.add_argument("--bak", help="backup directory for old doodle")
    args = parse.parse_args();

    params = ["../cmake-build-release/procgen",
              "-d", '200', '200',
              '-i', "{}".format(random.randint(1000, 10000)),
              '-p', "{}".format(random.randint(20, 130)),
              '-g', "{}".format(random.uniform(.77, .98)),
              '-a', "{}".format(random.uniform(.003, .065))
              ]
    print("{}".format(params))
    call(params)

    if os.path.exists(args.dst) and args.bak:
        bakfile = os.path.join(args.bak, "bak_%s.png")
        i = 0
        while os.path.exists(bakfile % i):
            i += 1
        os.rename(args.dst, bakfile % i)

    newest = max(glob.iglob('*.png'), key=os.path.getctime)
    os.rename(newest, args.dst)
    parse_logfile(args.bak)
