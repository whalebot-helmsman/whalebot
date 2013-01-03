#!/bin/bash

svn export http://google-url.googlecode.com/svn/trunk/ googleurl --force
cd googleurl/base/
patch ./basictypes.h < ../../basictypes.h.patch
patch ./logging.h < ../../logging.h.patch
patch ./string16.h < ../../string16.h.patch
patch ./string16.cc < ../../string16.cc.patch
