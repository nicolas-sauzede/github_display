/*    This file is part of github_clock
      Copyright (C) 2017  Julien Thevenon ( julien_thevenon at yahoo.fr )

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#ifndef SH_FILE_H
#define SH_FILE_H

#include <fstream>

class sh_file: public std::ofstream
{
 public:
  void open(const std::string &filename,
	    ios_base::openmode mode = ios_base::out );
  void close(void);
};
#endif // SH_FILE_H
// EOF
