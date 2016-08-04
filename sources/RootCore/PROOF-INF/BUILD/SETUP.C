Int_t SETUP()
{
  std::string dir = gSystem->WorkingDirectory();
  dir += "/";
  gSystem->Setenv ("ROOTCOREBIN", (dir + "RootCoreBin").c_str());

  try
  {
    gROOT->Macro ("RootCore/scripts/setup.C (\"RootCore/scripts/setup.C\")");
    gROOT->Macro ("RootCore/scripts/load_packages.C (\"preloadreflex\")");
  } catch (std::string& e)
  {
    std::cout << "error: " << e << std::endl;
    return -1;
  };
  return 0;
}
