#include "network.h"

void fmt_sockaddr(pid_t pid, unsigned long addr_ptr, socklen_t addr_len,
                     char *out, size_t out_size)
{
    if (!addr_ptr || addr_len == 0) {
        snprintf(out, out_size, "NULL");
        return;
    }

    struct sockaddr_storage ss;
    size_t copy_size = addr_len < sizeof(ss) ? addr_len : sizeof(ss);
    if (fast_decode(pid, addr_ptr, (char *)&ss, copy_size) !=
        0) {
        snprintf(out, out_size, "Failed to decode socket address");
        return;
    }

    switch (ss.ss_family) {
    case AF_INET: {
        struct sockaddr_in *sin = (struct sockaddr_in *)&ss;
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &sin->sin_addr, ip, sizeof(ip));
        snprintf(out, out_size, "{AF_INET, %s, %d}", ip, ntohs(sin->sin_port));
        break;
    }
    case AF_INET6: {
        struct sockaddr_in6 *sin6 = (struct sockaddr_in6*)&ss;
        char ip[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &sin6->sin6_addr, ip, sizeof(ip));
        snprintf(out, out_size, "{AF_INET6, [%s]:%d}", ip, ntohs(sin6->sin6_port));
        break;
    }
    case AF_UNIX: {
        struct sockaddr_un *sun = (struct sockaddr_un*)&ss;
        snprintf(out, out_size, "{AF_UNIX, \"%s\"}",
                    sun->sun_path[0] ? sun->sun_path : "<abstract>");
        break;
    }
    case AF_UNSPEC:
        snprintf(out, out_size, "{AF_UNSPEC}");
        break;
    default:
        snprintf(out, out_size, "{family=%d, <unhandled>}", ss.ss_family);
    }
    
}