import os
import fnmatch
#import string

from utils import RCError

class MakefileOutput (object) :
    """description: this class manages the output Makefile I am
    creating

    rationale: this is organized this way, so that it can track extra
    information, like the list of all dependency files"""

    def __init__ (self, file) :
        self.m_file = file
        self.m_targets = {}
        self.m_outputs = {}
        self.m_deplist = {}
        pass

    def checkOutput (self, output) :
        """returns: whether this output file still has to be added for this
        source

        rationale: this is used by dependency rules which have to do actions"""
        if output in self.m_outputs :
            return False
        self.m_outputs[output] = ""
        return True

    def checkAddOutput (self, output) :
        """effects: check whether the output file, and add it if necessary
        returns: whether this output file still has to be added for this
        source
        rationale: this is used to avoid descending into rules more
        than once"""
        if output in self.m_outputs :
            return False
        self.m_outputs[output] = ""
        return True

    def checkAddTarget (self, target, source) :
        """effects: check whether the target has already been added
        with this source, and add it if necessary
        returns: whether this target still has to be added for this
        source
        rationale: this is used to avoid adding a pattern based rule
        multiple times"""
        id = target + ":" + source
        if id in self.m_targets :
            return False
        self.m_targets[id] = ""
        return True

    def write (self, str) :
        """effects: write the given text to the file"""
        self.m_file.write (str)
        pass

    def addDep (self, dep) :
        """effects: add a dependency file.  also check that the file
        is valid if it exists, otherwise remove it."""
        if dep in self.m_deplist :
            return
        self.m_deplist[dep] = ""

        if os.path.exists (dep) :
            with open (dep, "rt") as myfile :
                content = myfile.read()
                content = content[content.find (":") + 1 :]
                #for myfile in string.split (content) :
                for myfile in content.split() :
                    if not os.path.exists (myfile) :
                        os.remove (dep)
                        return
                    pass
                pass
            pass
        pass
    pass


class MakefileRule (object) :
    """description: this is the base class for all classes that
    represent makefile rules"""

    def __init__ (self, output, input=None) :
        if not isinstance (output, basestring):
            raise Exception ("output field must be string")
        self.m_output = output
        self.m_target = output
        self.m_prereq = []
        self.m_order_prereq = []
        if input:
            self.m_input  = input
            self.m_source = input.output()
            pass
        else:
            self.m_source = ""
            pass
        pass

    def output (self) :
        """returns: the name of the output file generated"""
        return self.m_output

    def linkFlags (self) :
        """returns: the flags needed to link this file"""
        if not hasattr (self, "m_linkFlags") :
            basename = os.path.basename (self.output())
            if basename[0:3] == "lib" and basename[-3:] == ".so" :
                self.m_linkFlags = ["-L" + os.path.dirname (self.output()),
                                    "-l" + basename[3:-3]]
                pass
            else:
                self.m_linkFlags = [self.output()]
                pass
            pass
        return self.m_linkFlags

    def addPrereq (self, prereq) :
        """effects: add a regular prerequisite for this rule"""
        if not isinstance (prereq, MakefileRule):
            raise Exception ("prerequisite must be a rule")
        self.m_prereq.append (prereq)
        pass

    def addOrderPrereq (self, prereq) :
        """effects: add an order only prerequisite for this rule"""
        if not isinstance (prereq, MakefileRule):
            raise Exception ("prerequisite must be a rule")
        self.m_order_prereq.append (prereq)
        pass

    def write (self, file) :
        """effects: add this rule to the file, if it doesn't already
        exist.  also add all the dependent rules as needed.  calls
        writeCommands internally to write the actual commands"""
        if not file.checkAddOutput (self.m_output) :
            return

        if file.checkAddTarget (self.m_target, self.m_source) :
            file.write ("\n")
            file.write (self.m_target + " : " + self.m_source)
            for prereq in self.m_prereq :
                file.write (" " + prereq.m_output)
                pass
            if len (self.m_order_prereq) > 0 :
                file.write (" |")
                for prereq in self.m_order_prereq :
                    file.write (" " + prereq.m_output)
                    pass
                pass
            file.write ("\n")
            self.writeCommands (file)
            pass

        if hasattr (self, "m_input") :
            self.m_input.write (file)
            pass
        for prereq in self.m_prereq + self.m_order_prereq:
            prereq.write (file)
            pass
        pass

    def compactify (self, targetSuffix, sourceSuffix) :
        """effects: reduce this rule into a generic rule, i.e. one
        that uses % instead of an explicit file name (if possible)"""
        name1 = os.path.basename (self.m_target)
        split1 = name1.rfind (targetSuffix)
        name2 = os.path.basename (self.m_source)
        split2 = name2.rfind (sourceSuffix)
        if (split1 == len (name1) - len (targetSuffix) and
            split2 == len (name2) - len (sourceSuffix) and
            name1[0:split1] == name2[0:split2]) :
            self.m_target = os.path.join (os.path.dirname (self.m_target),
                                          "%" + targetSuffix)
            self.m_source = os.path.join (os.path.dirname (self.m_source),
                                          "%" + sourceSuffix)
            pass
        pass
    pass


class MakefileInput (MakefileRule) :
    """description: a dummy rule for files that are input files
    rationale: this is so that I can pass input files in lists of
    Makefile rules without problems"""

    def __init__ (self, filename) :
        super (MakefileInput, self).__init__ (filename)
        pass

    def write (self, file) :
        pass
    pass


class MakefileDirectory (MakefileRule) :
    """description: a rule for creating directories
    rationale: this allows to create necessary directories, just by
    depending on this rule (naturally as an order dependency)"""

    def __init__ (self, dirname) :
        super (MakefileDirectory, self).__init__ (dirname)
        pass

    def writeCommands (self, file) :
        file.write ("\t$(SILENT)echo Making directory $@\n")
        file.write ("\t$(SILENT)mkdir -p $@\n")
        pass
    pass


class MakefileDependency (MakefileRule) :
    """description: a makefile rule for a dependency file"""

    def __init__ (self, master, output, source, extraDepFlags) :

        if not output:
            output = os.path.basename (source)
            output = output[:output.rfind (".")] + ".d"
            output = os.path.join (master.objdir(), output)
            pass

        super (MakefileDependency, self).__init__ (output, source)

        self.m_objdir = os.path.dirname (output)
        self.addOrderPrereq (MakefileDirectory (self.m_objdir))

        self.m_extraDepFlags = extraDepFlags
        self.compactify (".d", ".cxx")
        pass

    def write (self, file) :
        file.addDep (self.output())
        super (MakefileDependency, self).write (file)
        pass

    def writeCommands (self, file) :
        file.write ("\t$(SILENT)echo Making dependency for `basename $<`\n")
        file.write ("\t$(SILENT)rc --internal make_dep " +
                    "$(RC_CXX) $(RC_CXXFLAGS) $(INCLUDES) " +
                    " ".join (self.m_extraDepFlags) + " -- $@ $< \n")
        pass
    pass


class MakefileObject (MakefileRule) :
    """description: the base class for rules that generate object files
    rationale: there is a whole hierarchy of rules here, since
    dictionaries need to run a code-generator first"""

    def __init__ (self, master, source, cxxfile) :
        if cxxfile :
            output = os.path.basename (cxxfile)
            self.m_cxxfile = os.path.join (master.objdir(), cxxfile)
            pass
        else:
            output = os.path.basename (source.output())
            self.m_cxxfile = "$<"
            pass
        output = output[0:output.rfind(".")] + ".o"
        output = os.path.join (master.objdir(), output)

        super (MakefileObject, self).__init__ (output, source)

        self.m_pkgname = master.pkgname()
        self.m_depfile = output[0:output.rfind(".")] + ".d"
        deprule = MakefileDependency (master, self.m_depfile, source, [])
        deprule.addOrderPrereq (MakefileDirectory (master.objdir()))
        self.addOrderPrereq (deprule)
        # rationale: I'm not adding the directory to my own rule, as I
        # already added it to the base rule

        self.m_depfile = deprule.output()
        pass

    def writeCommands (self, file) :
        file.write ("\t$(SILENT)echo Compiling `basename $@`\n")
        file.write ("\t$(SILENT)rc --internal check_dep_cc " + self.m_pkgname + " $@\n")
        self.writeCommandsCxx (file)
        file.write ("\t$(SILENT)$(RC_CXX) " + self.m_flags +
                    " $(INCLUDES) -c " + self.m_cxxfile + " -o $@\n")
        pass
    pass


class MakefileObjectSource (MakefileObject) :
    """description: a class for rules that generate object files from
    regular source files"""
    def __init__ (self, master, input) :
        super (MakefileObjectSource, self).__init__ (master, input, None)

        self.m_flags = "$(RC_CXXFLAGS)"
        self.compactify (".o", ".cxx")
        pass

    def writeCommandsCxx (self, file) :
        pass
    pass


class MakefileObjectCint (MakefileObject) :
    """description: a class for rules that generate an object file for
    the CINT dictionary"""
    def __init__ (self, master, input) :
        cxxfile = master.pkgname() + "CINT.cxx"
        super (MakefileObjectCint, self).__init__ (master, input, cxxfile)

        self.m_flags = "$(RC_DICTFLAGS)"

        self.m_headers = self.output()[0:self.output().rfind(".")] + ".headers"
        rule = MakefileDependency (master, self.m_headers, input,
                                   ["-D__CINT__", "-D__MAKECINT__",
                                    "-D__CLING__", "-Wno-unknown-pragmas"])
        rule.addOrderPrereq (MakefileDirectory (master.objdir()))
        self.addPrereq (rule)

        self.m_pkgname = master.pkgname()
        self.m_libdir = master.libdir()
        pass

    def writeCommandsCxx (self, file) :
        file.write ("\t$(SILENT)rc --internal rootcint " +
                    "$(ROOTSYS)/bin/rootcint $(RC_INCFLAGS) " +
                    self.m_source + " " + self.m_cxxfile + " " +
                    self.m_headers + " " + self.m_libdir + " " +
                    self.m_pkgname + "\n")
        pass
    pass

class MakefileObjectReflex (MakefileObject) :
    """description: a class for rules that generate an object file for
    the Reflex dictionary"""

    def __init__ (self, master, input, xml, lib) :
        cxxfile = master.pkgname() + "_Reflex.cxx"

        super (MakefileObjectReflex, self).__init__ (master, input, cxxfile)

        self.addPrereq (xml)

        self.m_xml = xml.output()
        self.m_lib = os.path.basename (lib)
        self.m_rootmap = os.path.join( master.libdir(),
                                       lib[0:lib.rfind(".")] + ".rootmap" )

        self.m_flags = "$(RC_DICTFLAGS)"
        self.m_libdir = master.libdir()
        pass

    def writeCommandsCxx (self, file) :
        file.write ("\t$(SILENT)rc --internal genreflex " +
                    self.m_source + " " + self.m_xml + " " +
                    self.m_cxxfile + " " + self.m_rootmap + " " +
                    self.m_lib + " " + self.m_libdir + " $(RC_INCFLAGS)\n")
        pass
    pass



class MakefileLibrary (MakefileRule) :
    """description: a class for rules that generate a library file
    rationale: there is a class hierarchy here, as we generate
    separate libraries for the Reflex dictionaries"""

    def __init__ (self, val_output, output, files, extra) :

        super (MakefileLibrary, self).__init__ (val_output)

        self.m_files = files
        for file in files :
            self.addPrereq (file)
            pass

        self.addOrderPrereq (MakefileDirectory (os.path.dirname (val_output)))

        self.m_extra = extra
        pass



    def writeCommands (self, file) :
        file.write ("\t$(SILENT)echo Linking `basename $@`\n")

        flags = []
        for obj in self.m_files :
            flags += obj.linkFlags()
            pass
        flags += self.extraFlags()
        flags += ["-o", "$@"]
        file.write ("\t$(SILENT)$(RC_LD) " + " ".join(flags) + "\n")
        #file.write ("\t$(SILENT)$(RC_LD) " + string.join (flags, " ") + "\n")
        pass
    pass



class MakefileLibraryMain (MakefileLibrary) :
    """description: a rule for generating the main library for a
    package"""
    def __init__ (self, master, pkg) :
        libpattern = ["Root/*.cxx"]
        extra = pkg.preload()
        rules = master.objectsPattern (libpattern)
        source = os.path.join (master.srcdir(), "Root", "LinkDef.h")
        if os.path.exists (source) :
            rules.append (MakefileObjectCint (master, MakefileInput (source)))
            pass
        if len (rules) == 0 :
            raise RCError ("package " + master.pkgname() +
                           " has no source files, please set PACKAGE_NOCC=1" +
                           " in Makefile.RootCore to disable library building")

        output = os.path.join (master.libdir(), "lib" + master.pkgname() + ".so")
        super (MakefileLibraryMain, self).__init__ (output, output, rules, extra)
        self.m_pkg = pkg
        pass

    def extraFlags (self) :
        """return: extra flags for linking"""
        result = ["$(RC_LIBFLAGS)", "-L" + self.m_pkg.wa().libdir()]
        for preload in self.m_pkg.preload() :
            result.append ("-l" + preload)
            pass
        return result
    pass



class MakefileLibraryReflex (MakefileLibrary) :
    """description: a rule for generating the library for the Reflex
    dictionary of a package (if it uses one)"""

    def __init__ (self, master) :
        dictpattern = ["Root/dict/*.cxx"]
        output = os.path.join (master.libdir(),
                               "lib" + master.pkgname() + "_Reflex.so")
        rules = master.objectsPattern (dictpattern)
        rules += master.m_librules
        source = os.path.join (master.m_srcdir, master.m_pkg.name(),
                               master.m_pkg.name() + "Dict.h")
        target = master.m_pkg.name() + "_Reflex.o"
        lib = "lib" + master.m_pkg.name() + "_Reflex.so"
        xml = MakefileInput (os.path.join (master.m_srcdir, master.m_pkg.name(),
                                           "selection.xml"))
        rules.append (MakefileObjectReflex (master, MakefileInput(source), xml,
                                            os.path.basename(output)))

        super (MakefileLibraryReflex, self).__init__ (output, output, rules, [])
        pass

    def extraFlags (self) :
        """return: extra flags for linking"""
        return ["$(RC_DICTLIBFLAGS)", "$(RC_BINFLAGS)"]
    pass



class MakefileBinary (MakefileRule) :
    """description: the rule for generating a binary file for the
    package"""

    def __init__ (self, master, source, bindir) :
        output = os.path.basename (source.output())
        output = output[0:-2]
        output = os.path.join (bindir, output)

        super (MakefileBinary, self).__init__ (output, source)

        for lib in master.librules() :
            self.addOrderPrereq (lib)
            pass

        self.addOrderPrereq (MakefileDirectory (bindir))

        self.compactify ("", ".o")
        pass

    def writeCommands (self, file) :
        file.write ("\t$(SILENT)echo Linking `basename $@`\n")
        file.write ("\t$(SILENT)$(RC_LD) -o $@ $< $(RC_BINFLAGS)\n")
        pass
    pass



class MakefilePostcompile (MakefileRule) :
    """description: a rule for running the postcompile stie of a
    package"""

    def __init__ (self, master, prereq) :
        pc_name = "postcompile_" + master.pkgname()
        super (MakefilePostcompile, self).__init__ (pc_name, None)

        for pre in prereq :
            self.addPrereq (pre)
            pass

        self.m_pkgname = master.pkgname()
        pass

    def writeCommands (self, file) :
        file.write ("\t$(SILENT)rc --internal postcompile_pkg " +
                    self.m_pkgname + "\n")
        pass
    pass


class MakefilePackage (MakefileRule) :
    """description: a class managing the master rule for compiling an
    entire package
    rationale: this collects all the rules of a package, and is meant
    to make it easy to go to a system where multiple packages are
    compiled in one go"""

    def __init__ (self, pkg) :
        binpattern = ["util/*.cxx"]
        testpattern = ["test/*.cxx"]

        super (MakefilePackage, self).__init__ ("package_" + pkg.name(), None)

        self.m_pkg = pkg
        pre = []

        if not self.m_pkg.nocc() :
            make = MakefileLibraryMain (self, self.m_pkg)
            self.m_librules = [make]
            pre.append (make)
            self.addPrereq (make)
            pass
        else:
            self.m_librules = []
            pass

        if self.m_pkg.useReflex() :
            make = MakefileLibraryReflex (self)
            pre.append (make)
            self.addPrereq (make)
            pass

        for file in self.objectsPattern (binpattern) :
            make = MakefileBinary (self, file, self.bindir())
            pre.append (make)
            self.addPrereq (make)
            pass
        for file in self.objectsPattern (testpattern) :
            make = MakefileBinary (self, file, self.testdir())
            pre.append (make)
            self.addPrereq (make)
            pass

        make = MakefilePostcompile (self, pre)
        self.addPrereq (make)
        pass

    def pkgname (self) :
        """description: the name of the package"""
        if not hasattr (self, "m_pkgname") :
            self.m_pkgname = self.m_pkg.name()
            pass
        return self.m_pkgname

    def librules (self) :
        """description: the rules for linking the library"""
        return self.m_librules

    def bindir (self) :
        """description: the binary directory"""
        if not hasattr (self, "m_bindir") :
            self.m_bindir = self.m_pkg.bindir()
            pass
        return self.m_bindir

    def testdir (self) :
        """description: the binary directory for tests"""
        if not hasattr (self, "m_testdir") :
            self.m_testdir = self.m_pkg.testdir()
            pass
        return self.m_testdir

    def libdir (self) :
        """description: the library directory"""
        if not hasattr (self, "m_libdir") :
            self.m_libdir = self.m_pkg.libdir()
            pass
        return self.m_libdir

    def objdir (self) :
        """description: the object directory"""
        if not hasattr (self, "m_objdir") :
            self.m_objdir = self.m_pkg.objdir()
            pass
        return self.m_objdir

    def srcdir (self) :
        """description: the source directory"""
        if not hasattr (self, "m_srcdir") :
            self.m_srcdir = self.m_pkg.srcdir()
            pass
        return self.m_srcdir

    def objectsPattern (self, patterns) :
        rules = []
        for pattern in patterns:
            mydir = os.path.join (self.srcdir(), os.path.dirname (pattern))
            mypattern = os.path.basename (pattern)
            if os.path.exists (mydir) :
                for file in os.listdir (mydir):
                    # rationale: the check avoids temporary files from emacs
                    if file[0] != '.' and fnmatch.fnmatch (file, mypattern) :
                        mf_input = MakefileInput (os.path.join (mydir, file))
                        rules.append (MakefileObjectSource (self, mf_input))
                        pass
                    pass
                pass
            pass
        return rules

    def writeCommands (self, file) :
        pass
    pass


class Makefile (object) :
    """description: this package manages an entire makefile and all
    the rules that go into it"""

    def __init__ (self, wa, pkg) :
        self.m_wa = wa
        self.m_pkg = pkg

        self.m_pedantic = (self.m_wa.pedantic() == 2 or
                           (self.m_wa.pedantic() == 1 and self.m_pkg.pedantic()))
        self.m_baseopt = self.m_pkg.noopt() != "1"
        self.m_dictopt = self.m_pkg.noopt() != "1" and self.m_pkg.noopt() != "dict"

        self.m_objdir = self.m_pkg.objdir()
        self.m_libdir = self.m_pkg.libdir()
        self.m_bindir = self.m_pkg.bindir()
        self.m_testdir = self.m_pkg.testdir()
        self.m_srcdir = self.m_pkg.srcdir()

        pass

    def addFiles (self) :
        self.m_master = MakefilePackage (self.m_pkg)
        pass

    def write (self, raw_file) :
        file = MakefileOutput (raw_file)
        file.write ("RC_CXX       = " + self.m_wa.getCxx () + "\n")
        file.write ("RC_LD        = " + self.m_wa.getLd () + "\n")

        cxx_flags = self.m_pkg.getCxxFlags (opt=self.m_baseopt,
                                            pedantic=self.m_pedantic)
        file.write ("RC_CXXFLAGS  = " + cxx_flags + "\n")
        dic_flags = self.m_pkg.getCxxFlags (opt=self.m_dictopt,
                                            pedantic=self.m_pedantic)
        file.write ("RC_DICTFLAGS = " + dic_flags + "\n")
        file.write ("RC_INCFLAGS  = " +
                    self.m_pkg.getCxxFlags (incOnly=True) + "\n")
        file.write ("RC_LIBFLAGS  = " + self.m_pkg.getLibFlags () + "\n")
        if self.m_pkg.useReflex():
            getLibFlags = self.m_wa.packages().getLibFlags
            dl_flags = getLibFlags("lib" + self.m_pkg.name() + "_Reflex.so")
            file.write ("RC_DICTLIBFLAGS = " + dl_flags + "\n")
            pass
        file.write ("RC_BINFLAGS  = " + self.m_pkg.getBinFlags () + "\n")
        file.write ("\n\n")

        file.write ("all_" + self.m_pkg.name()
                    + " : dep_" + self.m_pkg.name())
        file.write (" " + self.m_master.output())
        file.write ("\n")

        self.m_master.write (file)

        file.write ("\n\ndep_" + self.m_pkg.name() + " :")
        for dep in file.m_deplist.keys():
            file.write (" " + dep)
            pass
        file.write ("\n")

        if len (file.m_deplist) > 0:
            file.write ("\n\n-include ")
            for dep in file.m_deplist.keys():
                file.write (" " + dep)
                pass
            file.write ("\n")
            pass
        pass
    pass
