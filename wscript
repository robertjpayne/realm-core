#! /usr/bin/env python
# encoding: utf-8

top = '.'
out = 'out'

def options(opt):
        opt.load('xcode')
        opt.load('compiler_c compiler_cxx')
        opt.add_option("-d", "--debug", action="store_true", default=False, dest="DEBUG", help="Turn on debug flag")
        opt.add_option("-g", "--no-optimize", action="store_false", default=True, dest="OPTIMIZE", help="Turn off optimization (e.g. when using debugger)")
        opt.add_option("--coverage", action="store_true", default=False, dest="DOCOV", help="Enable for coverage testing tool")

def configure(conf):
        conf.load('compiler_c compiler_cxx')
        conf.env.CXXFLAGS = ['-g', '-std=c++14', '-pthread', '-Wall', '-Wextra', '-pedantic', '-Wundef', '-Wshadow']
        conf.env.CXXFLAGS.append('-DREALM_HAVE_CONFIG')
        if conf.options.DEBUG:
                conf.env.CXXFLAGS.extend(['-fno-elide-constructors', '-DREALM_DEBUG'])
        else:
                conf.env.CXXFLAGS.append('-DNDEBUG')
        if conf.options.OPTIMIZE:
                conf.env.CXXFLAGS.append('-O3')
        if conf.env.CXX_NAME == 'clang':
                conf.env.CXXFLAGS.extend(['-Wunreachable-code', '-Wshorten-64-to-32', '-Wold-style-cast', '-Wconditional-uninitialized', '-Wextra-semi'])
        if conf.env.DEST_OS == 'darwin':
                conf.env.CXXFLAGS.extend(['-mmacosx-version-min=10.8', '-stdlib=libc++', '-Wno-nested-anon-types'])
        conf.recurse('src/realm')
        conf.recurse('test')

def build(ctx):
        ctx.recurse('src/realm')
        ctx.recurse('test')

