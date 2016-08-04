void setup (const char *path)
{
  std::string mypath = path;
  mypath = mypath.substr (0, mypath.rfind ("RootCore") + 8);
  std::string cmd = mypath + "/internal/env_setup " + path;
  std::string setup = gSystem->GetFromPipe (cmd.c_str()).Data();

  while (!setup.empty())
  {
    std::string::size_type split = setup.find ("\n");
    std::string line;
    if (split == std::string::npos)
    {
      line = setup;
      setup.clear ();
    } else
    {
      line = setup.substr (0, split);
      setup = setup.substr (split + 1);
    }
    if (!line.empty())
    {
      if (line.find ("abort") == 0)
      {
	throw std::string ("encountered abort in env_setup");
      } else if (line.find ("echo ") == 0)
      {
	std::cout << line.substr (5) << std::endl;
      } else if (line.find ("dynamic ") == 0)
      {
	gSystem->AddDynamicPath (line.substr (8).c_str());
      } else if (line.find ("setenv ") == 0)
      {
	std::string::size_type split2 = line.find ("=");
	std::string var = line.substr (7, split2 - 7);
	std::string value = line.substr (split2+1);
	std::cout << var << "=" << value << std::endl;
	gSystem->Setenv (var.c_str(), value.c_str());
      } else if (line.find ("exec ") == 0)
      {
	std::string cmd = line.substr (5);
	std::cout << "exec " << cmd << std::endl;
	if (gSystem->Exec (cmd.c_str()) != 0)
	  throw std::string ("command failed: " + cmd);
      } else
      {
	throw std::string ("unknown command: " + line);
      }
    }
  }
}
