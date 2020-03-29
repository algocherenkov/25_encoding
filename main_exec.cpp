#include "compression.h"
#include <boost/program_options.hpp>
#include <iostream>

using namespace boost::program_options;

int main(int argc, const char *argv[])
{
    try
    {
        options_description desc{"=======Options======="};
        desc.add_options()
          ("pack", "defines what to do - pack")
          ("unpack", "defines what to do - unpack. File extension should be '.cmp'")
          ("file", value<std::string>()->value_name("what file will be packed or unpacked"));

        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.empty())
          std::cout << desc << '\n';
        else if (vm.count("pack") && vm.count("file"))
          CA::RLEcompress(vm["file"].as<std::string>(), vm["file"].as<std::string>() + ".cmp");
        else if (vm.count("unpack") && vm.count("file"))
          CA::RLEunpack(vm["file"].as<std::string>(), vm["file"].as<std::string>() + ".out");
        else
        {
            std::cerr << "inserted options were incorrect!" << std::endl;
            std::cout << desc << '\n';
        }
    }
    catch (const error &ex)
    {
        std::cerr << ex.what() << '\n';
    }
    return 0;
}
