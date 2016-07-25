import os
import re
import sys
#import string
import operator
import shutil
import multiprocessing
import fnmatch

# Local imports
from utils import RCError, get_field, get_field_env, mkdir_p, shell_exec
from svninfo import SvnInfo, SvnInfoUnmanaged
from compile import Makefile

class PackageList (object) :
    """description: this class implements a wrapper around the list of
    packages that provides some extra functionality.  to the outside
    it provides a regular iterable interface, but it provides some
    additional checks and functionality"""

    def __init__ (self, wa) :
        # the actual list of packages
        self.m_packages = []
        self.m_wa = wa
        pass

    def __iter__ (self) :
        return iter (self.m_packages)

    def addPkg (self, pkg) :
        """effects: add the given package to the list"""
        if pkg.m_wa != self.m_wa:
            raise RCError ("trying to add package belonging to a" +
                           " different working area")
        old = self.getPkg (pkg.name())
        if old:
            self.m_packages.remove (old)
        self.m_packages.append (pkg)
        pass

    def getPkg (self, name) :
        """returns: the package with the given name, or None if it
        can't be found"""
        if name[-1] == '/' :
            name = name[0:-1]
            pass
        for pkg in self.m_packages:
            if pkg.name() == name:
                return pkg
            pass
        return None

    def mergePackages (self, packages):
        """effects: merge the given package list into this one.  for
        packages that are already in this list, the information from
        both versions of the package gets merged"""
        if packages.m_wa != self.m_wa:
            raise RCError ("trying to add package belonging to a" +
                           " different working area")
        for pkg in packages:
            mypkg = self.getPkg (pkg.name())
            if mypkg:
                mypkg.mergePackage (pkg)
                pass
            else:
                self.m_packages.append (pkg)
                pass
            pass
        pass

    def sortByName (self):
        """effects: sort this package list by name
        rationale: normally the package list is ordered by dependency
        order, but inside find_packages I first order them by name in
        order to get a deterministic results as dependency ordering
        typically allows multiple orders"""
        self.m_packages = sorted(self.m_packages,
                                 key=operator.attrgetter('m_name'))
        pass

    # NOTE: This method and the next could be improved by building a list
    # of flags and joining them together only at the end.
    def getCxxFlags (self, user=False, opt=True, pedantic=False, external=False):
        """returns: the compiler flags to use when compiling code
        depending on all the packages in the list"""
        result = ""
        if not user:
            if opt:
                result = get_field (self.m_wa.root_conf(), "CXXFLAGS")
                pass
            else:
                result = get_field (self.m_wa.root_conf(), "CXXFLAGS_NOOPT")
                pass
            pass
        result = result.strip() + " -I" + self.m_wa.bin() + "/include -g -Wno-tautological-undefined-compare"
        #result = string.strip (result + " " + "-I" + self.m_wa.bin() + "/include -g")
        if not external:
            result += " -DROOTCORE"
            pass
        if not user:
            result = str(result + " " +
                         shell_exec (["root-config", "--cflags"])).strip()
            #result = string.strip (result + " " + shell_exec (["root-config", "--cflags"]))
            pass
        for var in ["CXXFLAGS", "EXTRA_CFLAGS", "CPPEXFLAGS"]:
            flags = os.getenv (var)
            if flags and flags != "":
                result = str(result + " " + flags).strip()
                #result = string.strip (result + " " + flags)
                pass
            pass
        # if not user:
        #     boostinc = os.getenv ("BOOSTINCDIR")
        #     if boostinc:
        #         result = string.strip (result + " -I" + boostinc)
        #         pass
        #     pass
        if pedantic and not external:
            result = str(result + " -pipe -W -Wall -Wno-deprecated" +
                         " -pedantic -Wwrite-strings -Wpointer-arith" +
                         " -Woverloaded-virtual -Wno-long-long" +
                         " -Wdeprecated-declarations").strip()
            pass
        for pkg in self.m_packages:
            result = str(result + " " + pkg.objflags()).strip()
            #result = string.strip (result + " " + pkg.objflags())
            pass
        return result

    def getLibFlags (self, lib, user=False, external=False):
        """returns: the linker flags to use when linking a library
        depending on all the packages in the list"""
        result = ""
        if not user and not external:
            result = str(result + " " +
                         get_field (self.m_wa.root_conf(), "SOFLAGS")).strip()
            #result = string.strip (result + " " + get_field (self.m_wa.root_conf(), "SOFLAGS"))
            pass
        result = str(result + " " +
                     shell_exec (["root-config", "--ldflags"])).strip()
        #result = string.strip (result + " " + shell_exec (["root-config", "--ldflags"]))
        for var in ["LDFLAGS", "CPPEXFLAGS"]:
            flags = os.getenv (var)
            if flags and flags != "":
                result = str(result + " " + flags).strip()
                pass
            pass
        if not user:
            result += " " + get_field (self.m_wa.root_conf(), "ROOTLIBS")
            pass
        if (not user and not external and
            get_field (self.m_wa.root_conf(), "PLATFORM") == "macosx"):
            result = str(result + " -dynamiclib -undefined dynamic_lookup" +
                         " -install_name @rpath/" +
                         os.path.basename (lib)).strip()
            #result = string.strip (result + " -dynamiclib -undefined dynamic_lookup -install_name @rpath/" + os.path.basename (lib))
            pass
        return result;

    def getBinFlags (self, user=False):
        """returns: the linker flags to use when linking a binary
        depending on all the packages in the list"""
        preresult = ["-L" + os.path.join (self.m_wa.bin(), "lib",
                                          self.m_wa.arch())]
        result = get_field (self.m_wa.root_conf(), "LDFLAGS").split()
        #result = string.split (get_field (self.m_wa.root_conf(), "LDFLAGS"))
        for var in ["LDFLAGS"]:
            flags = os.getenv (var)
            if flags and flags != "":
                preresult += flags.split()
                #preresult += string.split (flags)
                pass
            pass
        for pkg in self.m_packages:
            preresult += pkg.libflags().split()
            #preresult += string.split (pkg.libflags())
            result = pkg.binflags().split() + result
            #result = string.split (pkg.binflags()) + result
            for preload in pkg.preload():
                result = ["-l" + preload] + result
                pass
            if not pkg.nocc():
                result = ["-l" + pkg.name()] + result
            pass
        if not user:
            result += get_field (self.m_wa.root_conf(), "ROOTLIBS").split()
            #result += string.split (get_field (self.m_wa.root_conf(), "ROOTLIBS"))
        return " ".join (preresult + result)
        #return string.join (preresult + result, " ")

    def dep (self):
        """returns: the list of all packages including their dependencies"""
        result = PackageList (self.m_wa)
        for pkg in self.m_packages:
            for dep in pkg.dep():
                if not result.getPkg (dep):
                    result.addPkg (self.m_wa.packages().getPkg (dep))
                    pass
                pass
            pass
        return result

    pass



class PackageInfo (object):
    """description: the class for managing all the information for a
    particular package"""

    def __init__ (self, wa) :
        self.m_wa = wa
        self.m_name = None
        self.m_release = None
        self.m_srcdir = None
        self.m_outdir = None
        self.m_dep = None
        self.m_recompile = None
        self.m_auto = None
        self.m_nocc = None
        self.m_preload = None
        self.m_objdir = None
        self.m_libdir = None
        self.m_bindir = None
        self.m_testdir = None
        self.m_cxxflags = None
        self.m_objflags = None
        self.m_ldflags = None
        self.m_libflags = None
        self.m_binflags = None
        self.m_noopt = None
        self.m_pedantic = None
        self.m_nicos_name = None
        self.m_objfiles = None
        self.m_useReflex = None
        pass

    def wa (self) :
        """description: our working area"""
        return self.m_wa

    def name (self) :
        """description: the name of the package"""
        return self.m_name

    def release (self) :
        """description: whether the package comes from a release area"""
        return self.m_release

    def srcdir (self) :
        """description: the source directory of the package"""
        return self.m_srcdir

    def set_srcdir (self, value) :
        """effects: set the value of srcdir"""
        self.m_srcdir = value
        for field in "m_svninfo_srcdir", "m_svninfo" :
            if hasattr (self, field) :
                delattr (self, field)
                pass
            pass
        pass

    def outdir (self) :
        """description: the output directory for the package compilation
        warning: this essentially points to $ROOTCOREBIN/obj (or the
        release area equivalent, not the actual package and
        architecture specific subdirectory"""
        return self.m_outdir

    def dep (self) :
        """description: the names of the packages this package depends on
        rationale: this is only names not packages, as the packages
        may not all be known by the time this is filled"""
        return self.m_dep

    def deplist (self, list) :
        """description: the list of the packages this package depends
        on, picked up from PackageList"""
        result = PackageList (list.m_wa)
        for dep in self.dep():
            result.addPkg (list.getPkg (dep))
            pass
        return result

    def recompile (self) :
        """description: whether this package needs to be recompiled
        rationale: this is true for all local packages, as well as all
        release packages that depend on local packages"""
        return self.m_recompile

    def svninfo_srcdir (self) :
        """description: the SVN info object read from the source
        directory, or None if the source directory does not exist or
        has no SVN information"""
        if not hasattr (self, "m_svninfo_srcdir") :
            if self.srcdir() and os.path.exists (self.srcdir()) :
                self.m_svninfo_srcdir = SvnInfo.make_srcdir (self.srcdir(),
                                                             allowNone=True)
                pass
            else :
                self.m_svninfo_srcdir = None
                pass
            #if self.m_svninfo_srcdir and not isinstance (self.m_svninfo_srcdir, SvnInfo) :
            if (self.m_svninfo_srcdir and
                not isinstance (self.m_svninfo_srcdir, SvnInfo)):
                raise Exception ("result not an svninfo object")
            pass
        return self.m_svninfo_srcdir

    def svninfo_packages (self) :
        """description: the SVN info object as cached in the packages
        file, or None if it hasn't been cached there"""
        if hasattr (self, "m_svninfo_packages") :
            #if self.m_svninfo_packages and not isinstance (self.m_svninfo_packages, SvnInfo) :
            if (self.m_svninfo_packages and
                not isinstance (self.m_svninfo_packages, SvnInfo)):
                raise Exception ("result not an svninfo object")
            return self.m_svninfo_packages
        return None

    def set_svninfo_packages (self, value) :
        """effects: set the value of svninfo_packages()"""
        if not isinstance (value, SvnInfo) :
            raise Exception ("value not an svninfo object")
        self.m_svninfo_packages = value
        for field in "m_svninfo" :
            if hasattr (self, field) :
                delattr (self, field)
                pass
            pass
        pass

    def svninfo (self) :
        """description: the best estimate of the SVN info object of
        this package"""
        if not hasattr (self, "m_svninfo") :
            svninfo = self.svninfo_srcdir()
            if not svninfo :
                svninfo = self.svninfo_packages()
                pass
            if not svninfo :
                svninfo = SvnInfoUnmanaged (self.srcdir())
                pass
            if not isinstance (svninfo, SvnInfo) :
                raise Exception ("result not an svninfo object")
            self.m_svninfo = svninfo
            pass
        return self.m_svninfo

    def harddep (self) :
        """description: all the hard dependencies reported by the
        package, i.e. the one it insists it needs in order to work"""
        dep = get_field (self.conf_makefile(), "PACKAGE_DEP")
        if dep:
            return dep.split()
            #return string.split (dep)
        return []

    def trydep (self) :
        """description: all the trial dependencies reported by the
        package, i.e. the ones it will depend on when they are there,
        but can live without when they are not there"""
        dep = get_field (self.conf_makefile(), "PACKAGE_TRYDEP")
        if dep:
            return dep.split()
            #return string.split (dep)
        return []

    def auto (self) :
        """description: the auto-dependency reported by the package.
        this is an integer which is 0 for regular packages.  if this
        is greater than 0 this package will be added to all other
        packages, except those with a greater auto-dependency
        setting
        rationale: sometimes I introduce global packages that all
        other packages should use, but since it is is impractical to
        update the dependencies for all other packages those packages
        should just set an auto-dependency instead"""
        if not self.m_auto:
            dep = get_field (self.conf_makefile(), "PACKAGE_AUTODEP")
            if dep:
                self.m_auto = int (dep)
                pass
            else:
                self.m_auto = 0
                pass
        return self.m_auto

    def nocc (self) :
        """description: whether the package has no library to compile
        rationale: some packages contain only header files.  to avoid
        an error when compiling them (and to avoid adding a dummy
        source file) I introduced this flag"""
        if self.m_nocc == None:
            nocc = get_field (self.cc_makefile(), "PACKAGE_NOCC")
            if nocc == "1":
                self.m_nocc = True
                pass
            else:
                self.m_nocc = False
                pass
            pass
        return self.m_nocc

    def preload (self) :
        """description: the list of external libraries this package
        needs to have loaded/linked for its own library to work"""
        if self.m_preload == None:
            preload = get_field (self.cc_makefile(), "PACKAGE_PRELOAD")
            if preload:
                self.m_preload = preload.split()
                #self.m_preload = string.split (preload)
                pass
            else:
                self.m_preload = []
                pass
            pass
        return self.m_preload

    def objdir (self) :
        """description: the object directory for the package, i.e. the
        directory into which the object files should be placed"""
        if not self.m_objdir:
            self.m_objdir = os.path.join (self.outdir(), self.m_wa.arch(),
                                          self.name(), "obj")
            pass
        return self.m_objdir

    def libdir (self) :
        """description: the library directory for the package,
        i.e. the directory containing the libraries, as well as any
        other generated files that we need to keep around"""
        if not self.m_libdir:
            self.m_libdir = os.path.join (self.outdir(), self.m_wa.arch(),
                                          self.name(), "lib")
            pass
        return self.m_libdir

    def bindir (self) :
        """description: the binary directory for the regular programs
        of the package"""
        if not self.m_bindir:
            self.m_bindir = os.path.join (self.outdir(), self.m_wa.arch(),
                                          self.name(), "bin")
            pass
        return self.m_bindir

    def testdir (self) :
        """description: the binary directory for all the test binaries
        of the package"""
        if not self.m_testdir:
            self.m_testdir = os.path.join (self.outdir(), self.m_wa.arch(),
                                           self.name(), "test-bin")
            pass
        return self.m_testdir

    def conf_makefile (self) :
        """description: the makefile to use for this package during
        configuration
        rationale: this should be used prior to compilation to extract
        parameters that do not depend on the auto-configuration"""
        if not hasattr (self, "m_conf_makefile") :
            self.m_conf_makefile = os.path.join (self.srcdir(), "cmt",
                                                 "Makefile.RootCore")
            pass
        return self.m_conf_makefile

    def cc_makefile (self) :
        """description: the makefile to use for this package during
        compilation
        rationale: the auto-configuration has the option to modify
        settings in the makefile related to compilation based on the
        environment detected.  since we are not allowed to modify
        files in the source area we place a copy into the object
        directoy and modify it there"""
        if not hasattr (self, "m_cc_makefile") :
            self.m_cc_makefile = os.path.join (self.libdir(),
                                               "Makefile.RootCore")
            pass
        return self.m_cc_makefile

    def cxxflags (self) :
        """description: the compilation flags reported by the package
        that are *not* propagated to packages depending on it"""
        if self.m_cxxflags == None:
            self.m_cxxflags = get_field_env (self.cc_makefile(),
                                             "PACKAGE_CXXFLAGS")
            pass
        return self.m_cxxflags

    def objflags (self) :
        """description: the compilation flags reported by the package
        that *are* propagated to packages depending on it"""
        if self.m_objflags == None:
            self.m_objflags = get_field_env (self.cc_makefile(),
                                             "PACKAGE_OBJFLAGS")
            pass
        return self.m_objflags

    def ldflags (self) :
        """description: the linker flags (for linking a library) that
        are *not* propagated to packages depending on it"""
        if self.m_ldflags == None:
            self.m_ldflags = get_field_env (self.cc_makefile(),
                                            "PACKAGE_LDFLAGS")
            pass
        return self.m_ldflags

    def libflags (self) :
        """description: the linker flags (for linking a library) that
        *are* propagated to packages depending on it"""
        if self.m_libflags == None:
            self.m_libflags = get_field_env (self.cc_makefile(),
                                             "PACKAGE_LIBFLAGS")
            pass
        return self.m_libflags

    def binflags (self) :
        """description: the linker flags to use when linking a binary
        in this package or a package depending on it"""
        if self.m_binflags == None:
            self.m_binflags = get_field_env (self.cc_makefile(),
                                             "PACKAGE_BINFLAGS")
            pass
        return self.m_binflags

    def getCxxFlags (self, opt=True, pedantic=False, incOnly=False,
                     external=False) :
        """returns: the flags to be passed to the compiler when
        compiling a single source file of this package"""
        if incOnly:
            result = []
            #for flag in string.split (self.getCxxFlags()):
            for flag in self.getCxxFlags().split():
                if flag.find ("-I") == 0 or flag.find ("-D") == 0:
                    result.append (flag)
                    pass
                pass
            return " ".join (result)
            #return string.join (result, " ")
        list = self.deplist(self.m_wa.packages())
        result = "-I" + os.path.join (self.srcdir(), "Root")
        result += " -I" + self.srcdir()
        result += " " + list.getCxxFlags (opt=opt, pedantic=pedantic,
                                          external=external)
        #result = "-I" + os.path.join (self.srcdir(), "Root") + " -I" + self.srcdir() + " " + list.getCxxFlags (opt=opt, pedantic=pedantic, external=external)
        if not external:
            # rationale: this is for CLHEP, which chokes on the quotation mark
            result += ' -DROOTCORE_PACKAGE=\\"' + self.name() + '\\" '
            pass
        result += self.cxxflags()
        return result

    def getLibFlags (self, external=False) :
        """returns: the flags to be passed to the linker when building
        the library of this package"""
        list = self.deplist(self.m_wa.packages())
        result = list.getLibFlags (self.libdir() + "/lib" + self.name() + ".so",
                                   external=external)
        result += " " + self.ldflags()
        return result
        #return list.getLibFlags (self.libdir() + "/lib" + self.name() + ".so", external=external) + " " + self.ldflags()

    def getBinFlags (self) :
        """returns: the flags to be passed to the linker when
        producing an executable"""
        list = self.deplist(self.m_wa.packages())
        return str("-L" + self.libdir() + " " + list.getBinFlags()).strip()
        #return string.strip ("-L" + self.libdir() + " " + list.getBinFlags())

    def noopt (self) :
        """description: the optimization requested by the package.
        this is "1" to disable the optimization and "dict" to disable
        it only for the dictionary (which can be very large and slow
        to compile)"""
        if self.m_noopt == None:
            self.m_noopt = get_field_env (self.cc_makefile(), "PACKAGE_NOOPT")
            pass
        return self.m_noopt

    def scriptPattern (self) :
        """description: a list of patterns describing the scripts that
        should be put/linked into the paths
        rationale: some packages like to provide executables not in
        the form of binaries, but scripts.  those packages need to set
        this field accordingly"""
        if not hasattr (self, "m_scriptPattern") :
            self.m_scriptPattern = get_field_env (self.conf_makefile(),
                                                  "PACKAGE_SCRIPTS").split()
            #self.m_scriptPattern = string.split (get_field_env (self.conf_makefile(), "PACKAGE_SCRIPTS"))
            pass
        return self.m_scriptPattern

    def pedantic (self) :
        """description: whether pedantic compilation has been requested"""
        if self.m_pedantic == None:
            self.m_pedantic = get_field_env (self.cc_makefile(),
                                             "PACKAGE_PEDANTIC") == "1"
            pass
        return self.m_pedantic

    def nicos_name (self) :
        """description: the name we have for nicos
        rationale: nicos performs a special name mangling that
        incorporates the path of the package inside svn"""
        if self.m_nicos_name == None:
            self.m_nicos_name = self.svninfo().nicosName()
            pass
        return self.m_nicos_name


    def mergePackage (self, pkg) :
        """effects: merge the given package into this one, i.e. copy
        certain fields from that package into this one, if they are
        set in that package"""
        if self.m_name != pkg.m_name:
            raise RCError ("trying to merge package " + pkg.m_name +
                           " into package " + self.m_name)
        if pkg.m_srcdir:
            self.srcdir (pkg.m_srcdir)
            pass
        if pkg.m_outdir:
            self.m_outdir = pkg.m_outdir
            pass
        if pkg.m_dep:
            self.m_dep = pkg.m_dep
            pass
        if pkg.m_recompile != None:
            self.m_recompile = pkg.m_recompile
            pass
        if pkg.svninfo_packages() :
            self.set_svninfo_packages (pkg.svninfo_packages())
            pass
        pass

    @classmethod
    def parsePackagesLine (cls, line, wa) :
        """effects: parse a line from the packages and fill the
        information into a new PackageInfo object"""
        elem = line.split(":")
        #elem = string.split (line, ":")
        if len (elem) != 7:
            raise RCError ("failed to parse package file line, consider" +
                           " rerunning find_packages\noffending line:\n" + line)
        pkg = PackageInfo (wa)
        pkg.m_name = elem[0]
        if elem[1] != "":
            pkg.m_release = int (elem[1])
            pass
        pkg.m_srcdir = wa.expandPath (elem[2])
        pkg.m_outdir = wa.expandPath (elem[3])
        if elem[4] != "":
            pkg.m_dep = elem[4].split()
            #pkg.m_dep = string.split (elem[4])
            pass
        if elem[5] != "":
            if elem[5] == "1":
                pkg.m_recompile = True
                pass
            elif elem[5] == "0":
                pkg.m_recompile = False
                pass
            else:
                raise RCError ("invalid value for recompile: " + elem[5])
            pass
        svninfo = SvnInfo.make_packages (elem[6], pkg.m_srcdir)
        if svninfo :
            pkg.m_svninfo_packages = svninfo
        return pkg
    pass

    def writePackagesLine (self, file):
        """effects: writes the content of this package as a line to
        the given file (in packages format)"""
        file.write (self.name() + ":")
        if self.release() == True:
            file.write ("1")
            pass
        elif self.release() == False:
            file.write ("0")
            pass
        file.write (":" + self.m_wa.compressPath (self.srcdir()) +
                    ":" + self.m_wa.compressPath (self.outdir()) +
                    ":" + " ".join (self.dep()) + ":")
                    #":" + string.join (self.dep()) + ":")
        if self.recompile() == True:
            file.write ("1")
            pass
        elif self.recompile() == False:
            file.write ("0")
            pass
        file.write (":" + self.svninfo().forPackages() + "\n")
        pass

    def linkPreconfigure (self) :
        """effects: do all the linking of all files that can be linked
        before running the configuration of the package
        rationale: I link all the include directories, etc. first, so
        that the problems from broken dependencies are not quite as
        awkward too debug
        rationale: a secondary motive is that this should make it
        easier to enable parallel compilation of all packages, instead
        of compiling one package at a time"""
        if self.name() != "RootCore" :
            source = os.path.join (self.srcdir(), self.name())
            pass
        else :
            source = os.path.join (self.wa().bin(), "RootCore")
            pass
        if os.path.isdir (source):
            self.m_wa.lnRel (source, os.path.join (self.m_wa.bin(), "include",
                                                   self.name()))
            pass
        if self.name() != "RootCore" :
            source = os.path.join (self.srcdir(), "python")
            if os.path.isdir (source):
                self.m_wa.lnRel (source, os.path.join (self.m_wa.bin(), "python",
                                                       self.name()))
                pass
            source = os.path.join (self.srcdir(), "scripts")
            if os.path.isdir (source):
                self.m_wa.lnRel (source, os.path.join (self.m_wa.bin(),
                                                       "user_scripts",
                                                       self.name()))
                pass
            source = os.path.join (self.srcdir(), "data")
            if os.path.isdir (source):
                self.m_wa.lnRel (source, os.path.join (self.m_wa.bin(), "data",
                                                       self.name()))
                pass
            else:
                source = os.path.join (self.srcdir(), "share")
                if os.path.isdir (source):
                    self.m_wa.lnRel (source, os.path.join (self.m_wa.bin(),
                                                           "data", self.name()))
                    pass
                pass
            pass
        for pattern in self.scriptPattern() :
            source = os.path.join (self.srcdir(), os.path.dirname (pattern))
            for file in os.listdir (source) :
                if fnmatch.fnmatch (file, os.path.basename (pattern)) :
                    self.m_wa.lnRel (os.path.join (source, file),
                                     os.path.join (self.m_wa.bindir(), file))
                    pass
                pass
            pass
        pass

    def linkPrecompile (self) :
        """effects: link all the files created during the
        auto-configuration step"""
        if self.name() == "RootCore":
            return

        dir = self.libdir()
        if os.path.isdir (dir):
            expr = re.compile ("^" + self.name() + "_.*.h$")
            for file in os.listdir (dir):
                if expr.match (file):
                    self.m_wa.lnRel (os.path.join (dir, file),
                                     os.path.join (self.m_wa.bin(),
                                                   "RootCore", file))
                    pass
                pass
            pass
        pass

    def linkPostcompile (self) :
        """effects: link all the files created during compilation"""
        if self.name() == "RootCore":
            return

        ROOTCORECONFIG = self.m_wa.arch()

        #for file in "lib" + self.name() + ".so", "lib" + self.name() + "_Reflex.so", "lib" + self.name() + "_Reflex.rootmap", self.name() + "CINT_rdict.pcm", self.name() + "_Reflex_rdict.pcm":
        for file in ("lib" + self.name() + ".so",
                     "lib" + self.name() + "_Reflex.so",
                     "lib" + self.name() + "_Reflex.rootmap",
                     self.name() + "CINT_rdict.pcm",
                     self.name() + "_Reflex_rdict.pcm"):
            source = os.path.join (self.libdir(), file)
            if os.path.isfile (source):
                self.m_wa.lnRel (source, os.path.join (self.m_wa.bin(), "lib",
                                                       ROOTCORECONFIG, file))
                pass
            pass

        dir = self.bindir()
        if os.path.isdir (dir):
            for file in os.listdir (dir):
                self.m_wa.lnRel (os.path.join (dir, file),
                                 os.path.join (self.m_wa.bin(), "bin",
                                               ROOTCORECONFIG, file))
                pass
            pass

        file = os.path.join (self.srcdir(), "cmt", "link.RootCore")
        if os.path.isfile (file):
            if not os.access (file, os.X_OK):
                raise RCError ("execute flag not set on " +
                               os.path.basename (file) +
                               ", please run\n  chmod +x " + file);
            newEnv = {}
            newEnv["ROOTCORECMT"] = self.libdir()
            newEnv["ROOTCOREBIN"] = self.m_wa.bin()
            newEnv["ROOTCOREOBJ"] = self.m_wa.obj()
            shell_exec ([file], workDir=os.path.join (self.srcdir(), "cmt"),
                        noReturn=True, newEnv=newEnv)
            pass
        pass

    def compileCore (self, ignoreErrors, logFile) :
        """description: do all the actual compilation and linking of
        files needed for this package, except for the files linker
        during preconfigure (which are linked globally beforehand)"""
        if self.name() != "RootCore":
            for dir in "bin", "StandAlone", "test-bin", "obj":
                path = os.path.join (self.srcdir(), dir)
                if os.path.isdir (path):
                    logFile.write ("removing obsolete directory " + path + "\n")
                    shutil.rmtree (path, True)
                    pass
                pass
            pass

        make_flags = []
        if ignoreErrors:
            make_flags += ["-k"]
            pass

        ROOTCORE_NCPUS = os.getenv ("ROOTCORE_NCPUS")
        if ROOTCORE_NCPUS:
            make_flags += ["-j", ROOTCORE_NCPUS]
            pass
        else:
            make_flags += ["-j", str (multiprocessing.cpu_count())]
            pass

        OLDPATH = os.getcwd()

        if self.recompile() == True:
            logFile.write ("compiling " + self.name() + "\n")
            mkdir_p (self.libdir())
            shutil.copyfile (self.conf_makefile(), self.cc_makefile())
            if not self.pedantic() :
                logFile.write ("WARNING: package " + self.name() + " doesn't request pedantic compilation\n")
                logFile.write ("WARNING: fix this by setting 'PACKAGE_PEDANTIC = 1' in Makefile.RootCore\n")
                pass
            for dep in self.harddep():
                if not self.m_wa.packages().getPkg (dep):
                    raise RCError ("unknown dependency: " + dep)
                pass
            file = os.path.join (self.srcdir(), "cmt", "precompile.RootCore")
            if os.path.isfile (file):
                if not os.access (file, os.X_OK):
                    raise RCError ("execute flag not set on " +
                                   os.path.basename (file) +
                                   ", please run\n  chmod +x " + file);
                try:
                    os.chdir (os.path.join (self.srcdir(), "cmt"))
                    os.environ["ROOTCORECMT"] = self.libdir()
                    rc = shell_exec ([file], returnRC=True, stdout=logFile,
                                     stderr=logFile)
                    if rc != 0:
                        raise RCError ("failed to execute " + file)
                    pass
                finally:
                    os.chdir (OLDPATH)
                    del os.environ["ROOTCORECMT"]
                    pass
                pass
            pass
        self.linkPrecompile ()

        if self.recompile() == True:
            self.makePreamble ()
            try:
                os.chdir (os.path.join (self.srcdir(), "cmt"))
                shell_args = ["make", "-f", self.cc_makefile(),
                              "RC_INTERNAL_PREAMBLE=" + self.libdir() +
                              "/preamble.make"] + make_flags
                rc = shell_exec (shell_args, stdout=logFile, stderr=logFile,
                                 returnRC=True)
                #rc = shell_exec (["make", "-f", self.cc_makefile(), "RC_INTERNAL_PREAMBLE="+self.libdir() + "/preamble.make"] + make_flags, stdout=logFile, stderr=logFile, returnRC=True)
                if rc != 0:
                    raise RCError ("failed to compile package " + self.name())
                pass
            finally:
                os.chdir (OLDPATH)
                pass
            pass
        self.linkPostcompile ()
        pass

    def makePreamble (self) :
        """effects: make the make preamble for this package, i.e. the
        Makefile that contains all the package specific rules"""
        self.m_wa.root_conf ()
        path = os.path.join (self.libdir(), "preamble.make")
        with open (path, "w") as file:
            makefile = Makefile (self.m_wa, self)
            makefile.addFiles ()
            makefile.write (file)
            pass
        return path

    def useReflex (self):
        """returns: whether we use a Reflex dictionary"""
        if self.m_useReflex == None:
            self.m_useReflex = get_field (self.cc_makefile(),
                                          "PACKAGE_REFLEX") == "1"
            pass
        return self.m_useReflex

    def compile (self, ignoreErrors = False, logFile = None) :
        """effects: perform the compilation of the package
        side effects: may change the current path"""
        result = True
        if logFile == None:
            logFile = sys.stdout
            pass

        try:
            self.compileCore (ignoreErrors, logFile)
            pass
        except Exception as e:
            if not ignoreErrors:
                raise
            result = False
            logFile.write ("RootCore: error compiling " + self.name() + "\n")
            logFile.write (str (e) + "\n")
            pass
        return result


    def copySource (self, target) :
        """effects: copy our source directory to the source directory
        of the target"""
        if self.srcdir() == target.srcdir():
            print "using " + self.name() + " source from release"
            pass
        else:
            print "copying " + self.name() + " source"
            ignore = [".svn", "*~"]
            if self.name() == "RootCore":
                ignore += ["bin", "include", "obj", "rootcore_config",
                           "root_config_*", "load_packages_info_*",
                           "load_packages_success", "data", "lib", "python",
                           "user_scripts"]
                pass
            rel_dir = self.m_wa.relPath (target.m_wa.area(), self.srcdir(), self.srcdir())
            if rel_dir[0] != "/" and rel_dir != ".":
                ignore.append (os.path.basename (rel_dir))
                pass
            if rel_dir != ".":
                shutil.copytree (self.srcdir(), target.srcdir(), symlinks = True,
                                 ignore = shutil.ignore_patterns (*ignore))
                shell_exec (["chmod", "-R", "u+rw", target.srcdir()])
                pass
            pass
        pass

    def copyObject (self, target, config) :
        """effects: copy the object files from the source to the
        target"""
        if self.outdir() == target.outdir():
            print "using " + self.name() + " object files from release"
            pass
        else:
            print "copying " + self.name() + " for " + config
            for dir in "bin", "lib", "test-bin":
                source = os.path.join (self.outdir(), config,
                                       self.name(), dir)
                target_dir = os.path.join (target.outdir(), config,
                                           self.name(), dir)
                if os.path.isdir (source):
                    mkdir_p (os.path.dirname (target_dir))
                    shutil.copytree (source, target_dir)
                    shell_exec (["chmod", "-R", "u+rw", target_dir])
                    pass
                pass
            pass
        target.linkPreconfigure()
        target.linkPrecompile()
        target.linkPostcompile()
        pass

    def ut_test_list(self,recursive):
        """returns: the list of tests we have"""
        recursive_ut = {}
        for ut in get_field (self.conf_makefile(), "PACKAGE_RECURSIVE_UT", default="").split() :
            recursive_ut[ut] = ""
            pass

        result = []
        name_expr = re.compile("^((ut|gt|it|pt)_[^.]*)(.*)$")
        testdir = os.path.join(self.srcdir(), "test")
        if os.path.isdir(testdir):
            for file in os.listdir(testdir):
                test = None
                match = name_expr.match(file)
                if match:
                    if match.group(3) == ".cxx":
                        test = os.path.join(self.testdir(), match.group(1))
                        pass
                    elif match.group(3) == ".sh" or match.group(3) == ".py" or match.group(3) == ".C":
                        test = os.path.join(testdir, file)
                        pass
                    pass
                if test :
                    rec = False
                    name = os.path.basename (test)
                    if name in recursive_ut :
                        rec = True
                        del recursive_ut[name]
                        pass
                    if rec or not recursive :
                        result.append (test)
                        pass
                    pass
                pass
            pass
        for test in recursive_ut.keys() :
            raise RCError ("unknown recursive test: " + test)
        return result
    pass
