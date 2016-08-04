import fnmatch
import os
import re
import string

# Local imports
from utils import RCError, shell_exec, mkdir_p

class SvnInfo (object) :
    """description: the base class for all classes that handle package
    management for a single package, either as a detached url, or a
    url and an associated package directory
    rationale: this interface does guarantee that SvnInfo objects do
    not get modified, instead there are use accessors that will
    provide a modified copy of the object
    rationale: the interface doesn't include any methods to describe
    the actual URL, since depending on the package management
    systement it may not be a simple URL.
    warning: not all SvnInfo objects implement all functionality.
    they may only implement that functionality that they need to get
    their particular job done.  for all other methods the base class
    implements a rump method that will throw an appropriate
    exception"""

    def __init__ (self, dir) :
        self.m_dir = dir
        pass

    @classmethod
    def make_srcdir (cls, dir, allowNone=True):
        """effects: make an SVN info object for the package in the
        directory dir
        returns: the SVN info object"""
        if not os.path.isdir (dir) :
            raise RCError ("source directory does not exist: " + dir)
        if os.path.isdir (os.path.join (dir, ".svn")):
            return SvnInfoSvn (None, dir)
        elif os.path.isdir (os.path.join (dir, ".git")):
            return SvnInfoGit (None, dir)
        elif allowNone :
            return None
        else :
            return SvnInfoUnmanaged (dir)
        pass

    @classmethod
    def make_packages (cls, url, srcdir):
        """effects: make an SVN info object based on the cache in the
        packages file
        returns: the SVN info object, or NULL if none was cached"""
        if url == "" :
            return None
        if url.find ("git ") == 0 :
            return SvnInfoGit (url[4:], srcdir)
        elif url.find ("%%%%%") == -1:
            url = "svn+ssh://" + url
            pass
        url = url.replace ("%%%%%", ":")
        return SvnInfoSvn (url, srcdir)

    @classmethod
    def make_url (cls, url, release=None, wa=None):
        """effects: make an SVN info object based on the given URL"""
        if url.find ("/") == -1 :
            result = None
            if release and url in release :
                result = release[url]
                pass
            elif wa and wa.hasPackages() :
                pkg = wa.packages().getPkg (url)
                if pkg :
                    result = pkg.svninfo ()
                pass
            if not result :
                raise RCError ("unknown package " + url)
        elif url.find ("atlas") == 0 :
            result = SvnInfoSvn ("svn+ssh://svn.cern.ch/reps/" + url, None)
            pass
        elif url.find ("svn+") == 0 :
            result = SvnInfoSvn (url, None)
            pass
        elif url.find ("git ") == 0 :
            result = SvnInfoGit (url[4:], None)
            pass
        else:
            raise RCError ("can't parse SVN URL: " + url)
        if wa and wa.hasPackages() :
            pkg = wa.packages().getPkg (result.name())
            if pkg :
                result = result.useDir (pkg.srcdir())
                pass
            pass
        return result

    def name (self) :
        """returns: the name of the package being managed"""
        raise RCError ("name not implemented for " + self.__class__.__name__)

    def tag (self) :
        """returns: the tag the url selects, this can be 'trunk' for
        the head, 'tags' for the latest tag, and an empty string for
        URLs that don't specify this"""
        raise RCError ("tag not implemented for " + self.__class__.__name__)

    def dir (self) :
        """returns: the directory we are associated with"""
        return self.m_dir

    def forPackages (self) :
        """returns: the entry for the packages file"""
        raise RCError ("forPackages not implemented for " + self.__class__.__name__)

    def useDir (self, dir) :
        """returns: the same url, but using the given directory"""
        raise RCError ("useDir not implemented for " + self.__class__.__name__)

    def useTag (self, dir) :
        """returns: the same url, except it is using the given tag"""
        raise RCError ("useTag not implemented for " + self.__class__.__name__)

    def resolveTags (self) :
        """returns: if this url uses the special tag 'tags' it returns
        this url switched to the latest tag, otherwise it returns the
        same url"""
        if self.tag() == "tags" :
            tags = self.svn_ls_tags ()
            if len (tags) > 0:
                return self.useTag (tags[len(tags)-1])
            else:
                return self.useTag ("trunk")
            pass
        return self

    def isHead (self) :
        """returns: whether this is a head version to which we can
        commit, i.e. the trunk or a branch"""
        raise RCError ("isHead not implemented for " + self.__class__.__name__)

    def isTrunk (self) :
        """returns: whether this is the trunk"""
        raise RCError ("isTrunk not implemented for " + self.__class__.__name__)

    def nicosName (self) :
        """description: the name we have for nicos"""
        return self.name()

    def rc_checkout (self, shared=False) :
        """effects: do our part of rc checkout"""
        raise RCError ("rc_checkout not implemented for " + self.__class__.__name__)

    def rc_update (self, clobberSwitch=False) :
        """effects: do our part of rc update.  If clobberSwitch is
        True, switch tags even if you have local changes"""
        raise RCError ("rc_update not implemented for " + self.__class__.__name__)

    def rc_version (self) :
        """effects: do our part of rc version"""
        raise RCError ("rc_version not implemented for " + self.__class__.__name__)

    def rc_status (self) :
        """effects: do our part of rc status"""
        raise RCError ("rc_status not implemented for " + self.__class__.__name__)

    def svn_log (self) :
        """returns: the message log, as in svn log"""
        raise RCError ("svn_log not implemented for " + self.__class__.__name__)

    def svn_status (self, noReturn=False, update=False, allowNone=False):
        """effects: same as 'svn status'
        returns: the status (unless noReturn is specified)"""
        raise RCError ("svn_status not implemented for " + self.__class__.__name__)

    def svn_ls_tags (self):
        """returns: a list of tags as in 'svn ls >base-svn</tags'"""
        raise RCError ("svn_ls_tags not implemented for " + self.__class__.__name__)

    def svn_commit (self, message):
        """effects: commit this package, as with 'svn commit -m
        >message<'"""
        raise RCError ("svn_commit not implemented for " + self.__class__.__name__)

    def svn_cp_tags (self, tag, message):
        """effects: copy this url to the tags directory, as in 'svn cp
        -m >message< >url< >base_url</tags/>tag<'"""
        raise RCError ("svn_cp_tags not implemented for " + self.__class__.__name__)
    pass

class SvnInfoSvn (SvnInfo) :
    """description: the SVN info object for svn managed packages"""

    def __init__ (self, url, dir) :
        if not url :
            info = shell_exec (["svn", "info"], newEnv={"LANG":None}, workDir=dir)
            expr = re.compile ("\nURL: ([^\\n]*)\n")
            match = expr.search (info)
            if not match :
                raise RCError ("failed to parse output of svn info " + dir +
                               ":\n" + info)
            url = match.group (1)
            pass
        super (SvnInfoSvn, self).__init__ (dir)
        self.m_url = url
        while url[-1] == "/" :
            url = url[:-1]
            pass
        name1 = os.path.basename (url)
        name2 = os.path.basename (os.path.dirname (url))
        name3 = os.path.basename (os.path.dirname (os.path.dirname (url)))
        if name2 == "tags" or name2 == "branches" or name2 == "devbranches" :
            self.m_name = name3
            self.m_tag = name1
            self.m_base = os.path.dirname (os.path.dirname (url))
            self.m_isHead = (name2 != "tags")
            self.m_isTrunk = False
            pass
        elif name1 == "tags" or name1 == "trunk" :
            self.m_name = name2
            self.m_tag = name1
            self.m_base = os.path.dirname (url)
            self.m_isHead = (name1 == "trunk")
            self.m_isTrunk = (name1 == "trunk")
            pass
        else :
            self.m_name = name1
            self.m_tag = ""
            self.m_base = url
            self.m_isHead = True
            self.m_isTrunk = True
            pass
        pass

    def name (self) :
        """returns: the name of the package being managed"""
        return self.m_name

    def tag (self) :
        """returns: the tag the url selects, this can be 'trunk' for
        the head, 'tags' for the latest tag, and an empty string for
        URLs that don't specify this"""
        return self.m_tag

    def useDir (self, dir) :
        """returns: the same url, but using the given directory"""
        if dir == self.dir() :
            return self
        return SvnInfoSvn (self.m_url, dir)

    def useTag (self, tag) :
        """returns: the same url, except it is using the given tag"""
        if tag == self.m_tag :
            return self
        if tag == "trunk" or tag == "tags" :
            return SvnInfoSvn (self.m_base + "/" + tag, self.m_dir)
        return SvnInfoSvn (self.m_base + "/tags/" + tag, self.m_dir)

    def isHead (self) :
        """returns: whether this is a head version to which we can
        commit, i.e. the trunk or a branch"""
        return self.m_isHead

    def isTrunk (self) :
        """returns: whether this is the trunk"""
        return self.m_isTrunk

    def forPackages (self) :
        """returns: the entry for the packages file"""
        url = self.m_url
        prefix = "svn+ssh://"
        if url.find (prefix) == 0 :
            url = url[len (prefix):]
            pass
        return url.replace (":", "%%%%%")
        #return string.replace (url, ":", "%%%%%")

    def nicosName (self) :
        """description: the name we have for nicos"""
        expr = re.compile (".*/atlas[a-z-]*/(.*/" + self.name() +")/.*")
        match = expr.match (self.m_url)
        if match:
            return "_".join (match.group(1).split("/"))
            #return string.join (string.split (match.group (1), "/"), "_")
        return super (SvnInfoSvn, self).nicosName()

    def rc_checkout (self, shared=False) :
        """effects: do our part of rc checkout"""
        if os.path.isdir (self.dir()) :
            return
        print "checking out " + self.name()
        if self.m_url.find ("svn+ssh://svn.cern.ch") == 0 :
            if not self.svnuser() :
                self = self.useSvnuser (os.getenv ("CERN_USER"))
                pass
            if not self.svnuser() :
                self = self.useSvnuser (os.getenv ("CERNUSER"))
                pass
            pass
        self = self.resolveTags()
        mkdir_p (os.path.dirname (self.dir()))
        shell_exec (["svn", "checkout", self.m_url, self.dir()], retries=2,
                    noReturn=True)
        pass

    def rc_update (self, clobberSwitch=False) :
        """effects: do our part of rc update"""
        if not os.path.isdir (self.dir()) :
            return
        svninfo = SvnInfoSvn (None, self.dir())
        self = self.useSvnuser (svninfo.svnuser())
        self = self.resolveTags ()

        if self.tag() != svninfo.tag() :
            cmd = ["svn", "switch", self.m_url]
            if not clobberSwitch :
                status = "\n" + self.svn_status()
                if status.find ("\nM") != -1 or status.find ("\nC") != -1:
                    print "you currently have changes in package " + self.name()
                    print "either check them in, or switch versions manually using:"
                    print "  cd " + self.dir()
                    print "  " + " ".join(cmd)
                    #print "  " + string.join (cmd, " ")
                    return
                pass
            print "switching " + self.name() + " to " + self.tag()
            shell_exec (cmd, workDir=self.dir(), retries=2, noReturn=True)
            return

        if os.path.basename (os.path.dirname (self.m_url)) == "tags" :
            print "package " + self.name() + " already at version " + self.tag()
            return

        print "updating " + svninfo.name() + " from head"
        shell_exec (["svn", "update"], workDir=self.dir(), retries=2, noReturn=True)
        return

    def rc_version (self) :
        """effects: do our part of rc version"""
        self = self.useSvnuser (None)
        url = self.m_url
        prefix = "svn+ssh://svn.cern.ch/reps/"
        if url.find (prefix) == 0 :
            url = url [len (prefix):]
            pass
        print url
        pass

    def svn_add (self, file) :
        shell_exec (["svn", "add", file])
        pass

    def svn_log (self) :
        """returns: the message log, as in svn log"""
        return shell_exec (["svn", "log", "--stop-on-copy", self.m_url], retries=2)

    def svn_status (self, noReturn=False, update=False, allowNone=False):
        """effects: same as 'svn status'
        returns: the status (unless noReturn is specified)"""
        cmd = ["svn", "status"]
        retries = 0
        if update:
            cmd.append ("-u")
            retries = 2
            pass
        return shell_exec (cmd, workDir=self.dir(), noReturn=noReturn, retries=retries)

    def svn_ls_tags (self):
        """returns: a list of tags as in 'svn ls >base-svn</tags'"""
        result = []
        shell_args = ["svn", "ls", self.m_base + "/tags"]
        #for tag in string.split (shell_exec (["svn", "ls", self.m_base + "/tags"], retries=2)) :
        for tag in shell_exec (shell_args, retries=2).split():
            result.append (tag.strip("/"))
            #result.append(string.strip(tag, "/"))
            pass
        return sorted (result)

    def svn_commit (self, message):
        """effects: commit this package, as with 'svn commit -m
        >message<'"""
        if not self.dir() :
            raise Exception ("no directory associated with the url")
        shell_exec (["svn", "commit", "-m", message], workDir=self.dir(),
                    retries=2, noReturn=True)
        pass

    def svn_cp_tags (self, tag, message):
        """effects: copy this url to the tags directory, as in 'svn cp
        -m >message< >url< >base_url</tags/>tag<'"""
        shell_exec (["svn", "cp", "-m", message, self.m_base + "/trunk",
                     self.m_base + "/tags/" + tag], noReturn=True,
                    retries=2)
        pass

    def svnuser (self) :
        """description: the user specified in the URL"""
        if not hasattr (self, "m_svnuser") :
            svnurl = self.m_url
            expr = re.compile ("//(.*)@")
            match = expr.search (svnurl)
            if match :
                return match.group(1)
            else :
                return None
            pass
        pass

    def useSvnuser (self, svnuser) :
        """returns: this URL with the user replaced"""
        user = self.svnuser()
        if user :
            self = SvnInfoSvn (self.m_url.replace (user + "@", ""), self.dir())
            #self = SvnInfoSvn (string.replace (self.m_url, user + "@", ""), self.dir())
            pass
        if svnuser :
            split = self.m_url.find ("//")
            url = self.m_url[0:split+2] + svnuser + "@" + self.m_url[split+2:]
            self = SvnInfoSvn (url, self.dir())
            pass
        return self
    pass

class SvnInfoGit (SvnInfo) :
    """description: the SVN info class for git managed packages"""

    def __init__ (self, url, dir) :
        if not url :
            lines = shell_exec (["git", "remote", "-v"], workDir=dir).split("\n")
            #lines = string.split (shell_exec (["git", "remote", "-v"], workDir=dir), "\n")
            #if len (lines) > 0 and len (string.split (lines[0])) > 1:
            if len (lines) > 0 and len (lines[0].split()) > 1:
                url = lines[0].split()[1]
                #url = string.split(lines[0])[1]
                pass
            pass
        super (SvnInfoGit, self).__init__ (dir)
        self.m_url = url
        self.m_svngit = None
        if not url and os.path.exists (os.path.join (dir, ".git", "svn")) :
            info = shell_exec (["git", "svn", "info"], workDir=dir)
            expr = re.compile ("\nURL: ([^\\n]*)\n")
            match = expr.search (info)
            if not match :
                raise RCError ("failed to parse output of git svn info " +
                               dir + ":\n" + info)
            self.m_svngit = SvnInfoSvn (match.group(1), dir)
            pass
        pass

    def name (self) :
        """returns: the name of the package being managed"""
        if self.dir() :
            name = os.path.basename (self.dir())
        else :
            name = os.path.basename (self.m_url)
        if len (name) > 4 and name[-4:] == ".git" :
            name = name[:-4]
        return name

    def forPackages (self) :
        """returns: the entry for the packages file"""
        if self.m_url :
            return "git " + self.m_url.replace (":", "%%%%%")
            #return string.replace ("git " + self.m_url, ":", "%%%%%")
        if self.m_svngit :
            return self.m_svngit.forPackages()
        return ""

    def useDir (self, dir) :
        """returns: the same url, but using the given directory"""
        if dir == self.dir() :
            return self
        return SvnInfoGit (self.m_url, dir)

    def svn_status (self, noReturn=False, update=False, allowNone=False):
        """effects: same as 'svn status'
        returns: the status (unless noReturn is specified)"""
        if update:
            raise RCError ("-u not supported for git status")
        return shell_exec (["git", "status", "-s"], workDir=self.m_dir,
                           noReturn=noReturn)

    def svn_ls_tags (self):
        """returns: a list of tags as in 'svn ls >base-svn</tags'"""
        return sorted (shell_exec (["git", "tag"],
                                   workDir=self.m_dir).split ("\n"))
        #return sorted (string.split (shell_exec (["git", "tag"], workDir=self.m_dir), "\n"))

    def svn_commit (self, message):
        """effects: commit this package, as with 'svn commit -m
        >message<'"""
        if not self.dir() :
            raise Exception ("no directory associated with the url")
        shell_exec (["git", "commit", "-a", "-m", message], workDir=self.m_dir,
                    noReturn=True)
        pass

    def svn_cp_tags (self, tag, message):
        """effects: copy the trunk version of this package to the tags
        directory, as in 'svn cp -m >message< >url>/trunk
        >url</tags/>tag<'"""
        shell_exec (["git", "tag", "-m", message, tag], noReturn=True)
        pass

    def rc_update (self, clobberSwitch=False) :
        """effects: do our part of rc update.  If clobberSwitch is
        True, switch tags even if you have local changes"""
        shell_exec (["git", "fetch"], workDir=self.m_dir, noReturn=True)
        pass

    def rc_checkout (self, shared=False) :
        """effects: do our part of rc checkout"""
        if os.path.isdir (self.dir()) :
            return
        print "checking out " + self.name()
        mkdir_p (os.path.dirname (self.dir()))
        if not self.m_url :
            raise RCError ("can not run rc checkout without git url")
        cmd = ["git", "clone"]
        if shared :
            cmd += ["--shared"]
            pass
        cmd += [self.m_url, self.dir()]
        shell_exec (cmd, noReturn=True)
        pass

    def rc_version (self) :
        """effects: do our part of rc version"""
        if self.m_url :
            print "git " + self.m_url
            pass
        elif self.m_svngit :
            self.m_svngit.rc_version ()
            pass
        else :
            print self.name() + ": git repository with no remotes"
            pass
        pass
    pass

class SvnInfoUnmanaged (SvnInfo) :
    """description: the SVN info object for directories that are not
    in version control"""

    def __init__ (self, dir) :
        super (SvnInfoUnmanaged, self).__init__ (dir)
        pass

    def name (self) :
        """returns: the name of the package being managed"""
        return os.path.basename (self.dir())

    def forPackages (self) :
        """returns: the entry for the packages file"""
        return ""

    def useDir (self, dir) :
        """returns: the same url, but using the given directory"""
        if dir == self.dir() :
            return self
        return SvnInfoUnmanaged (dir)

    def svn_status (self, noReturn=False, update=False, allowNone=False):
        """effects: same as 'svn status'
        returns: the status (unless noReturn is specified)"""
        message = "no svn or git information found in " + self.m_dir
        if not allowNone:
            raise RCError (message)
        if noReturn:
            print message
            pass
        else:
            return message + "\n"
        pass

    def rc_checkout (self) :
        """effects: do our part of rc checkout"""
        print "WARNING: no url for package " + self.name() + ", skipping"
        pass

    def rc_version (self) :
        """effects: do our part of rc version"""
        print "package " + self.name() + " not under version control"
        pass

    def rc_update (self, clobberSwitch=False) :
        """effects: do our part of rc update"""
        pass
    pass
