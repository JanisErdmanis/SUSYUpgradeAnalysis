import copy
import os
import sys
import re
import optparse
import shutil
import hashlib
import string
import tempfile
import threading
import time
import traceback
import urllib
import fnmatch
import glob

# Local imports
from utils import (get_field, set_field, shell_exec, mkdir_p, print_except_message,
                   ask_question, format_block, RCError, ask_question)
from workarea import WorkArea
from package import PackageList
from externals import ExternalPackage
from utils import ShellInitSh
from svninfo import SvnInfo

# Command name to function mapping
def get_standard_cmd_map():
    """For the standard commands in this module, returns a dictionary
    of command name to function"""
    std_cmd_map = {
        "check_dep": rc_check_dep,
        "clean": rc_clean,
        "compile": rc_compile,
        "compile_pkg": rc_compile_pkg,
        "download_file": rc_download_file,
        "du": rc_du,
        "external_compile": rc_external_compile,
        "external_link": rc_external_link,
        "find_packages": rc_find_packages,
        "get_cxxflags": rc_get_cxxflags,
        "get_ldflags": rc_get_ldflags,
        "grid_submit": rc_grid_submit,
        "make_bin_area": rc_make_bin_area,
        "make_doxygen": rc_make_doxygen,
        "make_par": rc_make_par,
        "root": rc_root,
        "exec": rc_exec,
        "set_release": rc_set_release,
        "status": rc_status,
        "strip": rc_strip,
        "tag_log": rc_tag_log,
        "tag_package": rc_tag_package,
        "test_cc": rc_test_cc,
        "test_ut": rc_test_ut,
        "version": rc_version
    }
    return std_cmd_map

def get_internal_cmd_map():
    """For the internal RootCore commands in this module, returns a
    dictionary of command name to function"""
    int_cmd_map = {
        "base_setup": rc_base_setup,
        "check_dep_cc": rc_check_dep_cc,
        "external_download": rc_external_download,
        "genreflex": rc_genreflex,
        "package_info": rc_package_info,
        "package_list": rc_package_list,
        "ln": rc_ln,
        "ln_rc": rc_ln_rc,
        "make_dep": rc_make_dep,
        "make_preamble": rc_make_preamble,
        "manage_pkg": rc_manage_pkg,
        "manage_all": rc_manage_all,
        "postcompile_pkg": rc_postcompile_pkg,
        "prep_load_packages": rc_prep_load_packages,
        "rel_path": rc_rel_path,
        "rootcint": rc_rootcint,
        "svn_retry": rc_svn_retry
    }
    return int_cmd_map

def get_script_cmd_map(workarea):
    """Finds the RootCore command scripts and returns a dictionary of
    command name to script file path"""
    p = os.path
    script_cmd_map = dict()
    script_dir = p.join (workarea.dir(), "scripts")
    for file in os.listdir (script_dir):
        file_path = p.join (script_dir, file)
        name, extension = p.splitext(file)
        if (p.isfile(file_path) and extension in (".py", ".sh") and
            os.access (file_path, os.X_OK)):
            script_cmd_map[name] = file_path
    return script_cmd_map


def parse_release_files (files, wa=None):
    """effects: parse the release file for the packages requested
    returns: a dictionary with the SvnInfo objects"""
    result = {}
    for file in files:
        # TODO: make this better using 'with'
        #for line in string.split (open (file, "r").read(), "\n"):
        for line in open (file, "r").read().split("\n"):
            line = line.strip()
            #line = string.strip (line)
            if line != "" and line[0] != "#":
                svninfo = SvnInfo.make_url (line)
                if wa and wa.hasPackages():
                    pkg = wa.packages().getPkg (svninfo.name())
                    if pkg:
                        svninfo = svninfo.useDir (pkg.srcdir())
                        pass
                    pass
                result[svninfo.name()] = svninfo
                pass
            pass
        pass
    return result

def manage_package (name, action, release, wa, fromGroup):
    """effects: apply the management action to the specified package"""

    pkg = None
    if wa and wa.hasPackages():
        pkg = wa.packages().getPkg (name)
        pass

    if name in release:
        svninfo = release[name]
        pass
    elif pkg:
        svninfo = pkg.svninfo()
        pass
    elif not fromGroup:
        raise Exception ("don't know how to manage unknown package: " + name)
    else:
        return

    if action != "checkout" and action != "update" and action != "build":
        raise RCError ("unknown action: " + action)

    # do something intelligible for packages in the release
    if action != "checkout" and pkg and pkg.release() != 0:
        if not fromGroup:
            raise RCError ("can't manipulate release package " + name +
                           "\ncheck it out with\n  rc checkout_pkg " + name +
                           "\n  rc find_packages")
        if svninfo.tag() == pkg.svninfo().tag():
            return
        if action == "update":
            raise RCError ("can not update release package " + name +
                           "\ncheck it out with\n  rc checkout_pkg " + name +
                           "\n  rc find_packages")
        action = "checkout"
        pass

    # do something for non-existant packages
    if action != "checkout" and not pkg:
        if action == "build":
            action = "checkout"
            pass
        elif not fromGroup:
            raise RCError ("can not update non-existant package " + name +
                           "\ntry checking it out instead")
        else:
            return
        pass

    if action == "checkout":
        svninfo = svninfo.useDir (os.path.join (os.getcwd(), svninfo.name()))
        svninfo.rc_checkout ()
        pass
    else:
        svninfo = svninfo.useDir (pkg.srcdir())
        svninfo.rc_update ()
        pass
    pass


###############################################################################
###############################################################################
###############################################################################

def rc_package_info (argv):
    parser = optparse.OptionParser (usage="usage: %prog [options] package field")
    parser.add_option ("--optional", action="store_true",
                       help="whether missing packages are Ok")
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 2:
        parser.error ("incorrect number of arguments")
        pass
    args.package = extra_args[0]
    args.field = extra_args[1]

    wa = WorkArea ()
    pkg = wa.packages().getPkg (args.package)
    if args.field == "known":
        print "1" if pkg else "0"
        return

    if not pkg:
        if args.optional:
            return
        raise RCError ("unknown package: " + package)
    if args.field == "objdir":
        print pkg.objdir()
        pass
    elif args.field == "srcdir":
        print pkg.srcdir()
        pass
    elif args.field == "dep":
        print " ".join (pkg.dep())
        pass
    elif args.field == "release":
        print pkg.release()
        pass
    else:
        raise RCError ("unknown field: " + args.field)
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_ln (argv):
    parser = optparse.OptionParser (usage="usage: %prog [options] source target")
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 2:
        parser.error ("incorrect number of arguments")
        pass
    args.source = extra_args[0]
    args.target = extra_args[1]

    wa = WorkArea ()
    wa.lnRel (args.source, args.target)
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_find_packages (argv):
    parser = optparse.OptionParser()
    WorkArea.init_release_options (parser, True)
    parser.add_option ("--allow-miss", dest="allowMiss",
                       default=False, action="store_true")
    parser.add_option ("--allow-nesting", dest="allowNesting",
                       default=False, action="store_true")
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 0:
        parser.error ("incorrect number of arguments")
        pass

    wa = WorkArea ()
    wa.m_area = os.getcwd()
    wa.find_packages (args.release, args.allowMiss, args.allowNesting)
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_manage_pkg (argv):
    parser = optparse.OptionParser (usage="usage: %prog [options] action package")
    parser.add_option ("--list", dest="releases", default=[], action='append',
                       help="include the packages from the given release list")
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 2:
        parser.error ("incorrect number of arguments")
        pass
    args.action = extra_args[0]
    args.package = extra_args[1]

    wa = WorkArea ()

    release = parse_release_files (args.releases)
    svninfo = SvnInfo.make_url (args.package, release=release, wa=wa)
    release[svninfo.name()] = svninfo

    manage_package (svninfo.name(), args.action, release, wa, False)
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_manage_all (argv):
    parser = optparse.OptionParser (usage="usage: %prog [options] action package")
    parser.add_option ("--list", dest="releases", default=[], action='append',
                       help="include the packages from the given release list")
    parser.add_option ("--concurrent", action='store_true',
                       help="perform all actions concurrently")
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 1:
        parser.error ("incorrect number of arguments")
        pass
    args.action = extra_args[0]

    wa = WorkArea ()
    release = parse_release_files (args.releases)

    names = copy.deepcopy (release)
    if len (args.releases) == 0 or args.action == "build":
        for pkg in wa.packages():
            names[pkg.name()] = pkg
            pass
        pass

    class ManageThread (threading.Thread):
        def __init__ (self, name, action, release, wa):
            threading.Thread.__init__ (self)
            self.m_name = name
            self.m_action = action
            self.m_wa = wa
            self.m_release = release
            self.e = None
            pass
        def run (self):
            try:
                manage_package (self.m_name, self.m_action, self.m_release,
                                self.m_wa, True)
            except Exception as e:
                self.e = e
                pass
            pass
        pass

    if not args.concurrent:
        for name in names.keys():
            manage_package (name, args.action, release, wa, True)
            pass
        pass
    else:
        fail = []
        threads = []
        for name in names.keys():
            t = ManageThread (name, args.action, release, wa)
            t.start ()
            threads.append (t)
            t.join (1)
            pass
        for t in threads:
            t.join ()
            if t.e:
                fail.append (t.name)
                pass
            pass
        if len (fail > 0):
            for name in fail:
                print "failed package: " + name
                pass
            sys.exit (1)
            pass
        pass
    pass

###############################################################################
###############################################################################
###############################################################################

# TODO: needs documentation
def rc_test_cc (argv):
    parser = optparse.OptionParser (usage="usage: %prog [options] action")
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 1:
        parser.error ("incorrect number of arguments")
        pass
    args.action = extra_args[0]

    wa = WorkArea ()

    # TODO: supported actions need to be part of the argument parsing
    if args.action == "compile":
        nactions = 1
    elif args.action == "linklib":
        nactions = 2
    elif args.action == "link":
        nactions = 3
    elif args.action == "run":
        nactions = 4
    else:
        raise Exception ("unknown action: " + args.action)

    clear_dir = True
    ROOTCORETEST_DIR = os.getenv ("ROOTCORETEST_DIR")
    if ROOTCORETEST_DIR:
        DIR = ROOTCORETEST_DIR
        clear_dir = False
        pass
    else:
        DIR = os.getenv ("TMPDIR")
        if not DIR:
            DIR = "/tmp"
            pass
        DIR = os.path.join (DIR, "RootCoreTest." + str (os.getpid()))
        pass

    root_conf = wa.root_conf()

    packages = PackageList (wa)
    rawdep = os.getenv ("ROOTCORETEST_DEP")
    if rawdep:
        #for name in string.split (rawdep):
        for name in rawdep.split():
            pkg = wa.packages().getPkg(name)
            if not pkg:
                raise Exception ("unknown dependency " + name)
            packages.addPkg (pkg)
            pass
        pass
    packages = packages.dep()

    if os.path.exists (DIR):
        shutil.rmtree (DIR)
        pass
    mkdir_p (DIR)

    with open (DIR + "/test.cxx", "w") as file:
        # NOTE: Python waits for EOF here!
        file.write (sys.stdin.read())
        pass

    with open (DIR+"/log", "w") as log:
        with open (DIR+"/err", "w") as err:
            if nactions >= 1:
                log.write ("building object file\n")
                cmd = [get_field (root_conf, "CXX")]
                cmd += packages.getCxxFlags().split()
                #cmd += string.split (packages.getCxxFlags())
                flags = os.getenv ("ROOTCORETEST_CXXFLAGS")
                if flags:
                    cmd += flags.split()
                    #cmd += string.split (flags)
                    pass
                cmd += ["-c", DIR + "/test.cxx", "-o", DIR + "/test.o"]
                rc = shell_exec (cmd, stdout=log, stderr=err, returnRC=True)
                pass

            if nactions == 2 and rc == 0:
                log.write ("building library file\n")
                cmd = [get_field (root_conf, "LD")]
                cmd += packages.getLibFlags ("test.so").split()
                #cmd += string.split (packages.getLibFlags ("test.so"))
                flags = os.getenv ("ROOTCORETEST_LDFLAGS")
                if flags:
                    cmd += flags.split()
                    #cmd += string.split (flags)
                    pass
                cmd += ["test.o", "-o", "test.so"]
                rc = shell_exec (cmd, stdout=log, stderr=err, returnRC=True)
                pass

            if nactions >= 3 and rc == 0:
                log.write ("building binary file\n")
                cmd = [wa.getLd()]
                cmd += packages.getBinFlags().split()
                #cmd += string.split (packages.getBinFlags())
                cmd += ["test.o", "-o", "test"]
                rc = shell_exec (cmd, stdout=log, stderr=err, returnRC=True)
                pass

            if nactions >= 4 and rc == 0:
                log.write ("building binary file\n")
                cmd = ["test"]
                rc = shell_exec (cmd, stdout=log, stderr=err, returnRC=True)
                pass
            pass
        pass

    if clear_dir:
        shutil.rmtree (DIR)
        pass

    sys.exit (rc)
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_compile (argv):
    parser = optparse.OptionParser()
    parser.add_option ("--single-thread", dest="single_thread", action="store_true",
                       help="whether to run a single threaded compilation")
    parser.add_option ("--continue", dest="cont", action="store_true",
                       help="whether to continue on error")
    parser.add_option ("--log-files", dest="log_files", action="store_true",
                       help="whether to create log files")
    parser.add_option ("--no-load-packages", dest="noLoadPackages", action="store_true",
                       help="disable loading packages at the end")
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) > 0:
        return rc_compile_pkg (argv)

    if args.single_thread :
        os.environ ["ROOTCORE_NCPUS"] = "1"
        pass

    wa = WorkArea ()

    wa.root_conf ()

    for dir in ["include", "python", "user_scripts", "data",
                os.path.join ("bin", wa.arch()), os.path.join ("lib", wa.arch())]:
        path = os.path.join (wa.bin(), dir)
        if os.path.exists (path):
            shutil.rmtree (path)
            pass
        mkdir_p (path)
        pass
    wa.ln_rc ()
    mkdir_p (os.path.join (wa.bin(), "RootCore"))
    mkdir_p (os.path.join (wa.bin(), "download"))

    source  = '#ifndef ROOTCORE_PACKAGES_H\n'
    source += '#define ROOTCORE_PACKAGES_H\n'
    source += '\n'
    source += '// This file contains one define statement for each package detected by\n'
    source += '// RootCore.  It is meant to allow to write package A in a form that\n'
    source += '// it uses the services of package B when available, but otherwise\n'
    source += '// skips it without failing.  For this to work properly you need to list\n'
    source += '// package B in the PACKAGE_TRYDEP of package A.\n'
    source += '\n'
    for pkg in wa.packages():
        source += "#define ROOTCORE_PACKAGE_" + pkg.name() + "\n"
        pass
    source += '\n'
    source += '#endif\n'

    file = os.path.join (wa.bin(), "RootCore", "Packages.h")
    if not os.path.isfile (file) or open (file).read() != source:
        mkdir_p (os.path.dirname (file))
        with open (file, "w") as f:
            f.write (source)
            pass
        pass
    wa.lnRel (os.path.join (wa.bin(), "RootCore"),
              os.path.join (wa.bin(), "include", "RootCore"))

    for pkg in wa.packages():
        pkg.linkPreconfigure()
        pass

    wa.compileCore (wa.packages(), args.log_files, args.cont)

    if not args.noLoadPackages:
        path = os.path.join (wa.bin(), "load_packages")
        success = path + "_success"
        if os.path.exists (success):
            os.remove (success)
            pass
        if (shell_exec(["rc", "root", "-l", "-b", "-q"],
                       returnRC=True) != 0 or
            not os.path.isfile(success)):
            print "failed to test load libraries"
            sys.exit (1)
            pass
        pass
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_grid_submit (argv):
    parser = optparse.OptionParser (usage="usage: %prog [options] destination")
    parser.add_option ("--nobuild", dest="noBuild", default=False, action="store_true")
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 1:
        parser.error ("incorrect number of arguments")
        pass
    DIR=extra_args[0]
    if not os.path.isabs (DIR):
        DIR = os.path.join (os.getcwd(), DIR)
        pass

    wa = WorkArea ()
    wa_new = WorkArea (RootCoreBin = os.path.join (DIR, "RootCoreBin"),
                       RootCoreDir = os.path.join (DIR, "RootCore"))
    wa_new.m_area = DIR
    wa_new.m_packages = PackageList (wa_new)

    for pkg in wa.packages():
        mypkg = copy.copy (pkg)
        mypkg.m_wa = wa_new
        grid_release_prefix = os.getenv ("ROOTCORE_GRID_RELEASE_PREFIX")
        if not grid_release_prefix or grid_release_prefix == "":
            grid_release_prefix = "/cvmfs"
            pass
        if (mypkg.name() == "RootCore" or
            mypkg.srcdir().find (grid_release_prefix) != 0):
            mypkg.set_srcdir (os.path.join (DIR, pkg.name()))
            pass
        if (mypkg.name() == "RootCore" or
            mypkg.m_outdir.find (grid_release_prefix) != 0):
            mypkg.m_outdir = wa_new.obj()
            mypkg.m_recompile = True
            pass
        wa_new.packages().addPkg (mypkg)
        pass

    for pkg in wa.packages():
        mypkg = wa_new.packages().getPkg (pkg.name())
        pkg.copySource (mypkg)
        pass

    local_setup = os.path.join (wa_new.packages().getPkg ("RootCore").srcdir(),
                               "local_setup")
    if os.path.exists (local_setup + ".sh"):
        os.remove (local_setup + ".sh")
        pass
    if os.path.exists (local_setup + ".csh"):
        os.remove (local_setup + ".csh")
        pass

    mkdir_p (wa_new.bin())
    wa_new.write_config ()
    wa_new.writePackages (wa_new.packages())
    if args.noBuild:
        for config in os.listdir (wa.obj()):
            for file in "root_config_", "load_packages_info_":
                source = os.path.join (wa.bin(), file + config)
                target = os.path.join (wa_new.bin(), file + config)
                if os.path.isfile (source):
                    shutil.copyfile (source, target)
                    pass
                pass
            wa_new.m_arch = config
            wa_new.m_packages = wa_new.parsePackages ()
            for pkg in wa.packages():
                mypkg = wa_new.packages().getPkg (pkg.name())
                pkg.copyObject (mypkg, config)
                pass
            pass
        pass
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_make_bin_area (argv):
    parser = optparse.OptionParser (usage="usage: %prog [options] destination")
    parser.add_option ("--nobuild", dest="noBuild", default=False,
                      action="store_true")
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 1:
        parser.error ("incorrect number of arguments")
        pass
    DIR=extra_args[0]

    if not os.path.isabs (DIR):
        DIR = os.path.join (os.getcwd(), DIR)
        pass

    wa = WorkArea ()
    wa_new = WorkArea (RootCoreBin = DIR, RootCoreDir = wa.dir())
    wa_new.m_area = DIR
    wa_new.m_packages = PackageList (wa_new)

    for pkg in wa.packages():
        mypkg = copy.copy (pkg)
        mypkg.m_wa = wa_new
        mypkg.m_release = 1
        mypkg.m_recompile = False
        wa_new.packages().addPkg (mypkg)
        pass

    mkdir_p (wa_new.bin())
    wa_new.write_config ()
    wa_new.writePackages (wa_new.packages())
    set_field (wa_new.config(), "release", "--obj-release " + wa.bin())

    for pkg in wa_new.packages():
        pkg.linkPrecompile()
        pkg.linkPostcompile()
        pass

    print "you can setup the new area by typing: "
    print "  source " + os.path.join (DIR, "local_setup.sh")
    print "don't forget to setup root etc."
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_test_ut (argv):
    parser = optparse.OptionParser()
    parser.add_option ("--fast", action="store_true",
                       help="only run the fast checks")
    parser.add_option ("--list", dest="list", action="store_true",
                       help="make a list of all the tests")
    parser.add_option ("--long-list", dest="longList", action="store_true",
                       help="make a list of all the tests")
    parser.add_option ("--slow", action="store_true",
                       help="also run the slow checks")
    parser.add_option ("--compile", action="store_true",
                       help="run tests as part of compilation")
    parser.add_option ("--auto", action="store_true",
                       help="run the checks from the command line," +
                       " as if they were detected automatically")
    parser.add_option ("--no-delete", dest="noDelete", action="store_true",
                       help="don't delete the work directories")
    parser.add_option ("--package", dest="package",
                       action="store",help="limit to the specified package")
    parser.add_option ("--recursive", action="store_true",
                       help="run only the checks that are designated for "+
                       "rerursive running")
    (args, extra_args) = parser.parse_args(argv)
    args.tests = extra_args

    if args.compile :
        args.auto = True
        args.fast = True
        pass

    if len (args.tests) == 0 :
        args.auto = True
        pass

    wa = WorkArea()

    # List available tests, but don't run them
    if args.list or args.longList:
        for test in wa.ut_test_list(args.package,args.recursive):
            if args.longList:
                p = os.path
                pkgname = p.basename (p.dirname (p.dirname (test)))
                print pkgname + " " + test
                pass
            else:
                print test
                pass
            pass
        return

    # Configure auto/fast/slow options via the environment variables
    newEnv = {"ROOTCORE_AUTO_UT":None,
              "ROOTCORE_FAST_UT":None,
              "ROOTCORE_SLOW_UT":None,
              "XAOD_ACCESSTRACER_FRACTION":"0"}
    if len (args.tests) == 0 or args.auto:
        newEnv["ROOTCORE_AUTO_UT"] = "1"
        if args.fast:
            newEnv["ROOTCORE_FAST_UT"] = "1"
            pass
        elif args.slow:
            newEnv["ROOTCORE_SLOW_UT"] = "1"
            pass
        pass

    # If user provides tests, only run those
    if len (args.tests) > 0:
        tests = args.tests
        pass
    else:
        tests = wa.ut_test_list(args.package,args.recursive)
        pass

    count = 0
    failed = []
    basedir = os.getcwd()
    # Unit tests need to be named with prefix "ut_"
    name_expr = re.compile ("^((ut|gt|it|pt)_[^.]*)(.*)$")
    for test in tests:
        try:
            path = os.path
            match = name_expr.match (path.basename (test))
            if not match:
                raise RCError ("don't know how to handle test " + test)
            if match.group(2) == "gt" or match.group(2) == "pt" or not args.compile :
                count = count + 1
                print "running test " + test
                # Grab package name assuming directory structure:
                # "somepath/PACKAGE/somedir/sometest"
                pkgname = path.basename (path.dirname (path.dirname (test)))
                pkg = wa.packages().getPkg (pkgname)
                if not pkg:
                    raise RCError ("could not determine package for test " + test)
                # Tests are run in a temporary work directory
                dir = path.join (wa.bin(), "obj", wa.arch(), pkg.name(), "run",
                                 match.group(1))
                # Clean the work directory out if it already exists
                if path.isdir (dir):
                    shutil.rmtree (dir)
                    pass
                mkdir_p (dir)
                # Execute the test
                command = [test]
                if match.group(2) == "gt" or match.group(2) == "it" :
                    excludes = []
                    if args.fast :
                        excludes += ["SLOW_*.*:*.SLOW_*"]
                        pass
                    if args.auto :
                        excludes += ["MANUAL_*.*:*.MANUAL_*"]
                        pass
                    if len (excludes) > 0 :
                        command += [ "--gtest_filter=-" + string.join (excludes, ":") ]
                        pass
                    pass
                if match.group(3) == ".C" :
                    command = ["rc", "root", "-l", "-b", "-q", test]
                    pass
                output=None
                if args.compile :
                    outputName = dir + ".log_file"
                    output = open (outputName, "w")
                    #print "writing output to " + outputName
                    pass
                shell_exec (command, workDir=dir, noReturn=True,
                            newEnv=newEnv, stdout=output, stderr=output)
                if not args.noDelete:
                    shutil.rmtree (dir)
                    pass
                pass
            pass
        # Catch all exceptions, mark as failed, and move on
        except Exception:
            if output :
                print open (outputName, "r").read()
                pass
            print "test failed:"
            print_except_message()
            print "unit test result in directory:" + dir
            failed.append (test)
            pass
        pass

    # Print a summary of the unit tests
    print str(count - len(failed)) + " tests out of " + str(count) + " passed"
    if len (failed) > 0:
        print "failed tests:"
        for test in failed:
            print test
            pass
        sys.exit (1)
        pass
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_ln_rc (argv):
    parser = optparse.OptionParser()
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 0:
        parser.error ("incorrect number of arguments")
        pass

    wa = WorkArea ()
    wa.ln_rc ()
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_base_setup (argv):
    parser = optparse.OptionParser()
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 0:
        parser.error ("incorrect number of arguments")
        pass

    wa = WorkArea ()
    wa.write_config ()
    wa.ln_rc ()
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_make_preamble (argv):
    parser = optparse.OptionParser()
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 1:
        parser.error ("incorrect number of arguments")
        pass
    args.package = extra_args[0]

    wa = WorkArea ()
    pkg = wa.packages().getPkg (args.package)
    if not pkg:
        raise Exception ("unknown package: " + package)

    print pkg.makePreamble ()
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_package_list (argv):
    parser = optparse.OptionParser()
    parser.add_option ("--local-obj", dest="localObj", action='store_true',
                       help="only display packages that get recompiled")
    parser.add_option ("--all", action='store_true',
                       help="include RootCore in the list")
    parser.add_option ("--list", dest="releases", default=[], action='append',
                       help="include the packages from the given release list")
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 0:
        parser.error ("incorrect number of arguments")
        pass

    wa = WorkArea ()
    if (len (args.releases) == 0 or
        os.path.isfile (os.path.join (wa.bin(), "packages"))):
        packages = wa.parsePackages ()
        pass
    else:
        packages = PackageList (wa)
        pass

    for release in args.releases:
        packages.mergePackages (wa.parseRelease (release))
        pass

    for pkg in packages:
        if ((args.all or pkg.name() != "RootCore") and
            (not args.localObj or pkg.recompile())):
            print pkg.name()
            pass
        pass
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_prep_load_packages (argv):
    parser = optparse.OptionParser()
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 0:
        parser.error ("incorrect number of arguments")
        pass

    wa = WorkArea ()

    content = ""
    #for flag in string.split (wa.packages().getCxxFlags (user=True)):
    for flag in wa.packages().getCxxFlags (user=True).split():
        if flag.find ("-I") == 0:
            content += "include = " + wa.relPath (flag[2:], wa.bin()) + "\n"
            pass
        elif flag.find ("-D") == 0:
            content += "semiinclude = " + flag + "\n"
            pass
        else:
            content += "cxxflags = " + flag + "\n"
            pass
        pass
    for pkg in wa.packages():
        if not pkg.nocc():
            content += "lib = lib" + pkg.name() + "\n"
            if pkg.useReflex() :
                content += "reflexlib = lib" + pkg.name() + "_Reflex\n"
                pass
            pass
        else:
            for preload in pkg.preload():
                content += "lib = lib" + preload + "\n"
                pass
            pass
        pass
    for pkg in wa.packages():
        if pkg.name() == "xAODRootAccess":
            content += "process = xAOD::Init().ignore()\n"
            pass
        pass

    path = os.path.join (wa.bin(), "load_packages" + "_info_" + wa.arch())
    if not os.path.isfile (path) or open (path, "r").read() != content:
        with open (path, "w") as f:
            f.write (content)
        pass
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_clean (argv):
    parser = optparse.OptionParser()
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 0:
        parser.error ("incorrect number of arguments")
        pass

    wa = WorkArea ()

    print "cleaning all packages"
    try:
        p = os.path
        for dir in p.join (wa.bin(), "lib"), p.join (wa.bin(), "bin"), wa.obj():
            if p.isdir (dir):
                shutil.rmtree (dir)
                pass
            pass
        for path in glob.glob (p.join (wa.bin(), "root_config_*")) :
            if p.isfile (path):
                os.remove (path)
                pass
            pass
    finally:
        wa.ln_rc()
        pass
    pass

###############################################################################
###############################################################################
###############################################################################

# TODO: this needs some documentation
def download_file (wa, base_target, base_locations, md5sum=None):
    mkdir_p (os.path.join (wa.bin(), "download"))
    target = os.path.join (wa.bin(), "download", base_target)
    locations = [target]
    dir = os.getenv ("ROOTCORE_DOWNLOAD_DIR")
    if dir and dir != "":
        destination = os.path.join (dir, base_target)
        locations += [destination]
        pass
    else:
        destination = target
        pass
    path = os.getenv ("ROOTCORE_DOWNLOAD_PATH")
    if path:
        #for dir in string.split (path, ":"):
        for dir in path.split(":"):
            if dir != "":
                locations += [os.path.join (dir, base_target)]
                pass
            pass
        pass
    locations += base_locations

    for location in locations:
        if location[0] == "/":
            if os.path.exists (location):
                if location != target:
                    wa.lnRel (location, target)
                    pass
                print target
                return
            pass
        pass

    try:
        mkdir_p (os.path.dirname (destination))
        dir = tempfile.mkdtemp (prefix=".download",
                                dir=os.path.dirname (destination))
        file = os.path.join (dir, os.path.basename (destination))
        for location in locations:
            if location[0] != "/":
                if os.path.exists (file):
                    os.remove (file)
                    pass
                try:
                    print "trying to download: " + location
                    urllib.urlretrieve (location, file)
                    pass
                except Exception as e:
                    print "  failed to download: " + str(e)
                    continue
                if md5sum:
                    hash = hashlib.md5()
                    with open (file) as f:
                        buffer = "dummy"
                        while len (buffer) > 0:
                            # TODO: hardcoded value needs explanation
                            buffer = f.read (4096)
                            hash.update (buffer)
                            pass
                    if hash.hexdigest() != md5sum:
                        raise RCError ("checksum error: md5sum=" + hash.hexdigest() + " reference=" + md5sum + " url=" + location)
                        #print s % (hash.hexdigest(), md5sum)
                if not os.path.exists (destination):
                    os.rename (file, destination)
                    pass
                if destination != target:
                    wa.lnRel (destination, target)
                    pass
                print "  success"
                print target
                return
            pass
        pass
    finally:
        if dir:
            shutil.rmtree (dir, True)
            pass
        pass
    raise RCError ("failed to find/download: " + base_target)

def rc_download_file (argv):
    usagestr = "usage: %prog [options] name location-list"
    parser = optparse.OptionParser (usage=usagestr)
    parser.add_option ("--md5sum", dest="md5sum", action="store", type="string",
                       default=None)
    (args, extra_args) = parser.parse_args (argv)
    if len (extra_args) < 2:
        parser.error ("incorrect number of arguments")
        pass
    args.target = extra_args[0]
    args.locations = extra_args[1:]

    wa = WorkArea ()

    download_file (wa, args.target, args.locations, md5sum=args.md5sum)
    pass

def rc_external_compile (argv):
    parser = optparse.OptionParser (usage="usage: %prog [options]")
    (args, extra_args) = parser.parse_args (argv)
    if len (extra_args) != 0:
        parser.error ("incorrect number of arguments")
        pass

    wa = WorkArea ()

    package = ExternalPackage (wa)
    if package.check_configuration():
        return
    print "configuring " + package.m_pkgname
    package.clear_configuration ()
    package.find_options ()
    package.read_options ()
    package.complete_locations ()

    for location in package.m_locations:
        print "  testing location: " + str (location)
        try:
            location.install_test ()
            location.write_conf ()
            return
        except Exception as e:
            print "    failed: " + str (e)
            pass
        pass

    message = "failed to find valid " + package.m_pkgname + " installation in"
    for location in package.m_locations:
        message += " " + str (location)
        pass
    raise RCError (message)

def rc_external_link (argv):
    parser = optparse.OptionParser (usage="usage: %prog [options]")
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 0:
        parser.error ("incorrect number of arguments")
        pass

    wa = WorkArea ()

    package = ExternalPackage (wa)
    location = package.read_location ()
    location.link_package ()
    pass

def rc_external_download (argv):
    # NOTE: "shell_init" is already used for a function name!
    shell_init = ShellInitSh()
    def excepthook (exctype, value, trace):
        message = str (value)
        if os.getenv ("ROOTCORE_VERBOSE") == "1":
            message += "\n\n" + " ".join (traceback.format_tb (trace))
            #message += "\n\n" + string.join (traceback.format_tb (trace))
            pass
        shell_init.abort (message)
        pass
    sys.excepthook = excepthook
    sys.stdout = shell_init

    parser = optparse.OptionParser (usage="usage: %prog [options]")
    parser.add_option ("--no-flags", dest="no_flags", action="store_true",
                       help="whether to disable setting of compiler flags")
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 0:
        parser.error ("incorrect number of arguments")
        pass

    wa = WorkArea()

    package = ExternalPackage (wa)
    package.find_options()
    package.read_options()

    shell_init.execCmd ("set -e")
    shell_init.execCmd ("set -u")

    archive = os.path.join (package.m_pkgname,
                            os.path.basename (package.m_url[0]))
    if package.m_md5sum:
        options = " --md5sum " + package.m_md5sum
        pass
    else:
        options = ""
        pass
    shell_init.execCmd ("rc download_file " + options + " " + archive + " " +
                        " ".join (package.m_url))
                       #string.join (package.m_url, " "))
    archive = os.path.join (wa.bin(), "download", archive)

    myarea = os.path.join (os.getenv ("ROOTCORECMT"), "local", "src")
    mkdir_p (myarea)
    shell_init.execCmd ("cd " + myarea)
    shell_init.execCmd ("tar xfz " + archive)
    shell_init.execCmd ("cd " + package.m_srcdir)

    if not args.no_flags:
        cc = wa.getCxx()
        if os.path.basename (cc) == "clang++" :
            cc = os.path.join (os.path.dirname (cc), "clang")
            pass
        elif os.path.basename (cc) == "g++" :
            cc = os.path.join (os.path.dirname (cc), "gcc")
            pass
        elif os.path.basename (cc) == "c++" :
            cc = os.path.join (os.path.dirname (cc), "cc")
            pass
        shell_init.setEnv ("CC", cc)
        shell_init.echo ("CC = " + os.getenv ("CC"))
        shell_init.setEnv ("CXX", wa.getCxx())
        shell_init.echo ("CXX = " + os.getenv ("CXX"))
        shell_init.setEnv ("CXXCPP", wa.getCxx() + " " +
                           package.m_rcpkg.getCxxFlags (external=True) + " -E")
        shell_init.echo ("CXXCPP = " + os.getenv ("CXXCPP"))
        cflags = package.m_rcpkg.getCxxFlags (external=True).split()
        while "-std=c++11" in cflags :
            cflags.remove ("-std=c++11")
            pass
        shell_init.setEnv ("CFLAGS", string.join (cflags, " "))
        shell_init.echo ("CFLAGS = " + os.getenv ("CFLAGS"))
        shell_init.setEnv ("CXXFLAGS", package.m_rcpkg.getCxxFlags (external=True))
        shell_init.echo ("CXXFLAGS = " + os.getenv ("CXXFLAGS"))
        shell_init.setEnv ("LDFLAGS", package.m_rcpkg.getLibFlags (external=True))
        shell_init.echo ("LDFLAGS = " + os.getenv ("LDFLAGS"))
        #shell_init.setEnv ("CFLAGS", "")
        #shell_init.setEnv ("CXXFLAGS", "")
        #shell_init.setEnv ("LDFLAGS", "")
        pass
    shell_init.setEnv ("target", package.m_target)

    shell_init.echo ("compiler flags:")
    shell_init.execCmd ("env | grep -E '(FLAGS|-D__USE_XOPEN2K8)' || true")
    shell_init.makeCmds ()
    pass

###############################################################################
###############################################################################
###############################################################################

#class Object (object):
class ErrorSummary (object):
    def __init__ (self):
        self.errors = 0
        self.warnings = 0
        pass
    pass

def check_dep_read_file (wa, file, dep):
    rcinclude = os.path.join (wa.bin(), "include")
    # TODO: improve with 'with'
    #for ref in string.split (open (file).read()):
    for ref in open (file).read().split():
        if ref[0] == '/':
            rel = wa.relPath (ref, rcinclude)
            if rel[0] != "/" and rel[0] != ".":
                base = rel.split ("/")[0]
                #base = string.split (rel, "/") [0]
                if wa.packages().getPkg (base):
                    dep[base] = "x"
                pass
            pass
        pass
    pass

def check_dep_read_pkg (wa, pkg):
    expr_dep = re.compile (".*\.d$")
    dep = {}
    if os.path.isdir (pkg.objdir()):
        for file in os.listdir (pkg.objdir()):
            if expr_dep.match (file):
                myfile = os.path.join (pkg.objdir(), file)
                check_dep_read_file (wa, myfile, dep)
            pass
        pass
    return dep

def check_dep_pkg (wa, pkg, summary):
    reldep = check_dep_read_pkg (wa, pkg)
    fulldep = pkg.dep() + ["RootCore"]
    for dep in reldep.keys():
        # NOTE: the "RootCore" check here is redundant
        #if (not dep in fulldep and dep != "RootCore" and
        #    not dep in wa.packages()):
        if not dep in fulldep and not dep in wa.packages():
            s = "ERROR: %s does not list %s as dependency" % (pkg.name(), dep)
            print s
            summary.errors += 1
            #summary.errors = summary.errors + 1
            pass
    for dep in pkg.harddep() + pkg.trydep():
        if (wa.packages().getPkg (dep) and not dep in reldep and
            os.path.exists (os.path.join (wa.bin(), "include", dep))):
            print "WARNING: " + pkg.name() + " lists " + dep + \
                  " as dependency, but doesn't use it"
            summary.warnings += 1
            #summary.warnings = summary.warnings + 1
            pass
        pass
    if not pkg.nocc() and not pkg.pedantic():
        print "WARNING: %s does not request pedantic compilation" % pkg.name()
        summary.warnings += 1
        #summary.warnings = summary.warnings + 1
        pass
    pass

def rc_check_dep (argv):
    parser = optparse.OptionParser()
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 0:
        parser.error ("incorrect number of arguments")
        pass

    wa = WorkArea()
    summary = ErrorSummary()
    #summary = Object()
    #summary.errors = 0
    #summary.warnings = 0

    rcinclude = os.path.join (wa.bin(), "include")
    for pkg in wa.packages():
        if pkg.release() == 0:
            check_dep_pkg (wa, pkg, summary)
            pass
        pass
    print "encountered %d warnings and %d error(s)" % (summary.warnings,
                                                       summary.errors)
    #print "encountered " + str(summary.warnings) + " warnings and " + str(summary.errors) + " error(s)"
    if summary.errors > 0:
        sys.exit (1)
        pass
    pass

def rc_check_dep_cc (argv):
    parser = optparse.OptionParser()
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 2:
        parser.error ("incorrect number of arguments")
        pass
    package = extra_args[0]
    file = extra_args[1]

    wa = WorkArea()
    reldep = {}
    check_dep_read_file (wa, file[:-2] + ".d", reldep)
    pkg = wa.packages().getPkg (package)
    fulldep = pkg.dep() + ["RootCore"]
    error = False
    for dep in reldep.keys():
        if not dep in fulldep and not dep in wa.packages():
            print "ERROR: " + pkg.name() + " does not list " + dep + " as dependency needed by " + os.path.basename (file)
            error = True
            pass
        pass
    if error :
        sys.exit (1)
        pass
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_rel_path (argv):
    parser = optparse.OptionParser()
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 2:
        parser.error ("incorrect number of arguments")
        pass
    args.file = extra_args[0]
    args.dir = extra_args[1]

    wa = WorkArea ()
    if not os.path.isabs (args.file):
        args.file = os.path.join (os.getcwd(), args.file)
        pass
    if not os.path.isabs (args.dir):
        args.dir = os.path.join (os.getcwd(), args.dir)
        pass
    print wa.relPath (args.file, args.dir, "/")
    return

###############################################################################
###############################################################################
###############################################################################

def rc_version (argv):
    parser = optparse.OptionParser()
    parser.add_option ("--local", dest="local", default=False,
                       action="store_true")
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 0:
        parser.error ("incorrect number of arguments")
        pass

    wa = WorkArea ()
    for pkg in wa.packages():
        if not args.local or not pkg.release() :
            pkg.svninfo().rc_version()
            pass
        pass
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_set_release (argv):
    parser = optparse.OptionParser()
    WorkArea.init_release_options (parser, True)
    parser.add_option ("--allow-miss", dest="allowMiss", default=False,
                       action="store_true")
    parser.add_option ("--allow-nesting", dest="allowNesting", default=False,
                       action="store_true")
    parser.add_option ("--no-find-packages", dest="noFindPackages",
                       default=False, action="store_true")
    parser.add_option ("--no-clean", dest="noClean", default=False,
                       action="store_true")
    (args, extra_args) = parser.parse_args (argv)
    if len (extra_args) != 0:
        parser.error ("incorrect number of arguments")
        pass

    wa = WorkArea()
    set_field (wa.config(), "release", " ".join (args.release))
    #set_field (wa.config(), "release", string.join (args.release, " "))
    if not args.noClean and os.path.exists (wa.obj()):
        shutil.rmtree (wa.obj())
        pass
    if not args.noFindPackages:
        wa.find_packages ([], args.allowMiss, args.allowNesting)
        pass
    pass

###############################################################################
###############################################################################
###############################################################################

# TODO: redesign this function to use less nesting
def rc_strip (argv):
    parser = optparse.OptionParser()
    WorkArea.init_release_options (parser, True)
    parser.add_option ("--remove-debug", dest="removeDebug", default=False,
                       action="store_true")
    (args, extra_args) = parser.parse_args(argv)
    if len (extra_args) != 0:
        parser.error ("incorrect number of arguments")
        pass

    wa = WorkArea()
    for arch in os.listdir (wa.obj()):
        for pkg in os.listdir (os.path.join (wa.obj(), arch)):
            dir = os.path.join (wa.obj(), arch, pkg)
            for sub in "obj", "run", "src", "lib/local/src":
                subdir = os.path.join (dir, sub)
                if (os.path.exists (subdir)):
                    shutil.rmtree (subdir)
                    pass
                pass
            p = os.path
            for libdir in (p.join (dir, "lib"),
                           p.join (dir, "lib", "local", "lib")):
                if args.removeDebug and p.exists (libdir):
                    for lib in os.listdir (libdir):
                        if (fnmatch.fnmatch (lib, "*.so") or
                            fnmatch.fnmatch (lib, "*.dylib")):
                            options = []
                            if not wa.isMac:
                                options += ["--strip-all"]
                                pass
                            else:
                                options += ["-x"]
                                pass
                            shell_exec (["strip"] + options +
                                        [p.join (libdir, lib)],
                                        warnFail=True, noReturn=True)
                            pass
                        if fnmatch.fnmatch (lib, "*.a"):
                            shell_exec (["strip", "-S", p.join (libdir, lib)],
                                        warnFail=True, noReturn=True)
                            pass
                        pass
                    pass
                pass
            for bindir in (p.join (dir, "bin"), p.join (dir, "test-bin"),
                           p.join (dir, "lib", "local", "bin")):
                if args.removeDebug and p.exists (bindir):
                    for bin in os.listdir (bindir):
                        shell_exec (["strip", "-S", p.join (bindir, bin)],
                                    warnFail=True, noReturn=True)
                        pass
                    pass
                pass
            pass
        pass
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_make_dep (argv):
    cmd = []
    while argv[0] != "--":
        cmd.append (argv[0])
        argv = argv[1:]
        pass

    dep_file = argv[1]
    src_file = argv[2]
    dep_list = argv[3:]

    obj_file = dep_file[0:dep_file.rfind(".")] + ".o"
    cmd += ["-MM", src_file]

    indep = False
    #for dep in string.split (shell_exec (cmd)):
    for dep in shell_exec (cmd).split():
        if indep:
            if dep != "\\":
                if not os.path.isabs (dep):
                    dep = os.path.join (os.getcwd(), dep)
                    pass
                dep_list.append (dep)
                pass
            pass
        elif dep[-1] == ":":
            indep = True
            pass
        pass

    with open (dep_file, "wt") as file:
        file.write (obj_file + " " + dep_file + " : " +
                    " ".join (dep_list) + "\n")
        pass
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_rootcint (argv):

    if len (argv) < 3:
        raise RCError ("need at least two arguments to rootcint")

    rootcint = argv[0]
    input = argv[-5]
    output = argv[-4]
    depfile = argv[-3]
    libdir = argv[-2]
    pkgname = argv[-1]
    flags=argv[1:-5]

    wa = WorkArea()

    if output[-4:] != ".cxx":
        raise RCError ("output name needs to end in .cxx")

    inputText = open (input, "r").read()

    cmd = [rootcint, "-f", output, "-c", "-p"] + flags
    pastSeparator = False
    includeDir = os.path.join (os.getenv ("ROOTCOREBIN"), "include")
    # TODO: open file using 'with'
    #for file in string.split (open (depfile, "r").read()):
    for file in open (depfile, "r").read().split():
        if file == ":":
            pastSeparator = True
            pass
        elif pastSeparator:
            myfile = wa.relPath (file, area=includeDir)
            if os.path.isabs (myfile):
                split = myfile.rfind ("/" + pkgname + "/" )
                if split != -1:
                    subfile = myfile[split+1:]
                    if os.path.exists (os.path.join (includeDir, subfile)):
                        myfile = subfile
                        pass
                    pass
                pass
            if not os.path.isabs (myfile) and inputText.find (myfile) != -1:
                cmd.append (myfile)
                pass
    cmd.append (input)
    if os.getenv ("ROOTCORE_VERBOSE") == "1":
        print " ".join (cmd)
        #print string.join (cmd, " ")
        pass
    shell_exec (cmd)

    output_lines = []
    rcdir = os.getenv ("ROOTCOREDIR")
    with open (os.path.join (rcdir, "DictPreamble.cxx")) as file_in:
        for line in file_in:
            output_lines.append (line)
            pass
        pass

    with open (output) as in_file:
        for line in in_file:
            output_lines.append (line)
            pass
        pass

    with open (output, "w") as out_file:
        for line in output_lines:
            out_file.write (line)
            pass
        pass

    file = output[:-4] + "_rdict.pcm"
    if os.path.exists (file):
        target = os.path.join (libdir, os.path.basename (file))
        if os.path.exists (target):
            os.remove (target)
            pass
        shutil.copyfile (file, target)
        pass
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_genreflex (argv):

    if len (argv) < 3:
        raise RCError ("need at least two arguments to rootcint")

    arg_source = argv[0]
    arg_xml = argv[1]
    arg_cxxfile = argv[2]
    arg_rootmap = argv[3]
    arg_lib = argv[4]
    arg_libdir = argv[5]
    arg_flags=argv[6:]

    if arg_cxxfile[-4:] != ".cxx":
        raise RCError ("output name needs to end in .cxx")

    cmd = ["genreflex", arg_source, "-s", arg_xml, "-o", arg_cxxfile] + \
          arg_flags + ["--rootmap=" + arg_rootmap, "--rootmap-lib=" + arg_lib]
    if os.getenv ("ROOTCORE_VERBOSE") == "1":
        print " ".join (cmd)
        #print string.join (cmd, " ")
    shell_exec (cmd, noReturn=True)
    file = arg_cxxfile[:-4] + "_rdict.pcm"
    if os.path.exists (file):
        target = os.path.join (arg_libdir, os.path.basename (file))
        if os.path.exists (target):
            os.remove (target)
            pass
        shutil.copyfile (file, target)
        pass
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_svn_retry (argv):
    if argv[0] in {"ls":0, "status":0, "log":0}:
        print shell_exec (["svn"] + argv, retries=2).strip()
        #print string.strip (shell_exec (["svn"] + argv, retries=2))
        pass
    else:
        shell_exec (["svn"] + argv, retries=2, noReturn=True)
        pass
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_tag_log (argv):
    parser = optparse.OptionParser()
    WorkArea.init_release_options (parser, True)
    (args, extra_args) = parser.parse_args (argv)
    if len (extra_args) != 1:
        parser.error ("incorrect number of arguments")
        pass

    #if string.find (extra_args[0], "/") == -1:
    if "/" in extra_args[0]:
        pkg = wa.packages().getPkg (url)
        if not pkg:
            raise RCError ("unknown package: " + url)
        svninfo = pkg.svninfo()
        if not url:
            raise RCError ("package " + pkg.name() +
                           " does not have an associated SVN repository")
        pass
    else:
        svninfo = SvnInfo.make_url (extra_args[0], wa=wa)
        pass

    for tag in sorted (svninfo.svn_ls_tags(), reverse=True):
        svninfo = svninfo.useTag (tag)
        print
        print tag
        print svninfo.svn_log()
        pass
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_status (argv):
    parser = optparse.OptionParser()
    WorkArea.init_release_options (parser, True)
    parser.add_option ("-u", dest="update", default=False, action="store_true")
    (args, extra_args) = parser.parse_args (argv)
    if len (extra_args) != 0:
        parser.error ("incorrect number of arguments")
        pass

    wa = WorkArea()
    for pkg in wa.packages():
        if pkg.release() != 1:
            status = pkg.svninfo().svn_status (update=args.update,
                                               allowNone=True)
            if status != "":
                print pkg.name()
                sys.stdout.write (status)
                pass
            pass
        pass
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_tag_package (argv):
    parser = optparse.OptionParser()
    WorkArea.init_release_options (parser, True)
    parser.add_option ("--batch", dest="batch", default=False,
                       action="store_true")
    parser.add_option ("--previous", dest="previous", default=False,
                       action="store_true")
    parser.add_option ("--author", dest="author",
                       default=os.getenv ("ROOTCORE_AUTHOR"), action="store")
    parser.add_option ("--tag", dest="tag",
                       default=None, action="store")
    (args, extra_args) = parser.parse_args (argv)
    if len (extra_args) < 1:
        parser.error ("incorrect number of arguments")
        pass
    args.message = extra_args

    print "message begin"
    print "\n".join (args.message)
    #print string.join (args.message, "\n")
    print "message end"

    svn_info = SvnInfo.make_srcdir (os.getcwd())

    if not svn_info.isHead():
        print "not in the head of the package"
        svn_info = svn_info.useTag ("trunk")
        if not args.batch:
            if ask_question ("please confirm changing to trunk",
                             ["yes"]) != "yes":
                raise RCError ("aborting")
            pass
        pass

    # rationale: putting this here to give you an option to check
    #   while reading tags from the server
    svn_info.rc_update (clobberSwitch=True)
    svn_info.svn_status (noReturn=True)

    isBranch = False
    if args.tag :
        tag = args.tag
        pass
    elif not svn_info.isTrunk() :
        isBranch = True
        branch = svn_info.tag()
        pass
    else :
        tags = svn_info.svn_ls_tags()
        if len (tags) > 0:
            # whether to pick up the latest tag, or the one before it
            if args.previous:
                tag = tags[-2]
                pass
            else:
                tag = tags[-1]
                pass
            
            tag = tag.strip()
            #tag = string.strip (tag)
            while tag[-1] == "/":
                tag = tag[0:-1]
                pass

            # remove the hyphens from the version number nad make it
            # into an integer number
            tl = len (tag)
            version = tag[tl-8:tl-6] + tag[tl-5:tl-3] + tag[tl-2:]
            while version[0] == "0":
                version = version[1:]
                pass
            version = int (version)
            pass
        else:
            version = 0
            pass

        version = version + 1
        version = str (version)
        while len (version) < 6:
            version = "0" + version
            pass

        tag = "-".join ([svn_info.name(), version[0:2], version[2:4], version[4:6]])
        #tag = svn_info.name() + "-" + version[0:2] + "-" + version[2:4] + "-" + version[4:6]
        pass


    if isBranch :
        print "going into branch " + branch
        make_tag = False
        pass
    else :
        print "going into tag " + tag
        if args.batch:
            make_tag = True
            pass
        else:
            answer = ask_question ("create tag now or later",
                                   ["now", "later"])
            if answer == "now":
                make_tag = True
                pass
            else:
                make_tag = False
                pass
            pass
        pass

    answer = ask_question ("please confirm adding to ChangeLog",
                           ["yes", "no" ,"skip"])
    if answer == "no":
        raise RCError ("abort")
    if answer == "yes":
        changeLog = time.strftime ("%Y-%m-%d") + " " + args.author + "\n"
        for message in args.message:
            changeLog += format_block ("\t* ", "\t  ", 65, message)
            pass
        if isBranch :
            changeLog += "\t* going into branch " + branch + "\n"
            pass
        elif make_tag :
            changeLog += "\t* tagging as " + tag + "\n"
            pass
        else :
            changeLog += "\t* going into " + tag + "\n"
            pass

        changeLogFile = "ChangeLog"
        if os.path.exists (changeLogFile):
            changeLog += "\n" + open (changeLogFile, "r").read()
            with open (changeLogFile, "w") as file:
                file.write (changeLog)
                pass
            pass
        else:
            with open (changeLogFile, "w") as file:
                file.write (changeLog)
                pass
            svn_info.svn_add (changeLogFile)
            pass
        pass

    if svn_info.svn_status() != "":
        if args.batch:
            commit = True
            pass
        else:
            answer = ask_question ("please confirm committing changes",
                                   ["yes", "no"])
            if answer == "yes":
                commit = True
                pass
            else:
                commit = False
                pass
            pass
        pass

    if commit:
        svn_info.svn_commit ("\n".join (args.message))
        #svn_info.svn_commit (string.join (args.message, "\n"))
        print "commited changes"

        if make_tag:
            svn_info.svn_cp_tags (tag, "\n".join (args.message))
            #svn_info.svn_cp_tags (tag, string.join (args.message, "\n"))
            print "made tag " + tag
            pass
        pass
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_compile_pkg (argv):
    parser = optparse.OptionParser (usage="usage: %prog [options] package")
    parser.add_option ("--single-thread", dest="single_thread", action="store_true",
                       help="whether to run a single threaded compilation")
    parser.add_option ("--continue", dest="cont", action="store_true",
                       help="whether to continue on error")
    (args, extra_args) = parser.parse_args (argv)
    args.packages = extra_args

    if args.single_thread :
        os.environ ["ROOTCORE_NCPUS"] = "1"
        pass

    wa = WorkArea ()
    pkgList = []

    for package in args.packages :
        if package == "all" :
            for pkg in wa.packages() :
                if not pkg.name() in args.packages and pkg.recompile() :
                    pkgList.append (pkg)
                    pass
                pass
            pass
        else :
            pkg = wa.packages().getPkg (package)
            if not pkg:
                raise Exception ("unknown package: " + package)
            pkgList.append (pkg)
            pass
        pass

    for pkg in pkgList :
        pkg.linkPreconfigure ()
        pass

    wa.compileCore (pkgList, False, args.cont)
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_precompile_pkg (argv):
    parser = optparse.OptionParser (usage="usage: %prog [options] package")
    (args, extra_args) = parser.parse_args (argv)
    if len (extra_args) != 1:
        parser.error ("incorrect number of arguments")
        pass
    args.package = extra_args[0]

    wa = WorkArea ()
    pkg = wa.packages().getPkg (args.package)
    if not pkg:
        raise Exception ("unknown package: " + args.package)

    pkg.linkPrecompile ()
    print "finished pre-compiling " + args.package
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_postcompile_pkg (argv):
    parser = optparse.OptionParser (usage="usage: %prog [options] package")
    (args, extra_args) = parser.parse_args (argv)
    if len (extra_args) != 1:
        parser.error ("incorrect number of arguments")
        pass
    args.package = extra_args[0]

    wa = WorkArea ()
    pkg = wa.packages().getPkg (args.package)
    if not pkg:
        raise Exception ("unknown package: " + args.package)

    pkg.linkPostcompile ()
    shell_exec (["rc", "test_ut", "--package", pkg.name(), "--compile"], noReturn=True)
    print "finished compiling " + args.package
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_make_par (argv):
    parser = optparse.OptionParser (usage="usage: %prog [options] package")
    parser.add_option ("--nobuild", dest="mode", default="BUILD",
                       action="store_const", const="NOBUILD")
    parser.add_option ("--lite", dest="mode", default="BUILD",
                       action="store_const", const="LITE")
    (args, extra_args) = parser.parse_args (argv)
    if len (extra_args) > 1:
        parser.error ("incorrect number of arguments")
        pass

    wa = WorkArea()
    if len (extra_args) == 1:
        args.outfile = extra_args[0]
        if not os.path.isabs (args.outfile):
            args.outfile = os.path.join (os.getcwd(), args.outfile)
            pass
        pass
    else:
        args.outfile = os.path.join (wa.area(), "RootCore.par")
        pass
    if args.outfile[-4:] != ".par":
        raise RCError ("filename doesn't end in .par: " + args.outfile)

    dir = "/tmp"
    if os.getenv ("TMPDIR"):
        dir = os.getenv ("TMPDIR")
        pass
    dir = os.path.join (dir, "RootCoreParFile." + str (os.getpid()))
    if os.path.exists (dir):
        shutil.rmtree (dir, True)
        pass

    mydir = os.path.join (dir, os.path.basename (args.outfile)[:-4])
    mkdir_p (mydir)

    proof_inf = os.path.join (wa.dir(), "PROOF-INF", args.mode)
    shell_exec ([os.path.join (proof_inf, "COPY"), mydir], noReturn=True)
    mkdir_p (os.path.join (mydir, "PROOF-INF"))
    for fileName in "SETUP.C", "BUILD.sh":
        with open (os.path.join (proof_inf, fileName), "r") as file:
            content = file.read()
            pass
        content = content.replace ("%ROOTCOREDIR%", wa.dir())
        content = content.replace ("%ROOTCOREBIN%", wa.bin())
        content = content.replace ("%ROOTCOREOBJ%", wa.obj())
        content = content.replace ("%ROOTCORECONFIG%", wa.arch())
        #content = string.replace (content, "%ROOTCOREDIR%", wa.dir())
        #content = string.replace (content, "%ROOTCOREBIN%", wa.bin())
        #content = string.replace (content, "%ROOTCOREOBJ%", wa.obj())
        #content = string.replace (content, "%ROOTCORECONFIG%", wa.arch())
        with open (os.path.join (mydir, "PROOF-INF", fileName), "w") as file:
            file.write (content)
            pass
        pass

    shell_exec (["chmod", "+x", os.path.join (mydir, "PROOF-INF", "BUILD.sh")],
                noReturn=True)
    shell_exec (["tar", "-czf", args.outfile, os.path.basename (mydir)],
                workDir=os.path.dirname (mydir))
    shutil.rmtree (dir, True)
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_make_doxygen (argv):
    parser = optparse.OptionParser (usage="usage: %prog [options] package")
    (args, extra_args) = parser.parse_args (argv)
    if len (extra_args) != 1:
        parser.error ("incorrect number of arguments")
        pass
    output_dir = extra_args[0]

    wa = WorkArea()
    inputs = []
    strip = []
    for pkg in wa.packages():
        strip.append (pkg.srcdir())
        inputs.append (os.path.join (pkg.srcdir(), pkg.name()))
        inputs.append (os.path.join (pkg.srcdir(), "Root"))
        inputs.append (os.path.join (pkg.srcdir(), "python"))
        inputs.append (os.path.join (pkg.srcdir(), "util"))
        inputs.append (os.path.join (pkg.srcdir(), "doc"))
        pass
    input_line = ""
    for input in inputs :
        print input
        if os.path.exists (input) :
            input_line += " " + input
            pass
        pass
    strip_line = string.join (strip, " ")

    with open (os.path.join (wa.dir(), "doxygen.raw_conf"), "r") as file:
        content = file.read()
        pass
    content = content.replace ("%STRIP_LINE%", strip_line)
    content = content.replace ("%INPUT_LINE%", input_line)
    content = content.replace ("%OUTPUT_DIR%", output_dir)
    with open (os.path.join (wa.bin(), "doxygen.conf"), "w") as file:
        file.write (content)
        pass

    shell_exec (["doxygen", os.path.join (wa.bin(), "doxygen.conf")], noReturn=True)
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_du (argv):
    parser = optparse.OptionParser (usage="usage: %prog [options] package")
    (args, extra_args) = parser.parse_args (argv)
    if len (extra_args) > 0:
        parser.error ("incorrect number of arguments")
        pass

    wa = WorkArea()
    print "total\tobject\tsource\tpackage"
    for pkg in wa.packages():
        du_obj = 0
        du_src = 0
        if os.path.exists (pkg.srcdir()):
            du_src = int (shell_exec (["du", "-sk", pkg.srcdir()]).split()[0])
            #du_src = int (string.split (shell_exec (["du", "-sk", pkg.srcdir()]))[0])
            pass
        objdir = os.path.join (pkg.outdir(), wa.arch(), pkg.name())
        if os.path.exists (objdir):
            du_obj = int (shell_exec (["du", "-sk", objdir]).split()[0])
            #du_obj = int (string.split (shell_exec (["du", "-sk", objdir]))[0])
            pass

        print "%d\t%d\t%d\t%s" % (du_obj + du_src, du_obj, du_src, pkg.name())
        #print str (du_obj + du_src) + "\t" + str (du_obj) + "\t" + str (du_src) + "\t" + pkg.name()
        pass
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_get_cxxflags (argv):
    parser = optparse.OptionParser (usage="usage: %prog [options] packages")
    (args, extra_args) = parser.parse_args (argv)

    wa = WorkArea ()
    packages = PackageList (wa)
    for name in extra_args:
        pkg = wa.packages().getPkg (name)
        if not pkg:
            raise Exception ("unknown package: " + package)
        packages.addPkg (pkg)
        pass

    print packages.dep().getCxxFlags (user=True)
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_get_ldflags (argv):
    parser = optparse.OptionParser (usage="usage: %prog [options] packages")
    (args, extra_args) = parser.parse_args (argv)

    wa = WorkArea ()
    packages = PackageList (wa)
    for name in extra_args:
        pkg = wa.packages().getPkg (name)
        if not pkg:
            raise Exception ("unknown package: " + package)
        packages.addPkg (pkg)
        pass

    print packages.dep().getBinFlags (user=True)
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_root (argv) :
    wa = WorkArea ()
    rc = shell_exec (["root", os.path.join (wa.dir(), "scripts", "load_packages.C")] + argv, returnRC=True)
    sys.exit (rc)
    pass

###############################################################################
###############################################################################
###############################################################################

def rc_exec (argv) :
    wa = WorkArea ()
    rc = shell_exec (argv, returnRC=True)
    sys.exit (rc)
    pass
