/* ambi-tv: a flexible ambilight clone for embedded linux
*  Copyright (C) 2013 Georg Kaindl
*  
*  This file is part of ambi-tv.
*  
*  ambi-tv is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 2 of the License, or
*  (at your option) any later version.
*  
*  ambi-tv is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License
*  along with ambi-tv.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __AMBITV_SERVER_H__
#define __AMBITV_SERVER_H__

struct server_conf {
	/* master file descriptor list */
	fd_set master;
	/* maximum file descriptor number */
	int fdmax;
	/* listening socket descriptor */
	int listener;
};
extern struct server_conf ambitv_server_conf;

int
ambitv_server_init(int port);

void
ambitv_server_run();

#endif // __AMBITV_SERVER_H__
