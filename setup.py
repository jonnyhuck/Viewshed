#!/usr/bin/env python

"""
setup.py file for GDAL Viewshed
"""

from distutils.core import setup, Extension

# This bit added by JJH to import GDAl framework before compiling
import os
os.environ['LDFLAGS'] = '-framework GDAL'

viewshed_module = Extension('_viewshed',
                           sources=['viewshed_wrap.cxx', 'viewshed.cpp'],
                           )

setup (name = 'viewshed',
       version = '0.1',
       author      = "jonnyhuck",
       description = """SWIG Python interface to GDAL Viewshed""",
       ext_modules = [viewshed_module],
       py_modules = ["viewshed"],
       )