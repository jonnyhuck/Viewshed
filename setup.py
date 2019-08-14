#!/usr/bin/env python

"""
setup.py file for GDAL Viewshed
"""

from distutils.core import setup, Extension

# This bit added by JJH to import GDAl framework before compiling
# import os
# os.environ['LDFLAGS'] = '-framework GDAL'

viewshed_module = Extension('_viewshed',
                           sources=['viewshed_wrap.cxx', 'viewshed.cpp'],
                           extra_link_args=['-stdlib=libc++'],

                           # THIS STUFF FOR FRAMEWORK VERSION (one or the other?)
                           # extra_compile_args = ["-std=c++11", "-F/Library/Frameworks/","-framework", "GDAL", "-O3"],
                           # extra_compile_args = ["-stdlib=libc++", "-std=c++11", "-O3"],

                           # THIS STUFF FOR BREW VERSION
                           extra_compile_args = ["-L/usr/local/Cellar/gdal/2.4.1_1/lib", "-I/usr/local/Cellar/gdal/2.4.1_1/include", "-lgdal", "-std=c++11", "-O3", "-v"],
                           )

setup (name = 'viewshed',
       version = '0.1',
       author      = "jonnyhuck",
       description = """SWIG Python interface to GDAL Viewshed""",
       ext_modules = [viewshed_module],
       py_modules = ["viewshed"],
       )
