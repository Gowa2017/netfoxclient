/*
  Axel -- A lighter download accelerator for Linux and other Unices

  Copyright 2001-2007 Wilmer van der Gaast
  Copyright 2010      Mark Smith
  Copyright 2016-2017 Stephen Thirlwall
  Copyright 2017      Antonio Quartulli
  Copyright 2017      Ismael Luceno

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

/* TCP control file */

#include "axel.h"

#include <unordered_map>

std::mutex g_dnscache_lock;
bool g_open_dns_cache = false;
std::unordered_map<std::string, struct addrinfo *> g_dnscache;

static void
tcp_error(char *buffer, char *hostname, int port, const char *reason)
{
	sprintf(buffer, _("Unable to connect to server %s:%i: %s\n"),
		hostname, port, reason);
}

/* Get a TCP connection */
int
tcp_connect(tcp_t *tcp, char *hostname, int port, int secure, char *local_if,
	    char *message, unsigned io_timeout)
{
	struct sockaddr_in local_addr;
	const int portstr_len = 10;
	char portstr[portstr_len];
	struct addrinfo ai_hints;
	struct addrinfo *gai_results, *gai_result;
	int ret;
	int sock_fd = -1;

	memset(&local_addr, 0, sizeof(local_addr));
	if (tcp->ai_family == AF_INET && local_if && *local_if) {
		local_addr.sin_family = AF_INET;
		local_addr.sin_port = 0;
		local_addr.sin_addr.s_addr = inet_addr(local_if);
	}

	snprintf(portstr, portstr_len, "%d", port);

	memset(&ai_hints, 0, sizeof(ai_hints));
	ai_hints.ai_family = tcp->ai_family;
	ai_hints.ai_socktype = SOCK_STREAM;
	ai_hints.ai_flags = AI_ADDRCONFIG;
	ai_hints.ai_protocol = 0;

	g_dnscache_lock.lock();
	if (g_open_dns_cache)
	{
		auto it = g_dnscache.find(hostname);
		if (it == g_dnscache.end())
		{
			ret = getaddrinfo(hostname, portstr, &ai_hints, &gai_results);
			if (ret != 0) {
				tcp_error(message, hostname, port, strerror(ret));
				g_dnscache_lock.unlock();
				return -1;
			}
			g_dnscache.emplace(hostname, gai_results);
		}
		else
		{
			ret = 0;
			gai_results = it->second;
		}
	}
	else
	{
		ret = getaddrinfo(hostname, portstr, &ai_hints, &gai_results);
		if (ret != 0) {
			tcp_error(message, hostname, port, strerror(ret));
			g_dnscache_lock.unlock();
			return -1;
		}
	}

	g_dnscache_lock.unlock();

	gai_result = gai_results;
	sock_fd = -1;
	while ((sock_fd == -1) && (gai_result != NULL)) {

		sock_fd = socket(gai_result->ai_family,
				 gai_result->ai_socktype,
				 gai_result->ai_protocol);

		if (sock_fd != -1) {

			if (gai_result->ai_family == AF_INET) {
				if (local_if && *local_if) {
					ret = bind(sock_fd,
						   (struct sockaddr *)
						   &local_addr,
						   sizeof(local_addr));
					if (ret == -1) {
#ifndef WIN32
						close(sock_fd);
#else
						closesocket(sock_fd);
#endif
						sock_fd = -1;
						gai_result =
						    gai_result->ai_next;
					}
				}
			}

			if (sock_fd != -1) {
				struct timeval tout;
                tout.tv_sec = io_timeout;
                tout.tv_usec = 0;
				/* Set O_NONBLOCK so we can timeout */
				if (io_timeout)
				{
#ifndef WIN32
					fcntl(sock_fd, F_SETFL, O_NONBLOCK);
#else
					u_long flags;
					flags = 1;
					ioctlsocket(sock_fd, FIONBIO, &flags);
#endif
				}

				ret = connect(sock_fd, gai_result->ai_addr,
					      gai_result->ai_addrlen);
				
				/* Wait for the connection */
#ifndef WIN32
				if (ret == -1 && errno == EINPROGRESS) {
#else
				if (ret == -1 && h_errno == WSAEWOULDBLOCK) {
#endif
					fd_set fdset;
					FD_ZERO(&fdset);
					FD_SET(sock_fd, &fdset);
					ret = select(sock_fd + 1,
						     NULL, &fdset, NULL,
						     &tout);
				}
				if (ret == -1) {
#ifndef WIN32
					close(sock_fd);
#else
					closesocket(sock_fd);
#endif
					sock_fd = -1;
					gai_result = gai_result->ai_next;
				} else {
#ifndef WIN32
					fcntl(sock_fd, F_SETFL, 0);
#else
					unsigned long flags;
					flags = 0;
					ioctlsocket(sock_fd, FIONBIO, &flags);
#endif
				}
			}
		}
	}

	g_dnscache_lock.lock();
	if (!g_open_dns_cache)
		freeaddrinfo(gai_results);
	g_dnscache_lock.unlock();

	if (sock_fd == -1) {
		tcp_error(message, hostname, port, strerror(errno));
		return -1;
	}
#ifdef HAVE_SSL
	if (secure) {
		tcp->ssl = ssl_connect(sock_fd, hostname, message);
		if (tcp->ssl == NULL) {
#ifndef WIN32
			close(sock_fd);
#else
			closesocket(sock_fd);
#endif
			return -1;
		}
	}
#endif				/* HAVE_SSL */
	tcp->fd = sock_fd;

	/* Set I/O timeout */
        struct timeval tout;
        tout.tv_sec = io_timeout;
        tout.tv_usec = 0;
	setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tout, sizeof(tout));
	setsockopt(sock_fd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tout, sizeof(tout));

	return 1;
}

int
tcp_read(tcp_t *tcp, void *buffer, int size)
{
#ifdef HAVE_SSL
	if (tcp->ssl != NULL)
		return SSL_read(tcp->ssl, buffer, size);
	else
#endif				/* HAVE_SSL */
		return recv(tcp->fd, (char *)buffer, size, 0);
}

int
tcp_write(tcp_t *tcp, void *buffer, int size)
{
#ifdef HAVE_SSL
	if (tcp->ssl != NULL)
		return SSL_write(tcp->ssl, buffer, size);
	else
#endif				/* HAVE_SSL */
		return send(tcp->fd, (const char *)buffer, size, 0);
}

void
tcp_close(tcp_t *tcp)
{
	if (tcp->fd > 0) {
#ifdef HAVE_SSL
		if (tcp->ssl != NULL) {
			ssl_disconnect(tcp->ssl);
			tcp->ssl = NULL;
		} else
#endif				/* HAVE_SSL */
#ifndef WIN32
			close(tcp->fd);
#else
			closesocket(tcp->fd);
#endif
		tcp->fd = -1;
	}
}

void tcp_dns_cache(bool cache)
{
	g_dnscache_lock.lock();
	g_open_dns_cache = cache;
	if (!cache)
	{
		for (auto & obj : g_dnscache)
		{
			freeaddrinfo(obj.second);
		}
		g_dnscache.clear();
	}
	g_dnscache_lock.unlock();
}

void tcp_free_dns_cache()
{
	g_dnscache_lock.lock();
	for (auto & obj : g_dnscache)
	{
		freeaddrinfo(obj.second);
	}
	g_dnscache.clear();
	g_dnscache_lock.unlock();
}

int
get_if_ip(char *iface, char *ip)
{
#ifndef WIN32
	struct ifreq ifr;
	int ret, fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

	if (fd < 0)
		return 0;

	memset(&ifr, 0, sizeof(struct ifreq));

	strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name) - 1);
	ifr.ifr_name[sizeof(ifr.ifr_name) - 1] = '\0';
	ifr.ifr_addr.sa_family = AF_INET;

	ret = !ioctl(fd, SIOCGIFADDR, &ifr);
	if (ret) {
		struct sockaddr_in *x = (struct sockaddr_in *)&ifr.ifr_addr;
		strcpy(ip, inet_ntoa(x->sin_addr));
	}
#ifndef WIN32
	close(fd);
#else
	closesocket(fd);
#endif

	return ret;
#else
	int count;
	char hostname[128];
	struct hostent* inaddrs;
	if (gethostname(hostname, 128) == 0) {
		inaddrs = gethostbyname(hostname);
		if (inaddrs) {
			count = inaddrs->h_length / sizeof(in_addr);

			for (int i = 0; i < count; i++) {
				ip[i] = *(unsigned long*)inaddrs->h_addr_list[i];
			}
		}
	}
	return 1;
#endif
}
