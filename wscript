#! /usr/bin/env python
# encoding: utf-8

import os, platform, commands
from subprocess import call
from subprocess import check_output

top = '.'
out = 'out'

arch = platform.architecture()[0]
platf = platform.platform(terse=True)

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
                max_bpnode_size = os.getenv('REALM_BP_NODE_SIZE_DEBUG', 1000)
        else:
                conf.env.CXXFLAGS.append('-DNDEBUG')
                max_bpnode_size = os.getenv('REALM_BP_NODE_SIZE', 1000)
        if conf.options.OPTIMIZE:
                conf.env.CXXFLAGS.append('-O3')
        realm_version = os.getenv('REALM_VERSION', check_output(['git describe'], shell=True))
        print("Platform     : %s"%platf)
        print("Architecture : %s"%arch)
        print("Realm version: %s"%realm_version)
        conf.define('REALM_VERSION', realm_version);
        conf.define('REALM_INSTALL_PREFIX', conf.env.PREFIX)
        conf.define('REALM_INSTALL_EXEC_PREFIX', conf.env.PREFIX)
        conf.define('REALM_INSTALL_INCLUDEDIR', conf.env.PREFIX+'/include')
        conf.define('REALM_INSTALL_BINDIR', conf.env.BINDIR)
        conf.define('REALM_INSTALL_LIBDIR', conf.env.LIBDIR)
        conf.define('REALM_INSTALL_LIBEXECDIR', conf.env.PREFIX+'/libexec')
        conf.define('REALM_MAX_BPNODE_SIZE', max_bpnode_size);
        conf.define('REALM_ENABLE_ALLOC_SET_ZERO', 0)
        conf.define('REALM_ENABLE_ENCRYPTION', 0)
        conf.define('REALM_ENABLE_ASSERTIONS', 0)
        conf.define('REALM_ENABLE_MEMDEBUG', 0)
        conf.write_config_header('src/realm/util/config.h')

def build(ctx):
        ctx.recurse('src/realm')
        ctx.recurse('test')

