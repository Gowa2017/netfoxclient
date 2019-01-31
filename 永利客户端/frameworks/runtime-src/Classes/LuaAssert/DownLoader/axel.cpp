/*
  Axel -- A lighter download accelerator for Linux and other Unices

  Copyright 2001-2007 Wilmer van der Gaast
  Copyright 2007-2009 Y Giridhar Appaji Nag
  Copyright 2008-2009 Philipp Hagemeister
  Copyright 2015-2017 Joao Eriberto Mota Filho
  Copyright 2016      Denis Denisov
  Copyright 2016      Ivan Gimenez
  Copyright 2016      Sjjad Hashemian
  Copyright 2016      Stephen Thirlwall
  Copyright 2017      Antonio Quartulli
  Copyright 2017      Ismael Luceno
  Copyright 2017      nemermollon

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  In addition, as a special exception, the copyright holders give
  permission to link the code of portions of this program with the
  OpenSSL library under certain conditions as described in each
  individual source file, and distribute linked combinations including
  the two.

  You must obey the GNU General Public License in all respects for all
  of the code used other than OpenSSL. If you modify file(s) with this
  exception, you may extend this exception to your version of the
  file(s), but you are not obligated to do so. If you do not wish to do
  so, delete this exception statement from your version. If you delete
  this exception statement from all source files in the program, then
  also delete it here.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/* Main control */
#include "axel.h"
#include <thread>
#include "assert.h"

/* Axel */
static bool save_state(axel_t *axel);
static void *setup_thread(void *);
static void axel_message(axel_t *axel, const char *format, ...);
static void axel_divide(axel_t *axel);

//static char *buffer = NULL;

size_t readn( void *buf, size_t itemSize, size_t items, FILE *fd )
{
    size_t loaded = 0;
    size_t count;
    
    // Check parameters
    assert( buf != NULL );
    assert( itemSize > 0 );

	if (items <= 0)
		return 0;

    assert( fd != NULL );
    
    // Load requesting data
    while ( (count = fread( buf, itemSize, items, fd )) < items )
    {
        loaded += count;
        if ( feof( fd ) )
            return loaded;
        if ( ferror( fd ) && errno != EINTR )
            return loaded;
        buf     = ((char *) buf) + itemSize * count;
        items  -= count;
    }
    
    loaded += count;
    
    return loaded;
}

size_t writen( const void *buf, size_t itemSize, size_t items, FILE *fd )
{
    size_t saved = 0;
    size_t count;
    
    // Check parameters
    assert( buf != NULL );
    assert( itemSize > 0 );
	if (items <= 0)
		return 0;
    assert( fd != NULL );
    
    // Save requesting data
    while ( (count = fwrite( buf, itemSize, items, fd )) < items )
    {
        saved  += count;
        if ( ferror( fd ) && errno != EINTR )
            return saved;
        buf     = ((char *) buf) + itemSize * count;
        items  -= count;
    }
    
    saved += count;
    
    return saved;
}

long file_size(FILE *fp)
{
	long curpos, length;
	curpos = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	length = ftell(fp);
	fseek(fp, curpos, SEEK_SET);
	return length;
}

void axel_log(FILE * fd, const char * s, ...)
{
	if (!fd)
		return;

	va_list ap;
	va_start(ap, s);

	char* buf = (char*)malloc(1024 * 100);
	if (buf != nullptr)
	{
		vsnprintf(buf, 1024 * 100, s, ap);

		fprintf(fd, "%s", buf);
		fflush(fd);

		free(buf);
	}
	va_end(ap);
}

#define axel_log

/* Create a new axel_t structure */
axel_t *
axel_new(conf_t *conf, int count, const void *url, const void *outdir, const char * rname, FILE * logfd)
{
	const search_t *res;
	axel_t *axel;
	int status;
	long delay;
	url_t *u;
	char *s;
	int i;

	axel = (axel_t*)malloc(sizeof(axel_t));
	if (!axel)
		goto nomem;

	memset(axel, 0, sizeof(axel_t));
	axel->logfd = logfd;
	axel->conf = conf;
	axel->conn = (conn_t*)malloc(sizeof(conn_t) * axel->conf->num_connections);
	if (!axel->conn)
		goto nomem;
	memset(axel->conn, 0, sizeof(conn_t) * axel->conf->num_connections);

	axel_log(logfd, "new lock %d\n", __LINE__);
	for (i = 0; i < axel->conf->num_connections; i++)
	{
		axel->conn[i].lock = new std::mutex();
		//pthread_mutex_init(&axel->conn[i].lock, NULL);
	}
	axel_log(logfd, "axel api line %d\n", __LINE__);

	if (axel->conf->max_speed > 0) {
		if ((float)axel->conf->max_speed / axel->conf->buffer_size <
		    0.5) {
			if (axel->conf->verbose >= 2)
				axel_message(axel,
					     _("Buffer resized for this speed."));
			axel->conf->buffer_size = axel->conf->max_speed;
		}
		delay = (int)((float)1000000000 / axel->conf->max_speed *
			      axel->conf->buffer_size *
			      axel->conf->num_connections);

		axel->delay_time.tv_sec = delay / 1000000000;
		axel->delay_time.tv_nsec = delay % 1000000000;
	}

	axel_log(logfd, "axel api line %d\n", __LINE__);

	if (axel->buffer == NULL) {
		/* reserve 4 additional bytes for file extension ".st" */
		axel->buffer = (char *)malloc(max(MAX_STRING + 4, axel->conf->buffer_size));
		if (!axel->buffer)
			goto nomem;
	}

	axel_log(logfd, "axel api line %d\n", __LINE__);
	if (!url) {
		axel_message(axel, _("Invalid URL"));
		axel_log(logfd, "Invalid URL %d\n", __LINE__);
		axel_close(axel);
		return NULL;
	}

	axel_log(logfd, "axel api line %d\n", __LINE__);

	if (count == 0) {
		axel->url = (url_t *)malloc(sizeof(url_t));
		if (!axel->url)
			goto nomem;

		axel->url->next = axel->url;
		strncpy(axel->url->text, (const char *)url, sizeof(axel->url->text) - 1);
	} else {
		res = (const search_t *)url;
		u = (url_t *)malloc(sizeof(url_t) * count);
		if (!u)
			goto nomem;
		axel->url = u;

		for (i = 0; i < count; i++) {
			strncpy(u[i].text, res[i].url, sizeof(u[i].text) - 1);
			u[i].next = &u[i + 1];
		}
		u[count - 1].next = u;
	}

	axel_log(logfd, "axel api line %d\n", __LINE__);

	axel->conn[0].conf = axel->conf;
	if (!conn_set(&axel->conn[0], axel->url->text)) {
		axel_message(axel, _("Could not parse URL.\n"));
		axel->ready = -1;
		return axel;
	}

	axel_log(logfd, "axel api line %d\n", __LINE__);

	axel->conn[0].local_if = axel->conf->interfaces->text;
	axel->conf->interfaces = (message_t*)axel->conf->interfaces->next;

	if (rname)
	{
		strcpy(axel->filename, rname);
	}
	else
	{
		strncpy(axel->filename, axel->conn[0].file, sizeof(axel->filename) - 1);
	}
	strncpy(axel->save_path, (char *)outdir, strlen((const char *)outdir));
	http_decode(axel->filename);

	axel_log(logfd, "axel api line %d\n", __LINE__);

	if (*axel->filename == 0)	/* Index page == no fn */
		strncpy(axel->filename, axel->conf->default_filename,
			sizeof(axel->filename) - 1);
	if ((s = strchr(axel->filename, '?')) != NULL &&
	    axel->conf->strip_cgi_parameters)
		*s = 0;		/* Get rid of CGI parameters */

	axel_log(logfd, "axel api line %d\n", __LINE__);

	if (axel->conf->no_clobber && access(axel->filename, F_OK) == 0) {
		char stfile[MAX_STRING + 3];

		sprintf(stfile, "%s/%s.st", axel->save_path, axel->filename);
		if (access(stfile, F_OK) == 0) {
			printf(_("Incomplete download found, ignoring "
				 "no-clobber option\n"));
		} else {
			printf(_("File '%s' already there; not retrieving.\n"),
			       axel->filename);
			axel->ready = -1;
			axel_log(logfd, _("File '%s' already there; not retrieving %d.\n"),axel->filename, __LINE__);
			return axel;
		}
	}

	axel_log(logfd, "axel api line %d\n", __LINE__);

	do {
		if (!conn_init(&axel->conn[0])) {
			axel_message(axel, axel->conn[0].message);
			axel->ready = -1;
			axel_log(logfd,"conn_init err:%s %d", axel->conn[0].message, __LINE__);
			return axel;
		}

		axel_log(logfd, "axel api line %d\n", __LINE__);

		/* This does more than just checking the file size, it all
		 * depends on the protocol used. */
		for (i = 0; i < axel->conf->retry_times; ++i)
		{
			status = conn_info(&axel->conn[0]);

			axel_log(logfd, "axel status %d , %s, %d\n", status, axel->conn[0].message, __LINE__);

			if (status == 1)
			{
				break;
			}
			else
			{
				if (i == axel->conf->retry_times - 1)
				{
					axel_message(axel, axel->conn[0].message);
					axel->ready = -1;
					axel_log(logfd, "conn_info err:%s %d", axel->conn[0].message, __LINE__);
					return axel;
				}
				else
				{
					axel_log(logfd, "conn_info err retrytimes:%d %s %d", i, axel->conn[0].message, __LINE__);
					struct timespec delay = { axel->conf->reconnect_delay, 0 };
					axel_nanosleep(delay);
					continue;
				}
			}
		}
	}
	/* re-init in case of protocol change. This can happen only once
	 * because the FTP protocol can't redirect back to HTTP */
	while (status == -1);

	axel_log(logfd, "axel api line %d\n", __LINE__);

	conn_url(axel->conn, axel->buffer);

	strncpy(axel->url->text, axel->buffer, sizeof(axel->url->text) - 1);
	axel->size = axel->conn[0].size;
	if (axel->conf->verbose > 0) {
		if (axel->size != LLONG_MAX) {
			axel_message(axel, _("File size: %lld bytes"),
				     axel->size);
		} else {
			axel_message(axel, _("File size: unavailable"));
		}
	}

	/* Wildcards in URL --> Get complete filename */
	if (strchr(axel->filename, '*') || strchr(axel->filename, '?'))
		strncpy(axel->filename, axel->conn[0].file,
			sizeof(axel->filename) - 1);

	if (*axel->conn[0].output_filename != 0) {
		strncpy(axel->filename, axel->conn[0].output_filename,
			sizeof(axel->filename) - 1);
	}

	axel_log(logfd, "axel api line %d\n", __LINE__);

	return axel;
 nomem:
	axel_close(axel);
	axel_log(logfd, "errno:%s %d", strerror(errno), __LINE__);
	printf("%s\n", strerror(errno));
	return NULL;
}

/* Open a local file to store the downloaded data */
int
axel_open(axel_t *axel)
{
	int i = 0;
	FILE* fd;
	ssize_t nread;

	/* Local file existence check */
	char select_name[MAX_STRING];
	strcpy(select_name, axel->filename);
	while (true)
	{
		sprintf(axel->buffer, "%s/%s", axel->save_path, select_name);
		if (access(axel->buffer, F_OK) == 0)
		{
			if (axel->conn[0].supported)
			{
				sprintf(axel->buffer, "%s/%s.st", axel->save_path, select_name);
				if (access(axel->buffer, F_OK) == 0)
				{
					strcpy(axel->filename, select_name);
					break;
				}
			}
		}
		else
		{
			strcpy(axel->filename, select_name);
			sprintf(axel->buffer, "%s/%s.st", axel->save_path, select_name);
			if (access(axel->buffer, F_OK))
				break;
		}
		sprintf(select_name, "%s.%i", axel->filename, i);
		i++;
	}


	if (axel->conf->verbose > 0)
		axel_message(axel, _("Opening output file %s"), axel->filename);
	snprintf(axel->buffer, MAX_STRING, "%s/%s.st", axel->save_path, axel->filename);

	axel->outfd = NULL;

	/* Check whether server knows about RESTart and switch back to
	   single connection download if necessary */
	if (!axel->conn[0].supported) {
		axel_message(axel, _("Server unsupported, "
				     "starting from scratch with one connection."));
		axel->conf->num_connections = 1;
		void *new_conn = realloc(axel->conn, sizeof(conn_t));
		if (!new_conn)
			return 0;

		axel->conn = (conn_t *)new_conn;
		axel_divide(axel);
	} 
	else
    {
        if ((fd = fopen(axel->buffer, "rb")) != NULL)
        {
           
            int old_format = 0;
            off_t stsize = (off_t)file_size(fd);
            fseek(fd, 0, SEEK_SET);

            nread = readn(&axel->conf->num_connections, sizeof(char),
                     sizeof(axel->conf->num_connections), fd);
            if (nread != sizeof(axel->conf->num_connections)) {
                printf(_("%s/%s.st: Error, truncated state file\n"),
                       axel->save_path, axel->filename);
                fclose(fd);
                unlink(axel->buffer);
                return 0;
            }

            if (axel->conf->num_connections < 1) {
                fprintf(stderr,
                    _("Bogus number of connections stored in state file\n"));
                fclose(fd);
                unlink(axel->buffer);
                return 0;
            }

            if (stsize < (sizeof(axel->conf->num_connections) +
                      sizeof(axel->bytes_done) +
                      2 * axel->conf->num_connections *
                      sizeof(axel->conn[0].currentbyte))) {
                /* FIXME this might be wrong, the file may have been
                 * truncated, we need another way to check. */
    #ifdef DEBUG
                printf(_("State file has old format.\n"));
    #endif
                old_format = 1;
            }

            void *new_conn = realloc(axel->conn, sizeof(conn_t) *
                         axel->conf->num_connections);
            if (!new_conn) {
                fclose(fd);
                return 0;
            }
            axel->conn = (conn_t *)new_conn;

            //memset(axel->conn + 1, 0,
            //       sizeof(conn_t) * (axel->conf->num_connections - 1));

            if (old_format)
                axel_divide(axel);

            nread = readn(&axel->bytes_done, sizeof(char), sizeof(axel->bytes_done), fd);
            if (nread != sizeof(axel->bytes_done))
            {
                axel_message(axel,
                             _("State file find err."),
                             axel->bytes_done, axel->size - axel->bytes_done);
                fclose(fd);
                unlink(axel->buffer);
                return 0;
            }

            axel->start_byte = axel->bytes_done;

            for (i = 0; i < axel->conf->num_connections; i++) {
                nread = readn(&axel->conn[i].currentbyte, sizeof(char), sizeof(axel->conn[i].currentbyte), fd);
                if (nread != sizeof(axel->conn[i].currentbyte))
                {
                    axel_message(axel,
                                 _("State file find err.."),
                                 axel->bytes_done, axel->size - axel->bytes_done);
                    fclose(fd);
                    unlink(axel->buffer);
                    return 0;
                }
                
                if (!old_format) {
                    nread = readn(&axel->conn[i].lastbyte, sizeof(char),
                             sizeof(axel->conn[i].lastbyte), fd);
                    if(nread != sizeof(axel->conn[i].lastbyte))
                    {
                        axel_message(axel,
                                     _("State file find err..."),
                                     axel->bytes_done, axel->size - axel->bytes_done);
                        fclose(fd);
                        unlink(axel->buffer);
                        return 0;
                    }
                }
            }

            axel_message(axel,
                     _("State file found: %lld bytes downloaded, %lld to go."),
                     axel->bytes_done, axel->size - axel->bytes_done);

            fclose(fd);

            snprintf(axel->buffer, MAX_STRING, "%s/%s", axel->save_path, axel->filename);
            if ((axel->outfd = fopen(axel->buffer, "rb+")) == NULL)
			{
                axel_message(axel, _("Error opening local file"));
                return 0;
            }
			else
			{
				if (axel->conn[0].supported)
				{
					snprintf(axel->buffer, MAX_STRING, "%s/%s.st", axel->save_path, axel->filename);
					if ((axel->statusfd = fopen(axel->buffer, "w")) == NULL)
					{
						axel_message(axel, _("Error opening st file"));
						return 0;		/* Not 100% fatal.. */
					}
				}
			}
        }
    }

	/* If outfd == -1 we have to start from scrath now */
	if (axel->outfd == NULL) {
		axel_divide(axel);

		snprintf(axel->buffer, MAX_STRING, "%s/%s", axel->save_path, axel->filename);
		if ((axel->outfd =
		     fopen(axel->buffer, "wb")) == NULL) {
			axel_message(axel, _("Error opening local file"));
			return 0;
		}
		else
		{
			if (axel->conn[0].supported)
			{
				snprintf(axel->buffer, MAX_STRING, "%s/%s.st", axel->save_path, axel->filename);
				if ((axel->statusfd = fopen(axel->buffer, "w")) == NULL)
				{
					axel_message(axel, _("Error opening st file"));
					return 0;		/* Not 100% fatal.. */
				}
			}
		}

		/* And check whether the filesystem can handle seeks to
		   past-EOF areas.. Speeds things up. :) AFAIK this
		   should just not happen: */
		if (fseek(axel->outfd, axel->size, SEEK_SET) != 0 &&
		    axel->conf->num_connections > 1)
        {
			/* But if the OS/fs does not allow to seek behind
			   EOF, we have to fill the file with zeroes before
			   starting. Slow.. */
			axel_message(axel,
				     _("Crappy filesystem/OS.. Working around. :-("));
			fseek(axel->outfd, 0, SEEK_SET);
			char * buffer = (char *)malloc(axel->conf->buffer_size);
			memset(buffer, 0, axel->conf->buffer_size);
			long long int j = axel->size;
			while (j > 0)
            {
				ssize_t nwrite;

				if ((nwrite =
				     writen(buffer, sizeof(char), min(j, axel->conf->buffer_size), axel->outfd)) < 0)
				{
					if (errno == EINTR || errno == EAGAIN)
						continue;
					axel_message(axel,
						     _("Error creating local file"));
					free(buffer);
					return 0;
				}
				j -= nwrite;
			}
			free(buffer);
		}
	}

	return 1;
}

void
reactivate_connection(axel_t *axel, int thread)
{
	long long int max_remaining = 0;
	int idx = -1;

	if (axel->conn[thread].enabled ||
	    axel->conn[thread].currentbyte <= axel->conn[thread].lastbyte)
		return;
	/* find some more work to do */
	for (int j = 0; j < axel->conf->num_connections; j++) {
		long long int remaining =
		    axel->conn[j].lastbyte - axel->conn[j].currentbyte + 1;
		if (remaining > max_remaining) {
			max_remaining = remaining;
			idx = j;
		}
	}
	/* do not reactivate for less than 100KB */
	if (max_remaining >= 100 * 1024 && idx != -1) {
#ifdef DEBUG
		printf(_("\nReactivate connection %d\n"), thread);
#endif
		axel->conn[thread].lastbyte = axel->conn[idx].lastbyte;
		axel->conn[idx].lastbyte =
		    axel->conn[idx].currentbyte + max_remaining / 2;
		axel->conn[thread].currentbyte = axel->conn[idx].lastbyte + 1;
	}
}

/* Start downloading */
void
axel_start(axel_t *axel)
{
	int i;
	url_t *url_ptr;

	/* HTTP might've redirected and FTP handles wildcards, so
	   re-scan the URL for every conn */
	url_ptr = axel->url;
	axel_log(axel->logfd, "axel this api line:%d\n", __LINE__);
	for (i = 0; i < axel->conf->num_connections; i++) {
		conn_set(&axel->conn[i], url_ptr->text);
		url_ptr = (url_t *)url_ptr->next;
		axel->conn[i].local_if = axel->conf->interfaces->text;
		axel->conf->interfaces = (if_t *)axel->conf->interfaces->next;
		axel->conn[i].conf = axel->conf;
		if (i)
			axel->conn[i].supported = true;
	}
	axel_log(axel->logfd, "axel this api line:%d\n", __LINE__);
	if (axel->conf->verbose > 0)
		axel_message(axel, _("Starting download"));

	axel_log(axel->logfd, "axel this api line:%d\n", __LINE__);
	for (i = 0; i < axel->conf->num_connections; i++)
		if (axel->conn[i].currentbyte > axel->conn[i].lastbyte) {
			reactivate_connection(axel, i);
		}
	axel_log(axel->logfd, "axel this api line:%d\n", __LINE__);

	for (i = 0; i < axel->conf->num_connections; i++)
		if (axel->conn[i].currentbyte <= axel->conn[i].lastbyte) {
			if (axel->conf->verbose >= 2) {
				axel_message(axel,
					     _("Connection %i downloading from %s:%i using interface %s"),
					     i, axel->conn[i].host,
					     axel->conn[i].port,
					     axel->conn[i].local_if);
			}

			axel->conn[i].state = true;

            assert(axel->conn[i].lock != NULL);
			auto th_setup_thread = new std::thread(setup_thread, &axel->conn[i]);
			
			axel_log(axel->logfd, "new thread %d\n", __LINE__);

			if (th_setup_thread)
			{
				axel->conn[i].setup_thread[0] = th_setup_thread;
			}
			else
			{
				axel_message(axel, _("thread error!!!"));
				axel->ready = -1;
				axel_log(axel->logfd, "axel->ready = -1 :%d\n", __LINE__);
			}
		}

	axel_log(axel->logfd, "axel this api line:%d\n", __LINE__);
	/* The real downloading will start now, so let's start counting */
	axel->start_time = gettime();
	axel->ready = 0;
}

/* Main 'loop' */
#include "cocos2d.h"
#ifndef max
#define max(a, b)		((a) > (b) ? (a) : (b))
#endif
void
axel_do(axel_t *axel)
{
	fd_set fds[1];
	int hifd, i;
	long long int remaining, size;
	struct timeval timeval[1];
	url_t *url_ptr;
	struct timespec delay = {0, 100000000};
	float max_speed_ratio;

	/* Create statefile if necessary */
	if (gettime() > axel->next_state) {
		save_state(axel);
		axel->next_state = gettime() + axel->conf->save_state_interval;
	}

	/* Wait for data on (one of) the connections */
	FD_ZERO(fds);
	hifd = 0;
	for (i = 0; i < axel->conf->num_connections; i++) {
		/* skip connection if setup thread hasn't released the lock yet */
		if (axel->conn[i].lock->try_lock())
		{
			if (axel->conn[i].enabled) {
				FD_SET(axel->conn[i].tcp->fd, fds);
				hifd = max(hifd, axel->conn[i].tcp->fd);
			}

			axel->conn[i].lock->unlock();
			//pthread_mutex_unlock(&axel->conn[i].lock);
		}
	}
	if (hifd == 0) {
		/* No connections yet. Wait... */
		if (axel_nanosleep(delay) < 0) {
			axel_message(axel,
				     _("Error while waiting for connection: %s"),
				     strerror(errno));
			axel->ready = -1;
			axel_log(axel->logfd, "axel->ready = -1 :%d\n", __LINE__);
			return;
		}
		goto conn_check;
	}

	timeval->tv_sec = 0;
	timeval->tv_usec = 100000;
	if (select(hifd + 1, fds, NULL, NULL, timeval) == -1) {
		/* A select() error probably means it was interrupted
		 * by a signal, or that something else's very wrong... */
		axel->ready = -1;
		axel_log(axel->logfd, "axel->ready = -1 :%d\n", __LINE__);
		return;
	}

	/* Handle connections which need attention */
	for (i = 0; i < axel->conf->num_connections; i++) {
		/* skip connection if setup thread hasn't released the lock yet */
		if (!axel->conn[i].lock->try_lock())
		{
			continue;
		}

		if (!axel->conn[i].enabled)
			goto next_conn;

		if (!FD_ISSET(axel->conn[i].tcp->fd, fds)) {
			time_t timeout = axel->conn[i].last_transfer +
			    axel->conf->connection_timeout;
			if (gettime() > timeout) {
				if (axel->conf->verbose)
					axel_message(axel,
						     _("Connection %i timed out"),
						     i);
				conn_disconnect(&axel->conn[i]);
			}
			goto next_conn;
		}

		axel->conn[i].last_transfer = gettime();
		size =
		    tcp_read(axel->conn[i].tcp, axel->buffer,
			     axel->conf->buffer_size);
		if (size == -1) {
			if (axel->conf->verbose) {
				axel_message(axel, _("Error on connection %i! "
						     "Connection closed"), i);
			}
			conn_disconnect(&axel->conn[i]);
			goto next_conn;
		}

		if (size == 0) {
			if (axel->conf->verbose) {
				/* Only abnormal behaviour if: */
				if (axel->conn[i].currentbyte <
				    axel->conn[i].lastbyte &&
				    axel->size != LLONG_MAX) {
					axel_message(axel,
						     _("Connection %i unexpectedly closed"),
						     i);
				} else {
					axel_message(axel,
						     _("Connection %i finished"),
						     i);
				}
			}
			if (!axel->conn[0].supported) {
				axel->ready = 1;
				axel_log(axel->logfd, "axel->ready = 1 :%d\n", __LINE__);
			}
			conn_disconnect(&axel->conn[i]);
			reactivate_connection(axel, i);
			goto next_conn;
		}

		/* remaining == Bytes to go */
		remaining =
		    axel->conn[i].lastbyte - axel->conn[i].currentbyte + 1;
		if (remaining < size) {
			if (axel->conf->verbose) {
				axel_message(axel, _("Connection %i finished"),
					     i);
			}
			conn_disconnect(&axel->conn[i]);
			size = remaining;
			/* Don't terminate, still stuff to write! */
		}
		/* This should always succeed.. */
		fseek(axel->outfd, axel->conn[i].currentbyte, SEEK_SET);
		if (writen(axel->buffer, sizeof(char), size, axel->outfd) != size) {
			axel_message(axel, _("Write error!"));
			axel->ready = -1;
			axel_log(axel->logfd, "axel->ready = -1 :%d\n", __LINE__);
			//pthread_mutex_unlock(&axel->conn[i].lock);
			axel->conn[i].lock->unlock();
			return;
		}
		axel->conn[i].currentbyte += size;
		axel->bytes_done += size;
		if (remaining == size)
			reactivate_connection(axel, i);

	next_conn:
		axel->conn[i].lock->unlock();
		//pthread_mutex_unlock(&axel->conn[i].lock);
	}

	if (axel->ready)
		return;

 conn_check:
	/* Look for aborted connections and attempt to restart them. */
	url_ptr = axel->url;
	for (i = 0; i < axel->conf->num_connections; i++) {
		/* skip connection if setup thread hasn't released the lock yet */
		if (!axel->conn[i].lock->try_lock())
		{
			continue;
		}

		if (!axel->conn[i].enabled &&
		    axel->conn[i].currentbyte < axel->conn[i].lastbyte)
		{
			if (!axel->conn[i].state)
			{
				// Wait for termination of this thread
			
				if (axel->conn[i].setup_thread[0]->joinable())
				{
					axel->conn[i].lock->unlock();
					axel->conn[i].setup_thread[0]->join();
					axel->conn[i].lock->lock();
				}

				conn_set(&axel->conn[i], url_ptr->text);
				url_ptr = (url_t *)url_ptr->next;
				/* axel->conn[i].local_if = axel->conf->interfaces->text;
				   axel->conf->interfaces = axel->conf->interfaces->next; */
				if (axel->conf->verbose >= 2)
					axel_message(axel,
						     _("Connection %i downloading from %s:%i using interface %s"),
						     i, axel->conn[i].host,
						     axel->conn[i].port,
						     axel->conn[i].local_if);

				//释放原先的安装线程
				delete axel->conn[i].setup_thread[0];

				axel->conn[i].state = true;
                assert(axel->conn[i].lock != NULL);
				auto th_setup_thread = new std::thread(setup_thread, &axel->conn[i]);

				axel_log(axel->logfd, "new thread %d\n", __LINE__);

				if (th_setup_thread)
				{
					axel->conn[i].setup_thread[0] = th_setup_thread;
					axel->conn[i].last_transfer = gettime();
				}
				else
				{
					axel_message(axel,
						_("thread error!!!"));
					axel->ready = -1;
					axel_log(axel->logfd, "axel->ready = -1 :%d\n", __LINE__);
				}
			}
			else
			{
				if (gettime() > (axel->conn[i].last_transfer +
						 axel->conf->reconnect_delay)) {
					//pthread_cancel(*axel->conn[i].setup_thread);
					axel->conn[i].state = false;

					if (axel->conn[i].setup_thread[0]->joinable())
					{
						axel->conn[i].lock->unlock();
						axel->conn[i].setup_thread[0]->join();
						axel->conn[i].lock->lock();
					}
				}
			}
		}

		axel->conn[i].lock->unlock();
		//pthread_mutex_unlock(&axel->conn[i].lock);
	}

	/* Calculate current average speed and finish_time */
	axel->bytes_per_second =
	    (int)((double)(axel->bytes_done - axel->start_byte) /
		  (gettime() - axel->start_time));
	if (axel->bytes_per_second != 0)
		axel->finish_time =
		    (int)(axel->start_time +
			  (double)(axel->size - axel->start_byte) /
			  axel->bytes_per_second);
	else
		axel->finish_time = INT_MAX;

	/* Check speed. If too high, delay for some time to slow things
	   down a bit. I think a 5% deviation should be acceptable. */
	if (axel->conf->max_speed > 0) {
		max_speed_ratio = (float)axel->bytes_per_second /
		    axel->conf->max_speed;
		if (max_speed_ratio > 1.05)
			axel->delay_time.tv_nsec += 10000000;
		else if ((max_speed_ratio < 0.95) &&
			 (axel->delay_time.tv_nsec >= 10000000))
			axel->delay_time.tv_nsec -= 10000000;
		else if ((max_speed_ratio < 0.95) &&
			 (axel->delay_time.tv_sec > 0)) {
			axel->delay_time.tv_sec--;
			axel->delay_time.tv_nsec += 999000000;
		} else
		    if (((float)axel->bytes_per_second / axel->conf->max_speed <
			 0.95)) {
			axel->delay_time.tv_sec = 0;
			axel->delay_time.tv_nsec = 0;
		}
		if (axel_nanosleep(axel->delay_time) < 0) {
			axel_message(axel,
				     _("Error while enforcing throttling: %s"),
				     strerror(errno));
			axel->ready = -1;
			axel_log(axel->logfd, "axel->ready = -1 :%d\n", __LINE__);
			return;
		}
	}

	/* Ready? */
	if (axel->bytes_done == axel->size)
	{
		axel->ready = 1;
		axel_log(axel->logfd, "axel->ready = 1 :%d\n", __LINE__);
	}
}

/* Close an axel connection */
void
axel_close(axel_t *axel)
{
	if (!axel)
		return;

	/* this function can't be called with a partly initialized axel */
	assert(axel->conn);

	/* Terminate threads and close connections */
	for (int i = 0; i < axel->conf->num_connections; i++) {
		/* don't try to kill non existing thread */
		if (axel->conn[i].setup_thread[0] != NULL) {
			if (axel->conn[i].setup_thread[0]->joinable())
				axel->conn[i].setup_thread[0]->join();
		}
		conn_disconnect(&axel->conn[i]);
	}

	free(axel->url);

	//print_messages(axel);

	for (int i = 0; i < axel->conf->num_connections; i++)
	{
		axel_log(axel->logfd, "delete lock :%p - %d\n", axel->conn[i].lock, __LINE__);
		if (axel->conn[i].lock)
        {
			delete axel->conn[i].lock;
            axel->conn[i].lock = NULL;
        }

		axel_log(axel->logfd, "delete thread start::%p - %d\n", axel->conn[i].setup_thread[0],  __LINE__);
		if (axel->conn[i].setup_thread[0])
        {
			delete axel->conn[i].setup_thread[0];
            axel->conn[i].setup_thread[0] = NULL;
        }
		axel_log(axel->logfd, "delete thread end::%p - %d\n", axel->conn[i].setup_thread[0], __LINE__);
		//pthread_mutex_init(&axel->conn[i].lock, NULL);
	}

	if (axel->bytes_done > 0)
	{
		if (axel->ready != 1)
		{
			save_state(axel);
		}
	}

	if (axel->outfd)
	{
		fclose(axel->outfd);
	}

	if (axel->statusfd)
	{
		fclose(axel->statusfd);
	}

	if (axel->ready == 1) 
	{
		snprintf(axel->buffer, MAX_STRING, "%s/%s.st", axel->save_path, axel->filename);
		unlink(axel->buffer);
	}
	else
	{
		if (axel->bytes_done <= 0)
		{
			snprintf(axel->buffer, MAX_STRING, "%s/%s.st", axel->save_path, axel->filename);
			unlink(axel->buffer);

			snprintf(axel->buffer, MAX_STRING, "%s/%s", axel->save_path, axel->filename);
			unlink(axel->buffer);
		}
	}

	free(axel->conn);
	axel_log(axel->logfd, "delete conn %d\n", __LINE__);
	free(axel->buffer);
	axel_log(axel->logfd, "delete buffer %d\n", __LINE__);
	free(axel);
}

/* time() with more precision */
double
gettime()
{
	struct timeval time[1];

	gettimeofday(time, 0);
	return (double)time->tv_sec + (double)time->tv_usec / 1000000;
}

/* Save the state of the current download */
bool
save_state(axel_t *axel)
{
	if (axel->statusfd == NULL)
		return false;

	int i;
	//FILE *fd;
	char fn[MAX_STRING + 4];
	ssize_t nwrite;

	/* No use for such a file if the server doesn't support
	   resuming anyway.. */
	if (!axel->conn[0].supported)
		return false;

	//snprintf(fn, sizeof(fn), "%s/%s.st", axel->save_path, axel->filename);
	//if ((fd = fopen(fn, "w")) == NULL) {
	//	return;		/* Not 100% fatal.. */
	//}

	fseek(axel->statusfd, 0, SEEK_SET);

	nwrite =
	    writen(&axel->conf->num_connections, sizeof(char),
		  sizeof(axel->conf->num_connections), axel->statusfd);

	if (nwrite != sizeof(axel->conf->num_connections))
	{
		fclose(axel->statusfd);
		axel->statusfd = NULL;
		snprintf(fn, MAX_STRING, "%s/%s.st", axel->save_path, axel->filename);
		unlink(fn);
		return false;
	}

	nwrite = writen(&axel->bytes_done, sizeof(char), sizeof(axel->bytes_done), axel->statusfd);
	if (nwrite != sizeof(axel->bytes_done))
	{
		fclose(axel->statusfd);
		axel->statusfd = NULL;
		snprintf(fn, MAX_STRING, "%s/%s.st", axel->save_path, axel->filename);
		unlink(fn);
		return false;
	}

	for (i = 0; i < axel->conf->num_connections; i++) {
		nwrite =
			writen(&axel->conn[i].currentbyte, sizeof(char),
			  sizeof(axel->conn[i].currentbyte), axel->statusfd);
		if (nwrite != sizeof(axel->conn[i].currentbyte))
		{
			fclose(axel->statusfd);
			axel->statusfd = NULL;
			snprintf(fn, MAX_STRING, "%s/%s.st", axel->save_path, axel->filename);
			unlink(fn);
			return false;
		}

		nwrite =
			writen(&axel->conn[i].lastbyte, sizeof(char),
			  sizeof(axel->conn[i].lastbyte), axel->statusfd);
		if (nwrite != sizeof(axel->conn[i].lastbyte))
		{
			fclose(axel->statusfd);
			axel->statusfd = NULL;
			snprintf(fn, MAX_STRING, "%s/%s.st", axel->save_path, axel->filename);
			unlink(fn);
			return false;
		}
	}

	fflush(axel->statusfd);
    
	return true;
	//fclose(fd);
}

/* Thread used to set up a connection */
void *
setup_thread(void *c)
{
	conn_t *conn = (conn_t *)c;

	conn->lock->lock();

	if (conn_setup(conn)) {
		conn->last_transfer = gettime();
		if (conn_exec(conn)) {
			conn->last_transfer = gettime();
			conn->enabled = true;
			goto out;
		}
	}

	//printf("--- setup_thread:%p, LINE:%d\n", conn, __LINE__);
	conn_disconnect(conn);
 out:
	conn->state = false;
	conn->lock->unlock();
	//pthread_mutex_unlock(&conn->lock);

	//printf("--- setup_thread:%p, LINE:%d\n", conn, __LINE__);
	return NULL;
}

/* Add a message to the axel->message structure */
static void
axel_message(axel_t *axel, const char *format, ...)
{
	message_t *m;
	va_list params;

	if (!axel)
		goto nomem;

	m = (message_t *)malloc(sizeof(message_t));
	if (!m)
		goto nomem;

	memset(m, 0, sizeof(message_t));
	va_start(params, format);
	vsnprintf(m->text, MAX_STRING, format, params);
	va_end(params);

	if (axel->message == NULL) {
		axel->message = axel->last_message = m;
	} else {
		axel->last_message->next = m;
		axel->last_message = m;
	}

	return;

 nomem:
	/* Flush previous messages */
	//print_messages(axel);
	va_start(params, format);
	vprintf(format, params);
	va_end(params);
}

/* Divide the file and set the locations for each connection */
static void
axel_divide(axel_t *axel)
{
	int i;

	axel->conn[0].currentbyte = 0;
	axel->conn[0].lastbyte = axel->size / axel->conf->num_connections - 1;
	for (i = 1; i < axel->conf->num_connections; i++) {
#ifdef DEBUG
		printf(_("Downloading %lld-%lld using conn. %i\n"),
		       axel->conn[i - 1].currentbyte,
		       axel->conn[i - 1].lastbyte, i - 1);
#endif
		axel->conn[i].currentbyte = axel->conn[i - 1].lastbyte + 1;
		axel->conn[i].lastbyte =
		    axel->conn[i].currentbyte +
		    axel->size / axel->conf->num_connections;
	}
	axel->conn[axel->conf->num_connections - 1].lastbyte = axel->size - 1;
#ifdef DEBUG
	printf(_("Downloading %lld-%lld using conn. %i\n"),
	       axel->conn[i - 1].currentbyte, axel->conn[i - 1].lastbyte,
	       i - 1);
#endif
}
