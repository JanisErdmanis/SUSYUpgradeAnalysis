#include <fstream>
#include <iostream>
#include <string>
#include <TSystem.h>

void load_packages (std::string options = "")
{
#if ( ROOT_VERSION_CODE < ROOT_VERSION( 5, 99, 0 ) ) and not defined( __CINT__ )
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "*****************************************************************" << std::endl;
  std::cout << std::endl;
  std::cout << "Warning!   Warning!   Warning!   Warning!   Warning!" << std::endl;
  std::cout << "Warning!   Warning!   Warning!   Warning!   Warning!" << std::endl;
  std::cout << std::endl;
  std::cout << "You are currently trying to use a compiled version of load_packages.C," << std::endl;
  std::cout << "Which is strongly advised against" << std::endl;
  std::cout << std::endl;
  std::cout << "please switch to not compiling anymore, i.e. call it like:" << std::endl;
  std::cout << "  .x $ROOTCOREDIR/scripts/load_packages.C" << std::endl;
  std::cout << "Please note that there is *NO* plus sign(+) at the end of the line" << std::endl;
  std::cout << std::endl;
  std::cout << "Warning!   Warning!   Warning!   Warning!   Warning!" << std::endl;
  std::cout << "Warning!   Warning!   Warning!   Warning!   Warning!" << std::endl;
  std::cout << std::endl;
  std::cout << "*****************************************************************" << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
#endif

  if (gROOT->Get ("load_packages_run") != nullptr)
  {
    std::cout << "load_packages.C already run, not running again" << std::endl;
    return;
  }
  gROOT->Add (new TNamed ("load_packages_run", "load_packages_run"));

  const char *ROOTCOREDIR = gSystem->Getenv ("ROOTCOREDIR");
  if (ROOTCOREDIR == 0)
  {
    throw std::string ("ROOTCOREDIR not set, please set up RootCore");
  }
  std::string dir = ROOTCOREDIR;

  const char *ROOTCOREBIN = gSystem->Getenv ("ROOTCOREBIN");
  if (ROOTCOREBIN == 0)
  {
    throw std::string ("ROOTCOREBIN not set, please set up RootCore");
  }
  std::string bin = ROOTCOREBIN;

  const char *ROOTCORECONFIG = gSystem->Getenv ("ROOTCORECONFIG");
  if (ROOTCORECONFIG == 0)
  {
    throw std::string ("ROOTCORECONFIG not set, please set up RootCore");
  }
  std::string arch = ROOTCORECONFIG;

  const std::string include_dir = bin + "/include";
  if (gSystem->AccessPathName (include_dir.c_str()) != 0)
  {
    throw std::string ("failed to find " + include_dir);
  }
  gSystem->AddIncludePath (("-I\"" + include_dir + "\"").c_str());

  const std::string lib_dir = bin + "/lib/" + arch;
  if (gSystem->AccessPathName (lib_dir.c_str()) == 1)
  {
    std::cout << "failed to find " << lib_dir << std::endl;
    throw std::string ("failed to find " + lib_dir);
  }
  // gSystem->AddDynamicPath (lib_dir.c_str());

  if (gSystem->Exec ("$ROOTCOREDIR/internal/rc --internal prep_load_packages") != 0)
  {
    throw std::string ("failed to run load_packages preparation script");
  }



  // rationale: using string constants doesn't always seem to work
  //   properly in CINT, at least on Mavericks
  // std::string prefix_cxxflags = "cxxflags = ";
  // std::string prefix_include = "include = ";
  // std::string prefix_lib = "lib = ";

  std::ifstream info ((bin + "/load_packages_info_" + arch).c_str());
  std::istream *myinfo = &info;
  std::string line;
  while (std::getline (*myinfo, line))
  {
    if (!line.empty() && line[0] != '#')
    {
      // std::cout << line << std::endl;
      if (line.find ("cxxflags = ") == 0)
      {
	std::string cxxflags = line.substr (11);
	// std::cout << "cxxflags = " << cxxflags << std::endl;
	std::string optflags   = gSystem->GetFlagsOpt ();
	std::string debugflags = gSystem->GetFlagsDebug ();
	gSystem->SetFlagsOpt   ((optflags   + " "  + cxxflags).c_str());
	gSystem->SetFlagsDebug ((debugflags + " "  + cxxflags).c_str());
      } else if (line.find ("include = ") == 0)
      {
	std::string include = line.substr (10);
	// std::cout << "include = " << include << std::endl;
	if (include[0] != '/')
	  include = bin + "/" + include;
	// std::cout << "  include = " << include << std::endl;
	gSystem->AddIncludePath (("-I\"" + include + "\"").c_str());
      } else if (line.find ("semiinclude = ") == 0)
      {
	std::string semiinclude = line.substr (14);
	// std::cout << "  semiinclude = " << semiinclude << std::endl;
	gSystem->AddIncludePath (semiinclude.c_str());
      } else if (line.find ("lib = ") == 0)
      {
	std::string lib = line.substr (6);
	// std::cout << "lib = " << lib << std::endl;
	if (gSystem->Load (lib.c_str()) < 0)
	{
	  throw std::string ("failed to load library " + lib);
	}
      } else if (line.find ("reflexlib = ") == 0)
      {
	if (options.find ("preloadreflex") != std::string::npos)
	{
	  std::string lib = line.substr (12);
	  // std::cout << "reflexlib = " << lib << std::endl;
	  if (gSystem->Load (lib.c_str()) < 0)
	  {
	    throw std::string ("failed to load library " + lib);
	  }
	}
      } else if (line.find ("process = ") == 0)
      {
	std::string process = line.substr (10);
	gROOT->ProcessLine (process.c_str());
      } else
      {
	std::cout << "can't parse line \"" << line << "\"" << std::endl;
      }
    }
  }



  if (gSystem->AccessPathName ((bin + "/load_packages_success").c_str()) != 0)
    std::ofstream success ((bin + "/load_packages_success").c_str());
}
