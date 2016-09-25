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
#include <fstream>

#include <sys/time.h>

void print_hash(uint8_t *hash, int len)
{
    for (int index = 0; index < len; ++index)
    {
        printf("%02x", hash[index]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;
    std::string input_file = "image.png";
    double threshold = 0;

    const char *license = 
        #include "LICENSE.txt"
    ;
    
    try
    {
        po::options_description options_description;
        options_description.add_options()
            ("help,h", "Output help text and exit successfully")
            ("input-file,i", po::value<std::string>(&input_file)->default_value(input_file), "The input video file name (or text file for threshold mode)")
            ("threshold", po::value<double>(&threshold)->default_value(threshold), "Interpret input-file as text file containing lines with hex encoded image hashes. Prints for every line the line numbers with image hashes that are closer than threshold followed by the distance")
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
   
        if (0 == threshold)
        { 
            int hash_length = 0;
    
            uint8_t *hash = nullptr;    
    
            hash = ph_mh_imagehash(input_file.c_str(), hash_length);
    
            //! TODO: ERROR HANDLING?

            print_hash(hash, hash_length);
    
        }
        else
        {
            // Memory leak that we don't care about since after this the
            // program exits anyways.
            std::vector<uint8_t*> hashes;
            std::vector<int> hash_lengths;

            std::ifstream in(input_file);
            std::string line;
            while(std::getline(in, line))
            {
                if (line.length() != 0)
                {
                    if (line.length() % 2 != 0)
                    {
                        throw std::runtime_error("malformed hash");
                    }

                    // Read pairs of characters
                    uint8_t *hash = new uint8_t[line.length() / 2];

                    for (unsigned index = 0; index < line.length() / 2; ++index)
                    {
                        std::string numstring = line.substr(2 * index, 2);
                        unsigned num = 0;
                        sscanf(numstring.c_str(), "%x", &num);
                        hash[index] = num;
                    }

                    hashes.push_back(hash);
                    hash_lengths.push_back(line.length() / 2);
                 }
            }

            for (unsigned index = 0; index < hashes.size(); ++index)
            {
                for (unsigned index2 = index + 1; index2 < hashes.size(); ++index2)
                {
                    double distance = ph_hammingdistance2(hashes[index], hash_lengths[index], hashes[index2], hash_lengths[index2]);
                    if (distance < threshold)
                    {
                        std::cout << index << " " << index2 << " " << distance << std::endl;
                    }
                }
            }
        }
    }
    catch(std::exception &e)
    {
        std::cout << "Fail: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
