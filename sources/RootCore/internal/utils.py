import os
import re
import string
import subprocess
import sys
import textwrap
import traceback


class RCError (Exception):
    """description: the standard exception class used inside
    RootCore
    rationale: this is to be used with error messages that are
    formatted in a way that they by themselves without a stack trace
    are comprehensible to the user.  if the error message is of a more
    technical nature the regular Exception class is to be used, as it
    will be printed with the full stack trace."""
    pass


def rcexcepthook (exctype, value, trace):
    """effects: print out the caught exception and then exit
    rationale: this is used as the default exception handler inside
    RootCore.  it will surpress the stack trace for RCError, since
    those are supposed to be fully formatted for the user already"""
    print "RootCore: Error " + str (value)
    if exctype != RCError or os.getenv ("ROOTCORE_VERBOSE") == "1":
        print "\n\n"
        print string.join (traceback.format_tb (trace))
        pass
    sys.exit (1)
    pass

def add_exception_handler ():
    """effects: install our default exception handler"""
    sys.excepthook = rcexcepthook
    pass

def print_except_message ():
    """effects: print the exception message
    rationale: this is to be used inside an except block to get
    RootCore like printout of errors"""
    exc_info = sys.exc_info ()
    exctype = exc_info[0]
    value = exc_info[1]
    trace = exc_info[2]
    print "RootCore: Error " + str (value)
    if exctype != RCError or os.getenv ("ROOTCORE_VERBOSE") == "1":
        print "\n\n"
        print string.join (traceback.format_tb (trace))
        pass
    pass


def shell_exec (cmd, allowFail=False, newEnv={}, returnRC=False, noReturn=False,
                stdin=None, stdout=None, stderr=None, workDir=None, retries=0,
                warnFail=False):
    """effects: execute the given shell command, specified as a list
    where the command is the first element and the arguments the
    following arguments.  this will throw an exception on failure
    (non-zero return code), unless allowFail or returnRC is passed.
    newEnv allows to specify extra environment variables to be set (or
    unset if a variable is assigned None).  stdin, stdout and stderr
    allow to redirect the standard file descriptors to files (if
    desired).  workDir allows to specify a separate directory in which
    the command is executed.  retries allows to specify that upon
    failure the command should be retried the given number of times
    before actually considering it a failure.

    returns: the command output, unless returnRC is specified (in
    which case it returns the return value) or noReturn/stdout (in
    which case nothing is returned) is specified

    rationale: I defined my own function for executing shell commands,
    so that I can define my own extra options as needed.  since
    executing shell commands is central to a build and package
    management system that seemed wise.

    rationale: the retries parameter is meant for use with the svn
    commands which can have occasional random failures when not
    located at CERN"""
    oldEnviron = {}
    oldPath = None
    try:
        for key in newEnv.keys():
            oldEnviron[key] = os.getenv (key)
            if newEnv[key]:
                os.environ[key] = newEnv[key]
                pass
            elif key in os.environ.keys():
                del os.environ[key]
                pass
            pass
        if (not os.getenv ("DYLD_LIBRARY_PATH")) and os.getenv ("ROOTCORE_DYLD_PATH") :
            if os.getenv ("ROOTCORE_VERBOSE") == "1" :
                print "recovering old DYLD path: " + os.getenv ("ROOTCORE_DYLD_PATH")
                pass
            oldEnviron["DYLD_LIBRARY_PATH"] = None
            os.environ["DYLD_LIBRARY_PATH"] = os.getenv ("ROOTCORE_DYLD_PATH")
            pass
        if workDir:
            oldPath = os.getcwd()
            os.chdir (workDir)
            pass
        if not stdout:
            if returnRC or noReturn:
                stdout = None
                pass
            else:
                stdout = subprocess.PIPE
                pass
            pass
        if not stderr:
            stderr = sys.stderr
            pass
        if os.getenv ("ROOTCORE_VERBOSE") == "1" :
            print "calling: " + string.join (cmd, " ")
            pass
        p = subprocess.Popen (cmd, stdin=stdin, stdout=stdout, stderr=stderr)
        output = p.communicate()
        if retries > 0 and p.returncode != 0:
            return shell_exec (cmd, allowFail=allowFail, newEnv=newEnv,
                               returnRC=returnRC, noReturn=noReturn,
                               stdin=stdin, stdout=stdout, stderr=stderr,
                               workDir=workDir, retries=retries-1)
        if returnRC:
            return p.returncode
        if p.returncode != 0 and not allowFail:
            if warnFail:
                print "WARNING: failed to call: " + string.join (cmd, " ")
                pass
            else :
                raise RCError ("failed to call: " + string.join (cmd, " "))
            pass
        if noReturn:
            return
        return output[0]
    finally:
        for key in oldEnviron.keys():
            value = oldEnviron[key]
            if value:
                os.environ[key] = value
                pass
            elif key in os.environ.keys():
                del os.environ[key]
                pass
            pass
        if oldPath:
            os.chdir (oldPath)
            pass
        pass
    pass


# NOTE: I don't see this function used anywhere.
def shell_init (cmd):
    """effects: execute command cmd (syntax same as for shell_exec),
    and then update the environment of the current process in the same
    way as the command updated its own environment.

    rationale: most setup tools only provide mechanisms for setting up
    shell environments, not python environments.  this command allows
    to sidestep that problem"""
    separator = "------------------- cut here -------------------"
    bash_arg = "env && echo \"%s\" && %s && echo %s && env" % \
               (separator, " ".join (cmd), separator)
    output = shell_exec (["/bin/bash", "-c", bash_arg])
    outputList = string.split (output, separator + "\n")
    if len (outputList) != 3:
        raise RCError ("separator found in command output: " + \
                       string.join (cmd, " "))
    myenv = []
    for suboutput in outputList[0], outputList[2]:
        subenv = {}
        level = 0
        for line in string.split (suboutput, "\n"):
            if line.find ("{") != -1:
                level = level + 1
                pass
            if line != "" and level == 0:
                split = string.find (line, "=")
                if split == -1:
                    raise RCError ("failed to parse line: " + line + \
                                   "\nin:\n" + suboutput)
                subenv[line[0:split]] = line[split+1:]
                pass
            if line.find ("}") != -1:
                if level == 0:
                    raise RCError ("extra } found in: " + suboutput)
                level = level - 1
                pass
            pass
        myenv.append (subenv)
        pass

    for var in myenv[1].keys():
        if not var in myenv[0]:
            os.environ[var] = myenv[1][var]
            pass
        else:
            if myenv[0][var] != myenv[1][var]:
                os.environ[var] = myenv[1][var]
                pass
            del myenv[0][var]
            pass
        pass

    for var in myenv[0].keys():
        del os.environ[var]
        pass

    sys.stdout.write (outputList[1])
    pass


# NOTE: could be simplified with os.makedirs
def mkdir_p (dir):
    """effects: make the given directory and all its parent
    directories, if they don't exist already"""
    if not os.path.isdir (dir):
        while dir[-1:] == "/":
            dir = dir[:-1]
            pass
        if len (dir) == 0:
            raise Exception ("failed to find top level directory")
        basedir = os.path.dirname (dir)
        if len (basedir) != "":
            mkdir_p (basedir)
            pass
        os.mkdir (dir)
        pass
    pass


def expand_env (value):
    """effects: expand the given value with environment variables
    expanded as needed

    returns: the content of the field, or None if the field is not found

    rationale: this is mostly meant to parse variables from the
    Makefile.RootCore, and the format is accordingly, i.e. variables
    take the form "name=value" and all lines not matching that format
    (including lines starting with a '#') are ignored

    rationale: originally RootCore didn't allow environment variables
    in its configuration files, but that proved to restrictive.  When
    they were introduced it was not in a common format, since some
    where parsed by the shell, others by make, so now environment
    variables can take three forms "$NAME", "$(NAME)" and "${NAME}"
    """
    result = ""
    expr = re.compile ("\$([A-Za-z0-9_]+)|\$\(([A-Za-z0-9_]+)\)" + \
                      "|\$\{([A-Za-z0-9_]+)\}")
    while len (value) > 0:
        match = expr.search (value)
        if match:
            result += value[:match.start()]
            value = value[match.end():]
            if match.group(1):
                var = match.group(1)
            elif match.group(2):
                var = match.group(2)
            elif match.group(3):
                var = match.group(3)
            else:
                raise Exception ("internal error")
            val = os.getenv (var)
            if val:
                result += val
        else:
            result += value
            value = ""
            pass
        pass
    return result


def get_field (file, field, default=None, empty=""):
    """effects: get the content of the field with the given name from
    the given file
    returns: the content of the field, or None if the field is not found"""
    if file == None:
        result = os.getenv (field)
        pass
    else:
        expr = re.compile ("\s*" + field + "\s*=(.*)$")
        result = None
        with open (file) as input:
            for line in input:
                matchobj = expr.match (line)
                if matchobj:
                    result = string.strip (matchobj.group(1))
                    pass
                pass
            pass
        pass
    if result == None:
        return default
    if result == "":
        return empty
    return result


def get_field_env (file, field):
    """effects: get the content of the field with the given name from
    the given file, with environment variables expanded as needed
    returns: the content of the field, or None if the field is not
    found"""
    return expand_env (get_field (file, field, default=""))


def set_field (file, field, value):
    """effects: set the content of the field with the given name for
    the given file"""
    expr = re.compile ("\s*" + field + "\s*=(.*)$")
    content = ""
    changed = False
    found = False
    if not value:
        value = ""
        pass
    with open (file) as input:
        for line in input:
            matchobj = expr.match (line)
            if not matchobj:
                content += line
                pass
            elif found:
                changed = True
                pass
            else:
                found = True
                if string.strip (matchobj.group(1)) != value:
                    changed = True
                    content += field + " = " + value + "\n"
                    pass
                else:
                    content += line
                    pass
                pass
            pass
        pass
    if not found:
        content += field + " = " + value + "\n"
        changed = True
        pass
    if changed:
        with open (file, "w") as output:
            output.write (content)
            pass
        pass
    pass


def ask_question (question, answers):
    """effects: ask the given question repeatedly until I receive one
    of the given answers
    returns: the answer given"""
    answer = None
    while not answer in answers:
        sys.stdout.write (question + " (answer " + " or ".join (answers) + "): ")
        answer = string.strip (sys.stdin.readline ())
        pass
    return answer


def format_block (prefix_first, prefix_rest, width, text):
    """effects: format a text for output as a block with a given
    prefix"""
    result = ""
    for line in textwrap.wrap (text, width):
        result += prefix_first + line + "\n"
        prefix_first = prefix_rest
        pass
    return result


class ShellInit (object):
    """description: Base class for all classes that implement shell
    script initialization for different shells
    rationale: this is essentially just a strategy object, which
    accumulates the commands needed and then can print them out in
    the end.
    """

    def __init__ (self):
        # the list of commands to be executed
        self.m_cmds = []
        self.m_stdout = sys.stdout
        pass

    def abort (self, message):
        """effects: just print a message and abort, without doing
        anything
        side effects: ends the script evaluation
        """
        self.m_cmds = []
        self.echo (message, toError=True)
        self.execCmd ("return 1")
        self.makeCmds ()
        sys.exit (0)
        pass

    def makeCmds (self):
        """effects: print out all the assembled commands"""
        self.m_stdout.write (string.join (self.m_cmds, " ; "))
        pass

    def execCmd (self, cmd):
        """effects: execute the given command as is"""
        self.m_cmds.append (cmd)
        pass

    def quote (self, unquoted):
        """effects: quote a string, so that it can safely be passed
        into shell commands"""
        expr = re.compile ("[a-zA-Z0-9./_]")
        quoted = ""
        for char in unquoted:
            if char == "\n":
                raise RCError ("should not pass multi-line strings into quote")
            if not expr.match (char):
                quoted += "\\";
                pass
            quoted += char
            pass
        return quoted

    def echo (self, message, toError=False):
        """effects: echo the message to the screen"""
        expr = re.compile ("[a-zA-Z0-9./_]")
        if toError:
            postfix = " 1>&2"
            pass
        else:
            postfix = ""
            pass
        for line in string.split (message, "\n"):
            self.execCmd ("echo " + self.quote (line) + postfix)
            pass
        pass
    pass

    def addToPath (self, path, dir):
        """effects: add a new directory to the path
        warning: calling this more than once for the same path is
        not supported"""
        old = os.getenv (path)
        if old != None:
            self.setEnv (path, dir + ":" + old)
            pass
        else:
            self.setEnv (path, dir)
            pass
        pass

    def removeFromPath (self, path, dir):
        """effects: add a new directory to the path
        warning: calling this more than once for the same path is
        not supported"""
        old = os.getenv (path)
        if old != None:
            mypath = string.split (old, ":")
            while mypath.count (dir) > 0:
                mypath.remove (dir)
                pass
            self.setEnv (path, string.join (mypath, ":"))
            pass
        pass

    def unsetEnv (self, name):
        """effects: remove the environment variable of the given
        name"""
        if os.getenv (name):
            del os.environ[name]
            self.execCmd ('unset ' + name)
            pass
        pass

    def write (self, text):
        """effects: print the given text to the screen"""
        self.echo (text)
        pass
    pass


class ShellInitSh (ShellInit):
    """description: the ShellInit class for the bourne family of
    shells"""

    def __init__ (self):
        ShellInit.__init__ (self)
        # description: the suffix used for script files
        self.m_suffix = ".sh"
        pass

    def setEnv (self, name, content):
        """effects: set the variable of the given name to the
        given content"""
        os.environ[name] = content
        self.execCmd ('export ' + name + '="' + content + '"')
        pass
    pass


class ShellInitCSh (ShellInit):
    """description: the ShellInit class for the C family of shells"""

    def __init__ (self):
        ShellInit.__init__ (self)
        # description: the suffix used for script files
        self.m_suffix = ".csh"
        pass

    def setEnv (self, name, content):
        """effects: set the variable of the given name to the
        given content"""
        os.environ[name] = content
        self.execCmd ('setenv ' + name + ' "' + content + '"')
        pass
    pass


class ShellInitC (ShellInit):
    """description: a 'shell' init class for use with setup.C, which
    is needed for PROOF usage"""

    def __init__ (self):
        ShellInit.__init__ (self)
        # description: the suffix used for script files
        self.m_suffix = ".C"
        pass

    def setEnv (self, name, content):
        """effects: set the variable of the given name to the
        given content"""
        os.environ[name] = content
        self.m_cmds.append ('setenv ' + name + '=' + content)
        pass

    def abort (self, message):
        """effects: just print a message and abort, without doing
        anything
        side effects: ends the script evaluation"""
        self.m_cmds = []
        self.echo (message)
        self.m_cmds.append ("abort")
        self.makeCmds ()
        sys.exit (0)
        pass

    def makeCmds (self):
        """effects: print out all the assembled commands"""
        self.m_stdout.write (string.join (self.m_cmds, "\n"))
        pass

    def execCmd (self, cmd):
        """effects: execute the given command as is"""
        self.m_cmds.append ("exec " + cmd)
        pass

    def echo (self, message):
        """effects: echo the message to the screen"""
        for line in string.split (message, "\n"):
            self.m_cmds.append ("echo " + line)
            pass
        pass

    def addToPath (self, path, dir):
        """effects: add a new directory to the path
        warning: calling this more than once for the same path is
        not supported"""
        if path == "LD_LIBRARY_PATH" or path == "DYLD_LIBRARY_PATH":
            self.m_cmds.append ("dynamic " + dir)
            pass
        else:
            super (ShellInitC, self).addToPath (path, dir)
            pass
        pass
    pass

