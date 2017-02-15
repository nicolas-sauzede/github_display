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
#include "sh_file.h"
#include "quicky_exception.h"

//------------------------------------------------------------------------------
void sh_file::open(const std::string & p_file_name,
		   ios_base::openmode p_mode)
{
  std::ofstream::open(p_file_name,
	 p_mode
	 );
  if(!this->is_open())
    {
      throw quicky_exception::quicky_runtime_exception("Unable to create sh file \"" + p_file_name ,__LINE__,__FILE__);
    }
  *this << "#!/bin/sh -f" << std::endl;
}

//------------------------------------------------------------------------------
void sh_file::close(void)
{
  *this << "#EOF" << std::endl;
  std::ofstream::close();
}

