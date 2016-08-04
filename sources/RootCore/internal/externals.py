import os
import re
import fnmatch

# Local imports
from utils import RCError, get_field, get_field_env, shell_exec, expand_env, mkdir_p

class ExternalPackage:
    """description: this class holds all the global information for an
    external package"""

    # TODO: get rid of the "m_*" naming, and use
    # class properties where necessary
    def __init__ (self, wa):
        self.m_wa = wa
        self.m_pkgname = get_field ("Makefile.RootCore", "PACKAGE")
        self.m_rcpkg = wa.packages().getPkg (self.m_pkgname)
        self.m_work_area = os.getenv ("ROOTCORECMT")
        if not self.m_work_area:
            raise RCError ("ROOTCORECMT not set, required for external package")
        self.m_conf_file = os.path.join (self.m_work_area, "configured")
        self.m_options_file = None
        if os.path.exists (self.m_conf_file):
            field = get_field (self.m_conf_file, "options_file", empty=None)
            self.m_options_file = self.m_wa.expandPath(field)
            #self.m_options_file = self.m_wa.expandPath(get_field(self.m_conf_file, "options_file", empty=None))
            pass
        if not self.m_options_file:
            self.m_options_file = os.path.join (os.getcwd(), "options")
            pass

        self.m_binname = None
        self.m_incname = None
        self.m_libname = None
        pass

    def check_configuration (self):
        """returns: whether the configuration is current"""
        if not os.path.isfile (self.m_conf_file):
            return False
        time = os.stat (self.m_conf_file).st_mtime
        return (time > os.stat ("precompile.RootCore").st_mtime or
                time > os.stat ("install").st_mtime or
                time > os.stat ("options").st_mtime or
                time > os.stat (self.m_options_file).st_mtime)

    def clear_configuration (self):
        """effects: clear out the old configuration, so that it can be
        remade"""
        if os.path.exists (self.m_conf_file):
            os.remove (self.m_conf_file)
            pass
        self.m_options_file = os.path.join (os.getcwd(), "options")
        pass

    def find_options (self):
        """effects: find the right options file to use"""
        self.m_options_file = None
        #for alternate in string.split (get_field_env ("options", "alternates"), " "):
        for alternate in get_field_env ("options", "alternates").split():
            if os.path.exists (alternate):
                self.m_options_file = self.w_ma.expandPath (alternate)
                return
            pass
        if not self.m_options_file:
            self.m_options_file = os.path.join (os.getcwd(), "options")
            pass
        pass

    def read_options (self):
        """effects: read the fields from the options file"""
        self.m_name = get_field (self.m_options_file, "name",
                                 default=self.m_pkgname, empty=self.m_pkgname)
        self.m_url = get_field (self.m_options_file, "url", default="").split()
        self.m_md5sum = get_field (self.m_options_file, "md5sum",
                                   default=None, empty=None)
        self.m_srcdir = get_field (self.m_options_file, "src_dir", default="")
        self.m_target = os.path.join (os.getenv ("ROOTCORECMT"), "local")
        self.m_binname = get_field (self.m_options_file, "binname",
                                    default="").split()
        self.m_incname = get_field (self.m_options_file, "incname",
                                    default="").split()
        self.m_libname = get_field (self.m_options_file, "libname",
                                    default="").split()

        force_local = get_field (self.m_options_file, "force_local",
                                 default="no", empty="no")
        allow_global = get_field (self.m_options_file, "allow_global",
                                  default="no", empty="no")
        self.m_siteroot = get_field (self.m_options_file, "siteroot", empty=None)

        if allow_global == "yes":
            self.m_allow_global = True
        elif allow_global == "no":
            self.m_allow_global = False
        else:
            raise RCError ("invalid value " + allow_global +
                           " for option allow_global")

        if force_local == "yes":
            self.m_force_local = True
        elif force_local == "no":
            self.m_force_local = False
        else:
            raise RCError ("invalid value " + force_local +
                           " for option force_local")
        pass

    def complete_locations (self):
        """effects: add in all the locations from global, site-root
        and local installation"""

        if self.m_force_local:
            self.m_locations = [ExternalLocationLocal (self)]
            return

        self.m_locations = []

        if self.m_allow_global:
            self.m_locations.append (ExternalLocationGlobal (self))
            pass

        if self.m_siteroot:
            SITEROOT = os.getenv ("SITEROOT")
            if SITEROOT:
                shell_args = ["find", os.path.join (SITEROOT, self.m_siteroot),
                              "-type", "d", "-name", "lib"]
                for libdir in shell_exec (shell_args).split("\n"):
                    if libdir.find (os.getenv("CMTCONFIG")) != -1:
                        print "found via SITEROOT: " + libdir
                        maindir = os.path.dirname (libdir)
                        incdir = os.path.join (maindir, "include")
                        loc = incdir + ":" + libdir
                        self.m_locations.append (ExternalLocation(self, loc))
                        pass
                    pass
                pass
            pass

        #for location in string.split (get_field (self.m_options_file, "locations", default=""), " "):
        for location in get_field (self.m_options_file, "locations",
                                   default="").split():
            location = expand_env (location)
            if location and location != "":
                self.m_locations.append (ExternalLocation (self, location))
                pass
            pass

        if os.path.exists ("install"):
            self.m_locations.append (ExternalLocationLocal (self))
            pass
        pass

    def read_location (self):
        result = ExternalLocation (self, None)
        if not os.path.exists (self.m_conf_file):
            raise RCError ("failed to read external location, rerun compile")
        expandPath = self.m_wa.expandPath
        result.m_bindir = expandPath (get_field (self.m_conf_file, "bindir"),
                                      self.m_rcpkg.outdir())
        result.m_libdir = expandPath (get_field (self.m_conf_file, "libdir"),
                                      self.m_rcpkg.outdir())
        result.m_incdir = expandPath (get_field (self.m_conf_file, "incdir"),
                                      self.m_rcpkg.outdir())
        if not result.m_bindir and not result.m_libdir and not result.m_incdir:
            result = ExternalLocationGlobal (self)
            pass
        self.m_binname = get_field (self.m_conf_file, "binname").split()
        self.m_libname = get_field (self.m_conf_file, "libname").split()
        self.m_incname = get_field (self.m_conf_file, "incname").split()
        #self.m_binname = string.split (get_field (self.m_conf_file, "binname"))
        #self.m_libname = string.split (get_field (self.m_conf_file, "libname"))
        #self.m_incname = string.split (get_field (self.m_conf_file, "incname"))
        return result
    pass


class ExternalLocation:
    """description: this class incorporates all the information needed
    about one particular location that may be used for an external
    package"""

    def __init__ (self, package, location):
        self.m_package = package
        self.m_bindir = None
        self.m_incdir = None
        self.m_libdir = None

        if location:
            mylocation = location.split(":")
            #mylocation = string.split (location, ":")
            if len (mylocation) == 1:
                self.m_bindir = os.path.join (mylocation[0], "bin")
                self.m_incdir = os.path.join (mylocation[0], "include")
                self.m_libdir = os.path.join (mylocation[0], "lib")
                pass
            elif len (mylocation) == 2:
                self.m_bindir = ""
                self.m_incdir = mylocation[0]
                self.m_libdir = mylocation[1]
                pass
            elif len (mylocation) == 3:
                self.m_bindir = mylocation[2]
                self.m_incdir = mylocation[0]
                self.m_libdir = mylocation[1]
                pass
            else:
                raise RCError ("invalid location format: " + location)
            pass
        pass

    def __str__ (self):
        if (self.m_bindir and self.m_incdir and self.m_libdir and
            os.path.dirname (self.m_bindir) == os.path.dirname (self.m_libdir) and
            os.path.dirname (self.m_bindir) == os.path.dirname (self.m_incdir)):
            return os.path.dirname (self.m_bindir)
        result = [self.m_incdir, self.m_libdir, self.m_bindir]
        while len(result) > 0 and result[len(result)-1] == None:
            result = result[0:len(result)-1]
            pass
        if len (result) > 0:
            return ":".join (result)
            #return string.join (result, ":")
        return "NULL"

    def write_conf (self):
        pkg = self.m_package
        mkdir_p (os.path.dirname (pkg.m_conf_file))
        with open (pkg.m_conf_file, "w") as f:
            compressPath = pkg.m_wa.compressPath
            outdir = pkg.m_rcpkg.outdir()
            f.write ("options_file = " +
                     compressPath (pkg.m_options_file, outdir) + "\n")
            f.write ("bindir = " + compressPath (self.m_bindir, outdir) + "\n")
            f.write ("incdir = " + compressPath (self.m_incdir, outdir) + "\n")
            f.write ("libdir = " + compressPath (self.m_libdir, outdir) + "\n")
            f.write ("binname = " + " ".join (pkg.m_binname) + "\n")
            f.write ("incname = " + " ".join (pkg.m_incname) + "\n")
            f.write ("libname = " + " ".join (pkg.m_libname) + "\n")
            #f.write ("binname = " + string.join (pkg.m_binname, " ") + "\n")
            #f.write ("incname = " + string.join (pkg.m_incname, " ") + "\n")
            #f.write ("libname = " + string.join (pkg.m_libname, " ") + "\n")
            pass
        print "using " + pkg.m_name + " installation"
        print open (pkg.m_conf_file, "r").read()
        pass

    def install_test (self) :
        self.test_location()
        self.test_script()
        self.test_compile()
        pass

    # TODO: 2D list needs explanation
    def directories (self):
        pkg = self.m_package
        return [[self.m_incdir, pkg.m_incname, "",
                 os.path.join (pkg.m_wa.bin(), "include"), "include"],
                [self.m_libdir, pkg.m_libname, "*",
                 os.path.join (pkg.m_wa.bin(), "lib", pkg.m_wa.arch()), "lib"],
                [self.m_bindir, pkg.m_binname, "",
                 os.path.join (pkg.m_wa.bin(), "bin", pkg.m_wa.arch()), "bin"]]

    def test_location (self) :
        for area in self.directories():
            if len (area[1]) > 0:
                if not area[0]:
                    raise RCError ("no " + area[4] + " directory defined")
                if not os.path.isdir (area[0]):
                    raise RCError ("directory not found: " + area[0])
                for pattern in area[1]:
                    found = False
                    for file in os.listdir (area[0]):
                        pat2 = pattern + area[2]
                        if not found and fnmatch.fnmatch (file, pat2):
                            found = True
                            pass
                        pass
                    if not found:
                        raise RCError ("file + " + pattern + " not found in " +
                                       area[0])
                    pass
                pass
            pass
        pass

    def test_script (self) :
        if os.path.isfile ("test_file.sh"):
            if shell_exec (["./test_file.sh"], returnRC=True) != 0:
                raise RCError ("test script failed")
            pass
        pass

    def test_compile (self) :
        if os.path.isfile ("test_file.C"):
            with open ("test_file.C") as f:
                env = {}
                if self.m_incdir:
                    env["ROOTCORETEST_CXXFLAGS"] = "-I" + self.m_incdir
                    pass
                if shell_exec (["rc", "test_cc", "compile"],
                               stdin=f, newEnv=env, returnRC=True) != 0:
                    raise RCError ("test compilation failed")
                pass
            pass
        pass

    def link_package (self):
        expr_dylib = re.compile ("^(.*)\\.dylib$")
        expr_numbered = re.compile ("^([^-]*)-([0-9.]*)(\\.[a-z]*)$")
        for area in self.directories():
            if len (area[1]) > 0:
                if not area[0]:
                    raise RCError ("missing " + area[4] +
                                   " directory for package " +
                                   self.m_package.m_pkgname)
                for pattern in area[1]:
                    for file in os.listdir (area[0]):
                        if fnmatch.fnmatch (file, pattern + area[2]):
                            source = os.path.join (area[0], file)
                            target = os.path.join (area[3], file)
                            self.m_package.m_wa.lnRel (source, target)
                            match = expr_dylib.match (file)
                            if match:
                                # NOTE: very deep nesting. Should rewrite.
                                source = target
                                file = match.group(1) + ".so"
                                target = os.path.join (area[3], file)
                                self.m_package.m_wa.lnRel (source, target)
                                pass
                            match = expr_numbered.match (file)
                            if match and len (match.group(1)) > 0:
                                source = target
                                file = match.group(1) + match.group(3)
                                target = os.path.join (area[3], file)
                                self.m_package.m_wa.lnRel (source, target)
                                pass
                            pass
                        pass
                    pass
                pass
            pass
        pass
    pass

class ExternalLocationGlobal (ExternalLocation) :
    def __init__ (self, package):
        ExternalLocation.__init__ (self, package, None)
        pass

    def __str__ (self):
        return "%%%GLOBAL%%%"

    def directories (self):
        return []
    pass

class ExternalLocationLocal (ExternalLocation) :
    def __init__ (self, package):
        ExternalLocation.__init__ (self, package,
                                   os.path.join (package.m_work_area, "local"))
        pass

    def __str__ (self):
        return "%%%LOCAL%%%"

    def install_test (self) :
        try:
            ExternalLocation.install_test (self)
            return
        except:
            pass

        print "trying manual install"
        os.environ["options_file"] = self.m_package.m_options_file
        if shell_exec (["./install"],returnRC=True) != 0:
            raise RCError ("installation of " + self.m_package.m_pkgname +
                           " failed")
        self.link_package ()
        ExternalLocation.install_test (self)
    pass
