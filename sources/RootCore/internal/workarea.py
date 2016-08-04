import os
import sys
import re
import string
import optparse
import copy

# Local imports
from utils import RCError, get_field, set_field, shell_exec, mkdir_p
from package import PackageList, PackageInfo

class RelPath:
    """description: this class allows to determine whether a path is
    relative completely within the workarea.  this tries to preserve
    the path structure as much as possible, i.e. while it resolves
    symlinks as much as needed to make sure relative paths traversing
    symlinks are valid, it will use any symlinks that can be safely
    part of a relative path

    rationale: this is used internally, mainly to make all links/paths
    within the work-area/release relative so that it can be moved and
    relocated as needed"""

    def __init__(self, path):
        if not os.path.isabs(path):
            path = os.path.join(os.getcwd(), path)

        self.m_area = ["/"]
        self.m_path = []
        segments = string.split(path, "/")
        while len(segments) > 0:
            segment = segments[0]
            segments = segments[1:]
            if segment == "" or segment == ".":
                pass
            elif segment == "..":
                mypath = os.path.join(*(self.m_path + [segment]))
                if os.path.islink(mypath):
                    link = os.readlink(mypath)
                    if not os.path.isabs(link):
                        link = os.path.join(os.path.dirname(mypath), link)
                    segments = string.split(link, "/") + segments
                    self.m_path = []
                else:
                    self.m_path = self.m_path[0:-1]
            else:
                self.m_path.append(segment)

    def resolve_one(self):
        if len(self.m_path) == 0:
            raise Exception("resolve_one needs to have a user path left")

        segment = self.m_path[0]
        self.m_path = self.m_path[1:]
        path = os.path.join(*(self.m_area + [segment]))
        if not os.path.islink(path):
            self.m_area.append(segment)
            return

        link = os.readlink(path)
        if not os.path.isabs(link):
            link = os.path.join(os.path.dirname(path), link)
        rel = RelPath(link)
        self.m_area = rel.m_area
        self.m_path = rel.m_path + self.m_path

    def resolve_full(self):
        """effects: perform a full resolve of all symlinks in the path
        rationale: this should be used for resolving the area to which
        symlinks are constrained"""
        while len(self.m_path) > 0:
            self.resolve_one()

    def resolve_area(self, area):
        """effects: resolve the working area part of the path if we
        are in the working area, otherwise resolve the complete path
        returns: whether we are inside the working area
        warning: resolve_full needs to be called on area before
        rationale: this function should be called when trying to
        determine whether we are in the working area"""
        mylen = len(area.m_area)
        while len(self.m_area) < mylen or self.m_area[0:mylen] != area.m_area:
            if len(self.m_path) == 0:
                return False
            self.resolve_one()
        return True

    def resolve_relative(self, target):
        """effects: resolve this as the source directory from which we
        point to the target.  on success, it sets the relative member
        to be the relative path"
        returns: whether we have a relative path within the work area"""
        if not self.resolve_area(target):
            return False
        equal = 0
        iter = 0
        while iter < len(self.m_path):
            if (equal == iter and iter < len(target.m_path) and 
                self.m_path[iter] == target.m_path[iter]):
                iter = iter + 1
                equal = iter
            else:
                mypath = os.path.join(*(self.m_area + self.m_path[0:iter+1]))
                if os.path.islink(mypath):
                    link = os.readlink(mypath)
                    if not os.path.isabs(link):
                        link = os.path.join(os.path.dirname(mypath), link)
                    rel = RelPath(link)
                    if not rel.resolve_area(target):
                        return False
                    self.m_area = rel.m_area
                    self.m_path = rel.m_path
                    equal = 0
                    iter = 0
                else:
                    iter = iter + 1

        relative = []
        iter = equal
        while iter < len(self.m_path):
            relative.append("..")
            iter = iter + 1
        relative += target.m_path[equal:]
        if len(relative) > 0:
            self.m_relative = os.path.join(*relative)
        else:
            self.m_relative = "."
        return True


class WorkArea(object) :
    """description: this class provides a joint interface for all the
    information about the work area"""

    def __init__(self, RootCoreBin=None, RootCoreDir=None):
        self.m_area = None
        self.m_areaList = None
        self.m_packages = None
        self.m_arch = os.getenv("ROOTCORECONFIG")
        if not self.m_arch:
            self.m_arch = "generic"
            pass
        self.m_root_conf = None
        self.m_pedantic = None

        if RootCoreDir and not os.path.isabs(RootCoreDir):
            RootCoreDir = os.path.join(os.getcwd(), RootCoreDir)
        if RootCoreBin and not os.path.isabs(RootCoreBin):
            RootCoreBin = os.path.join(os.getcwd(), RootCoreBin)

        if RootCoreDir != None:
            self.m_dir = RootCoreDir
            if RootCoreBin != None:
                self.m_bin = RootCoreBin
            else:
                self.m_bin = self.m_dir
            self.m_obj = os.path.join(self.m_bin, "obj")
        elif RootCoreBin != None:
            self.m_bin = RootCoreBin
            self.m_config = self.m_bin + "/rootcore_config"
            self.m_dir = self.expandPath(get_field(self.m_config, "rootcoredir"))
            if not self.m_dir or self.m_dir == "":
                self.m_dir = RootCoreBin
            self.m_obj = self.expandPath(get_field(self.m_config, "rootcoreobj"))
            if not self.m_obj or self.m_obj == "":
                self.m_obj = os.path.join(RootCoreBin, "obj")

            p = os.path
            if (not p.isfile(p.join(self.m_dir, "internal", "workarea.py")) and
                not p.isfile(p.join(self.m_dir, "internal", "RootCore.py"))):
                raise RCError("invalid RootCore installation: " + self.m_dir)
        
        else:
            self.m_dir = os.getenv("ROOTCOREDIR")
            self.m_bin = os.getenv("ROOTCOREBIN")
            self.m_obj = os.getenv("ROOTCOREOBJ")
            if not all([self.m_dir, os.path.isabs(self.m_dir),
                        self.m_bin, os.path.isabs(self.m_bin),
                        self.m_obj, os.path.isabs(self.m_obj)]):
                raise RCError("environment not correctly setup please rerun RootCore setup")
        self.m_config = self.m_bin + "/rootcore_config"

        if not self.m_dir or not os.path.isabs(self.m_dir):
            raise RCError("failed to configure ROOTCOREDIR")
        if not self.m_bin or not os.path.isabs(self.m_bin):
            raise RCError("failed to configure ROOTCOREBIN")
        if not self.m_obj or not os.path.isabs(self.m_obj):
            raise RCError("failed to configure ROOTCOREOBJ")
        pass

    # TODO: use property decoration for all of these
    def dir(self):
        """description: $ROOTCOREDIR, i.e. the location of the actual
        RootCore package"""
        return self.m_dir

    def bin(self):
        """description: $ROOTCOREBIN, i.e. the location where RootCore
        puts all the files it generates.  this can be the same as or
        different from $ROOTCOREBIN"""
        return self.m_bin

    def obj(self):
        """description: $ROOTCOREOBJ.
        rationale: In practice this is the same as $ROOTCOREBIN/obj.
        users can point this somewhere else if they are low on disk
        space for the area where $ROOTCOREBIN is located.  By
        construction all large files are located inside $ROOTCOREOBJ,
        not $ROOTCOREBIN.  However, given that afs space is getting
        more generous this option has become less necessary/useful,
        and I am unsure if it still works"""
        return self.m_obj

    def arch(self):
        """description: $ROOTCORECONFIG
        rationale: when used with the releases this will in general
        have the same format as CMTCONFIG, but outside of the releases
        the user has the option to override this with whatever he sees fit
        rationale: for RootCore itself this is nothing but an
        arbitrary string (although it tries to set reasonable
        defaults).  the only packages that actively care about the
        value of this variable are the externals packages"""
        return self.m_arch

    def config(self):
        """description: the location of the config file
        rationale: this file contains all global RootCore
        configuration, i.e. independent of $ROOTCORECONFIG and the
        packages used"""
        return self.m_config

    def libdir(self) :
        """description: the location where the libraries get linked.
        actually they get linked in a subdirectory named after
        $ROOTCORECONFIG"""
        return os.path.join(self.bin(), "lib", self.arch())

    def bindir(self) :
        """description: the location where the binaries get linked.
        actually they get linked in a subdirectory named after
        $ROOTCORECONFIG"""
        return os.path.join(self.bin(), "bin", self.arch())

    def pedantic(self):
        """description: 2 for compiling all packages pedantic, 1 for
        compiling the packages pedantic that request it, and 0 for
        compiling no packages pedantic"""
        if self.m_pedantic == None:
            pedantic = get_field(self.config(), "use_pedantic")
            if pedantic == "2":
                self.m_pedantic = 2
            elif pedantic == "0":
                self.m_pedantic = 0
            else:
                self.m_pedantic = 1
        return self.m_pedantic

    def root_version(self):
        """description: the root version description string.

        rationale: this is mostly used to check that we call 'rc
        clean' whenever we change root version"""
        if not hasattr(self, "m_root_version"):
            try:
                root_version = shell_exec(["root", "-b", "-q"])
                index = root_version.find("\nROOT")
                if index == -1:
                    raise RCError("failed to read root version, please send me the output of\nroot -b -q")
                root_version = root_version[index+1:]
                index = root_version.find("\n")
                if index == -1:
                    raise RCError("failed to read root version, please send me the output of\nroot -b -q")
                self.m_root_version = root_version[0:index]
            except RCError:
                self.m_root_version = shell_exec(["root-config", "--version"]).rstrip()
        return self.m_root_version

    def root_conf(self):
        """effects: determine the root parameters used for compilation
        returns: the path to the configuration file generated
        failures: incompatible root installation"""
        if not self.m_root_conf:
            file = os.path.join(self.bin(), "root_config_" + self.arch())
            ROOTSYS = os.getenv("ROOTSYS")
            if ROOTSYS == None:
                if os.path.isfile(file):
                    ROOTSYS = get_field(file, "ROOTSYS")
                    if ROOTSYS and ROOTSYS != "":
                        raise RCError("no valid root version found, try setting up root using\n" +
                                      "  source " + ROOTSYS + "/bin/thisroot.sh")
                if os.path.isdir("/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase"):
                    raise RCError("no valid root version found, try setting up root using\n" +
                                  "  export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase\n" +
                                  "  source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh\n" +
                                  "  localSetupROOT")
                if shell_exec(["which", "root-config"], allowFail=True) != "":
                    raise RCError("no valid root version found, try setting up root using\n" +
                                  "  source `root-config --prefix`/bin/thisroot.sh")
                raise RCError("no valid root version found, please set up root")

            if os.path.isfile(file):
                myrootversion = get_field(file, "ROOT_VERSION")
                if not myrootversion:
                    set_field(file, "ROOT_VERSION", self.root_version())
                elif self.root_version() != myrootversion:
                    myrootsys = get_field(file, "ROOTSYS")
                    raise RCError("Root Version: " + self.root_version() + "\n" +
                                  "is not the same as previous version:\n" +
                                  "   " + myrootversion + "\n" +
                                  "either set up the correct root version e.g. via:\n" +
                                  "  source " + myrootsys + "/bin/thisroot.sh\n" +
                                  "or clean out the old object files via:\n" +
                                  "  rc clean")
                self.m_root_conf = file
                return file

            myarch = shell_exec(["root-config", "--etcdir"]) + "/Makefile.arch"
            if not os.path.isfile(myarch):
                myarch = os.getenv("ROOTSYS") + "/share/doc/root/test/Makefile.arch"
            if not os.path.isfile(myarch):
                shell_args = ["find", os.getenv("ROOTSYS") + "/.",
                              "-name", "Makefile.arch"]
                search = shell_exec(shell_args).strip().split("\n")
                if len(search) > 0:
                    myarch = search[0]
            if not os.path.isfile(myarch):
                raise RCError("failed to find Makefile.arch in " + os.getenv("ROOTSYS"))

            with open(file + "-", "w") as f:
                shell_args = ["make", "-f", self.dir() + "/Makefile-print",
                              "MAKEFILE_ARCH=" + myarch]
                rc = shell_exec(shell_args, stdout=f, returnRC=True)
            if rc != 0:
                raise RCError("could not determine compilation parameters")
            os.rename(file + "-", file)
            self.m_root_conf = file
        return self.m_root_conf

    def getCxx(self, user=False):
        """returns: the c++ compiler to be used"""
        return get_field(self.root_conf(), "CXX")

    def getLd(self, user=False):
        """returns: the c++ linker to be used"""
        return get_field(self.root_conf(), "LD")


    # TODO: use a property instead
    def area(self):
        """description: the base directory of the work area, i.e. the
        directory in which find_packages was called"""
        if not self.m_area:
            rel = get_field(self.m_config, "workareaRelative", default="")
            if os.path.isabs(rel):
                raise RCError("workareaRelative set to invalid value '" + \
                              rel + "' in " + self.m_config)
            self.m_area = self.bin()
            if rel:
                while rel != "" and rel != ".":
                    self.m_area = os.path.dirname(self.m_area)
                    rel = os.path.dirname(rel)
        return self.m_area

    def areaList(self):
        """description: the base directory of the work area as a
        normalized list
        rationale: normalized lists are used when determining relative
        paths"""
        if not self.m_areaList:
            rel = RelPath(self.area())
            rel.resolve_full()
            self.m_areaList = rel
        return self.m_areaList

    def expandPath(self, path, area=None) :
        """effects: expand a path found in the packages file
        returns: the expanded path
        rationale: this mostly takes care of converting between
        absolute paths and relative ones, so that the work area can be
        relocated"""
        if path == "" or path == None:
            return None
        if os.path.isabs(path) :
            return path
        if not area:
            area = self.area()
        return os.path.join(area, path)

    def compressPath(self, path, area=None) :
        """effects: compress a path for the packages file
        returns: the compressed path
        rationale: this mostly takes care of converting between
        absolute paths and relative ones, so that the work area can be
        relocated"""
        if not path or path == "":
            return ""
        if not area:
            area = self.area()
        return self.relPath(path, area, area)

    def relPath(self, file, dir = None, area = None) :
        """effects: calculate the path of file relative to dir, if
        they are both in the work area, resolving symbolic links as
        necessary.  if dir is not specified it is assumed to be the
        working area
        returns: the relative path, which may have ".." inside, if
        they don't correspond to symbolic links.  or the original
        path, if this is not a relative path inside the workarea"""
        if area != None:
            areaList = RelPath(area)
            areaList.resolve_full()
        else:
            areaList = self.areaList()
        fileList = RelPath(file)
        if not fileList.resolve_area(areaList):
            return file
        if dir:
            dirList = RelPath(dir)
            if not dirList.resolve_area(areaList):
                return file
        else:
            dirList = copy.deepcopy(areaList)

        if not dirList.resolve_relative(fileList):
            return file
        return dirList.m_relative

    def lnRel(self, source, target) :
        """effects: link source to target, making a relative link if
        that link would not leave the work area.  if the link already
        exists, nothing is done"""
        rel = self.relPath(source, os.path.dirname(target))
        if os.path.islink(target):
            link = os.readlink(target)
            if link == rel:
                return
            os.remove(target)
        elif os.path.exists(target):
            # raise RCError("can not replace non-link with link: " + target)
            os.remove(target)
        mkdir_p(os.path.dirname(target))
        os.symlink(rel, target)

    def isMac(self) :
        """returns: whether this is a mac architecture
        rationale: this is used in various places in the build
        procedure where MacOS behaves differently from Linux"""
        if not hasattr(self, "m_isMac") :
            if self.m_arch.find("-mac") != -1 :
                self.m_isMac = True
            elif self.m_arch.find("-slc") != -1 :
                self.m_isMac = False
            else :
                self.m_isMac =(shell_exec(["uname"]) == "Darwin")
        return self.m_isMac

    def hasPackages(self) :
        """returns: whether we already have a packages file,
        i.e. whether find_packages has already been called"""
        return self.m_packages or os.path.exists(os.path.join(self.bin(),
                                                              "packages"))

    def packages(self):
        """description: the list of packages we are using"""
        if not self.m_packages:
            self.m_packages = self.parsePackages()
        return self.m_packages

    def parsePackages(self):
        """returns: the list of all packages, as found by find_packages.
        if packages is specified it is used instead of the find_packages
        file."""
        path = os.path.join(self.bin(), "packages")
        if not os.path.isfile(path):
            raise RCError("could not find:\n  " + path +
                           "\nplease try running:\n  rc find_packages")
        result = PackageList(self)
        with open(self.bin() + "/packages") as file:
            for line in file:
                line = string.replace(line, "\n", "")
                if line != "" and line[0] != "#":
                    result.addPkg(PackageInfo.parsePackagesLine(line, self))
        return result

    def writePackages(self, packages):
        """effects: writes the content of the package list into a new
        package file"""
        with open(self.bin() + "/packages", "w") as file:
            for pkg in packages:
                pkg.writePackagesLine(file)

    def write_config(self) :
        """effects: write the configuration for the workarea as it is
        configured now"""
        if not os.path.isfile(self.config()):
            open(self.config(), "w").close()
        set_field(self.config(), "workareaRelative",
                  self.relPath(self.area(), self.bin()))
        set_field(self.config(), "rootcoredir", self.compressPath(self.dir()))
        set_field(self.config(), "rootcoreobj", self.compressPath(self.obj()))
        set_field(self.config(), "rootcoreconf", self.arch())
        self.lnRel(os.path.join(self.dir(), "internal", "local_setup.sh"),
                   os.path.join(self.bin(), "local_setup.sh"))
        self.lnRel(os.path.join(self.dir(), "internal", "local_setup.csh"),
                   os.path.join(self.bin(), "local_setup.csh"))

    def ln_rc(self) :
        """effects: create the link for rc"""
        source = os.path.join(self.dir(), "internal", "rc")
        target = os.path.join(self.bin(), self.bin(), "bin", self.arch(), "rc")
        self.lnRel(source, target)

    def ut_test_list(self,package,recursive):
        """returns: the list of tests we have"""
        result = []
        if package :
            pkg = self.packages().getPkg(package)
            if not pkg :
                raise RCError ("unknown package: " + package)
            packages = [pkg]
            pass
        else :
            packages = self.packages()
            pass
        for pkg in packages :
            result += pkg.ut_test_list (recursive)
            pass
        return result

    # NOTE: might be best to keep the option parsing in with
    # the top-level commands. Is it possible?
    @classmethod
    def init_release_options(cls, parser, accu):
        """effects: initialize the release options for the
        OptionParser, i.e. all the options that actually select the
        release as passed into find_packages and set_release"""

        def release_option(option, opt_str, value, parser):
            release = getattr(parser.values, option.dest)
            release.append(opt_str)
            if value != None:
                release += [value]
        def base_release_option(option, opt_str, value, parser, args):
            getattr(parser.values, option.dest).append([value, option.callback_args[0]])
        if accu:
            parser.add_option("--src-release", dest="release", default=[],
                              type="string", action="callback",
                              callback=release_option)
            parser.add_option("--obj-release", dest="release",default=[],
                              type="string", action="callback",
                              callback=release_option)
            parser.add_option("--restrict", dest="release", default=[],
                              type="string", action="callback",
                              callback=release_option)
            parser.add_option("--drop-restrict", dest="release", default=[],
                              action="callback", callback=release_option)
            parser.add_option("--no-pedantic", dest="release", default=[],
                              type="string", action="callback",
                              callback=release_option)
            parser.add_option("--force-pedantic", dest="release", default=[],
                              type="string", action="callback",
                              callback=release_option)
        else:
            parser.add_option("--src-release", dest="places", default=[],
                              type="string", action="callback",
                              callback=base_release_option,
                              callback_args=tuple([True]))
            parser.add_option("--obj-release", dest="places",default=[],
                              type="string", action="callback",
                              callback=base_release_option,
                              callback_args=tuple([False]))
            parser.add_option("--restrict", dest="restrict", default=[],
                              action="append")
            parser.add_option("--drop-restrict", dest="restrict", default=[],
                              action="store_const", const=[])
            parser.add_option("--no-pedantic", dest="use_pedantic", default=1,
                              action="store_const", const=0)
            parser.add_option("--force-pedantic", dest="use_pedantic",
                              default=1, action="store_const", const=2)

    def find_packages_release(self, release):
        """effects: parse the release options specified to
        find_packages and pick up the packages from the base
        release"""
        myrelease=get_field(self.bin() + "/rootcore_config", "release")
        if myrelease:
            print "using release set with set_release"
            release = string.split(myrelease) + release

        rel_parser = optparse.OptionParser()
        WorkArea.init_release_options(rel_parser, False)
        (release_args, extra_release_args) = rel_parser.parse_args(release)
        if len(extra_release_args) != 0:
            rel_parser.error("incorrect number of arguments")

        WORKAREAREL = self.relPath(self.area(), self.bin())
        if os.path.isabs(WORKAREAREL):
            print "work area does not contain ROOTCOREBIN, aborting"
            print "  ROOTCOREBIN=" + self.bin()
            print "  WORKAREA=" + self.m_area
            print "  WORKAREAREL=" + WORKAREAREL
            sys.exit(1)

        self.m_packages = PackageList(self)
        for place in release_args.places:
            for pkg in WorkArea(RootCoreBin=place[0]).parsePackages():
                pkg.m_wa = self
                pkg.m_recompile = place[1]
                pkg.m_release = True
                self.m_packages.addPkg(pkg)
        self.m_pedantic = release_args.use_pedantic
        return release_args.restrict

    def find_packages_find(self, area, allowNesting):
        """effects: search for packages in the work area"""
        for filename in os.listdir(area):
            dir = os.path.join(area, filename)
            is_package = False
            if os.path.exists(os.path.join(dir, "cmt", "Makefile.RootCore")):
                is_package = True
                name=os.path.basename(dir)
                name2=get_field(dir + "/cmt/Makefile.RootCore", "PACKAGE")
                if not name2:
                    raise Exception("failed to read package name from " +
                                    dir + "/cmt/Makefile.RootCore")
                if name != name2:
                    #raise Exception("package " + dir + " should have name " + name2 + "\n" +
                    #                "please rename it to " + os.path.dirname(dir) + "/" + name2 + " by typing\n" +
                    #                "  mv '" + dir + "' '" + os.path.dirname(dir) + "/" + name2 + "'")
                    dirname = os.path.dirname(dir) + "/" + name2
                    exstr = "package %s should have name %s\n" % (dir, name2)
                    exstr += "please rename it to %s by typing\n" % dirname
                    exstr += "  mv '%s' '%s'" % (dir, dirname)
                    raise Exception(exstr)
                pkg2 = self.m_packages.getPkg(name)
                if pkg2 and pkg2.release() == 0:
                    raise Exception("duplicate packages, please remove one of them:\n" +
                                    "  " + dir + "\n" +
                                    "  " + pkg2.srcdir())

                pkg = PackageInfo(self)
                pkg.m_name = name
                pkg.m_srcdir = dir
                pkg.m_release = False
                pkg.m_recompile = True
                self.m_packages.addPkg(pkg)
            if((not is_package or allowNesting) and os.path.isdir(dir) and
                (area != self.bin() or
                 (filename != "RootCore" and filename != "bin"          and
                  filename != "data"     and filename != "include"      and
                  filename != "lib"      and filename != "obj"          and 
                  filename != "python"   and filename != "user_scripts" ))):
                self.find_packages_find(dir, allowNesting)

    def find_packages_reorder(self, allowMiss, restrict):
        """effects: do the reordering of the packages based on
        dependencies for find_packages"""
        def addPkgDep(self, name, required, caller, cycle, catalog, allowMiss, wa):
            if self.getPkg(name):
                return
            for pkg in cycle:
                if name == pkg:
                    raise Exception("cyclical dependence involving packages: " +
                                    string.join(cycle))
            pkg = catalog.getPkg(name)
            if not pkg:
                if allowMiss or not required:
                    return
                message = ""
                if caller:
                    message = "package " + name + " not known, required by " + caller
                else:
                    message = "package " + name + " not known"
                shell_args = ["grep", "/" + name + "/tags", wa.dir() + "/all_packages"]
                known = shell_exec(shell_args, allowFail=True).split("\n")
                while len(known) > 0 and known[len(known) - 1] == "":
                    known.remove("")
                if len(known) > 0:
                    message += "\ntry checking it out with"
                    for pkg in known:
                        message += "\n  rc checkout_pkg " + pkg
                raise RCError(message)
            for dep in pkg.harddep():
                addPkgDep(self, dep, True, name, cycle + [name], catalog, allowMiss, wa)
            for dep in pkg.trydep():
                addPkgDep(self, dep, False, name, cycle + [name], catalog, allowMiss, wa)
            self.addPkg(pkg)

        ordered = PackageList(self)
        max_auto = 1
        while max_auto > 0:
            max_auto = 0
            for pkg in self.m_packages:
                if not ordered.getPkg(pkg.name()):
                    if pkg.auto() > max_auto:
                        max_auto = pkg.auto()
            if max_auto == 0:
                break
            for pkg in self.m_packages:
                if pkg.auto() == max_auto:
                    addPkgDep(ordered, pkg.name(), True, None, [],
                              self.m_packages, allowMiss, self)

        if len(restrict) > 0:
            for pkg in restrict:
                addPkgDep(ordered, pkg, True, None, [],
                          self.m_packages, allowMiss, self)
        else:
            for pkg in self.m_packages:
                addPkgDep(ordered, pkg.name(), True, None, [],
                          self.m_packages, allowMiss, self)
        self.m_packages = ordered

    def find_packages_dep_rebuild(self):
        """effects: set the determined dependencies and the rebuild
        flags for the find_packages operation"""
        myauto = []
        for pkg in self.m_packages:
            pkg.m_dep = []
            for dep in myauto :
                pkg.m_dep.append(dep)
            for dep in pkg.harddep() + pkg.trydep():
                pkg2 = self.m_packages.getPkg(dep)
                if pkg2:
                    for mydep in pkg2.dep():
                        if not mydep in pkg.m_dep:
                            pkg.m_dep.append(mydep)
                    if pkg2.recompile():
                        pkg.m_recompile = True
            pkg.m_dep.append(pkg.name())
            if pkg.auto() > 0 and pkg.name() != "RootCore" :
                for dep in pkg.m_dep :
                    if not dep in myauto :
                        myauto = myauto + [dep]
            if pkg.name() == "RootCore" or pkg.recompile() == True:
                pkg.m_outdir = self.obj()

    def find_packages_write(self):
        """effects: write out the result of the find_packages
        operation"""
        RootCore = self.m_packages.getPkg("RootCore")
        if RootCore.srcdir() != self.dir():
            print "WARNING: RootCore installation changed, please setup again in a new shell"
            self.m_dir = RootCore.srcdir()

        CXXFLAGS = ""
        LDFLAGS = ""
        if os.getenv("CXXFLAGS"):
            CXXFLAGS = os.getenv("CXXFLAGS")
        if os.getenv("LDFLAGS"):
            LDFLAGS = os.getenv("LDFLAGS")
        if os.getenv("EXTRA_CFLAGS"):
            CXXFLAGS += " " + os.getenv("EXTRA_CFLAGS")
        if os.getenv("CPPEXFLAGS"):
            CXXFLAGS += " " + os.getenv("CPPEXFLAGS")
            LDFLAGS += " " + os.getenv("CPPEXFLAGS")
        set_field(self.config(), "CXXFLAGS", CXXFLAGS)
        set_field(self.config(), "LDFLAGS", LDFLAGS)
        set_field(self.config(), "use_pedantic", str(self.m_pedantic))
        self.write_config()
        self.writePackages(self.m_packages)

    def find_packages(self, release, allowMiss, allowNesting):
        """effects: perform the find_packages command"""

        restrict = self.find_packages_release(release)

        print "looking for packages in " + self.m_area
        self.find_packages_find(self.m_area, allowNesting)

        self.m_packages.sortByName()
        print
        print "packages found:"
        for pkg in self.m_packages:
            print pkg.srcdir()

        self.find_packages_reorder(allowMiss, restrict)

        print
        print "sorted packages:"
        for pkg in self.m_packages:
            print pkg.srcdir()
        self.find_packages_dep_rebuild()
        # rationale: I am setting the recompile flag for RootCore,
        # because it's outdir always points to the current output
        # directory, even if it comes from the release.  I'm setting
        # this here, to avoid accidentally recompiling everything
        # depending on RootCore.
        self.m_packages.getPkg("RootCore").m_recompile = True
        print
        print "writing changes"
        self.find_packages_write()
        print "done"

    def compileCore(self, packages, log_files, cont) :
        """effects: do the core part of 'rc compile', i.e. compile all
        the packages"""

        failures = []
        for pkg in packages:
            log_file = None
            if log_files:
                path = pkg.nicos_name() + ".loglog"
                logdir = os.getenv("ROOTCORELOG")
                if logdir:
                    path = os.path.join(logdir, path)
                    pass
                print "writing " + pkg.name() + " compilation log to " + path
                log_file = open(path, "w")
            try:
                if not pkg.compile(ignoreErrors = cont, logFile = log_file):
                    raise RCError("failed to compile : " + pkg.name())
            except Exception as e:
                failures.append(pkg.name())
                if not cont:
                    raise
            finally:
                if log_file:
                    log_file.close()
                    pass
                pass
            pass

        if len(failures) > 0:
            raise RCError("compilation failures in: " + string.join(failures, " "))
        pass
    pass
