// For more info:
// man 7 rtnetlink
// man 3 libnetlink
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <net/if.h>
#include <libnetlink.h>

struct iplink_req {
	struct nlmsghdr         n;
	struct ifinfomsg        i;
	char                    buf[1024];
};

int main(int argc, char *argv[])
{
	const char *dev = NULL;
	struct iplink_req req;
	struct rtnl_handle rth = { .fd = -1 };
	unsigned int ifindex = 0;
	int len = 0;

	if (argc < 2) {
		fprintf(stderr, "usage: rmif <dev>\n");
		exit(1);
	}

	dev = argv[1];
	ifindex = if_nametoindex(dev);
	if (!ifindex) {
		fprintf(stderr, "Invalid device %s\n", dev);
		exit(1);
	}

	len = strlen(dev);
	if (len > IFNAMSIZ) {
		fprintf(stderr, "Interface name too long: %s\n", dev);
		exit(1);
	}

	if (rtnl_open(&rth, 0)) {
		fprintf(stderr, "Cannot open rtnetlink!\n");
		exit(1);
	}

	memset(&req, 0, sizeof(req));
	req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
	req.n.nlmsg_flags = NLM_F_REQUEST;
	req.n.nlmsg_type = RTM_DELLINK;
	req.i.ifi_family = AF_UNSPEC;
	req.i.ifi_index = ifindex;

	addattr_l(&req.n, sizeof(req), IFLA_IFNAME, dev, len);

	if (rtnl_talk(&rth, &req.n, 0, 0) < 0) {
		fprintf(stderr, "failed to talk to netlink!\n");
		exit(1);
	}

	rtnl_close(&rth);

	printf("Deleted interface %s\n", dev);
}

