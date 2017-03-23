#!/usr/bin/env python
import os
import random
from subprocess import call

if __name__ == '__main__':
    params = ["./cmake-build-release/procgen",
              "-d", '500', '500',
              '-i', "{}".format(random.randint(1000, 10000)),
              '-p', "{}".format(random.randint(20, 130)),
              '-g', "{}".format(random.uniform(.77, .98)),
              '-a', "{}".format(random.uniform(.003, .065))
              ]
    print("{}".format(params))
    call(params)
