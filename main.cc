/*
    Copyright (C) 2016 Florian Paul Schmidt <mista.tapas@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <boost/program_options.hpp>
#include <pHash.h>

#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <string>
#include <climits>
#include <cstdio>

#include <sys/time.h>

void set_watchdog_timeout(unsigned watchdog_timeout_seconds)
{
    if (0 != watchdog_timeout_seconds)
    {
        itimerval timer;
        itimerval old_timer;
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 0;
        timer.it_value.tv_sec = watchdog_timeout_seconds;
        timer.it_value.tv_usec = 0;
        int timer_success = setitimer(ITIMER_VIRTUAL, &timer, &old_timer);
        
        if (0 != timer_success)
        {
            throw std::runtime_error("Failed to set watchdog timeout");
        }
    }
}

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;
    
    std::string input_file = "image.png";
    
    unsigned watchdog_timeout_seconds = 15u;
    
    const char *license = 
        #include "LICENSE.txt"
    ;
    
    try
    {
        po::options_description options_description;
        options_description.add_options()
            ("help,h", "Output help text and exit successfully")
            ("input-file,i", po::value<std::string>(&input_file)->default_value(input_file), "The input video file name")
            ("watchdog-timeout", po::value<unsigned>(&watchdog_timeout_seconds)->default_value(watchdog_timeout_seconds), "How long to wait for processing a frame (including seeking, etc) to finish. If this time (seconds) is exceeded abort with failure. Use 0 to disable the watchdog.")
            ("license", "Output license information")
        ;
        
        po::variables_map variables_map;
        po::store(po::parse_command_line(argc, argv, options_description), variables_map);
        po::notify(variables_map);
        
        if (variables_map.count("help"))
        {
            std::cout
                << "imagehash Copyright (C) 2016 Florian Paul Schmidt" << std::endl
                << "This program comes with ABSOLUTELY NO WARRANTY;" << std::endl
                << "This is free software, and you are welcome to redistribute it" << std::endl
                << "under certain conditions; Use --license for details" << std::endl
                ;

            std::cout << options_description << std::endl;
            return EXIT_SUCCESS;
        }
        
        if (variables_map.count("license"))
        {
            std::cout << license << std::endl;
            return EXIT_SUCCESS;
        }
    
        int hash_length = 0;

        uint8_t *hash = nullptr;    

        hash = ph_mh_imagehash(input_file.c_str(), hash_length);

        //! TODO: ERROR HANDLING?

        for (int index = 0; index < hash_length; ++index)
        {
            printf("%x", hash[index]);
        }
        printf("\n");
    }
    catch(std::exception &e)
    {
        std::cout << "Fail: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
