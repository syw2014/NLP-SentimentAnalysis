#!/usr/bin/env python
# -*- encoding: utf8 -*-
#
# Author: Jerry Shi
# Mail: jerryshi0110@gmail.com
# Created Time: 2016-04-18 17:50:00
# Python version 2.7.5
# ------------------------------------

ifs = open('samples.pos','r')
ofs = open('samples.pos_50', 'a')
cnt = 0;
while cnt < 50:
    line = ifs.readline()
    ofs.write(line)
    cnt += 1;
ifs.close()
ofs.close()
