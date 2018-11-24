
int main(int argc, char *argv[])
{
	const char *type = "macvlan";
	uint32_t mode = MACVLAN_MODE_PRIVATE;
	const char *name = NULL;
	const char *dev = NULL;
	struct iplink_req req;
	struct rtnl_handle rth = { .fd = -1 };
	unsigned int ifindex = 0;
	int len = 0;

// do_iplink   iplink.c
	req.n.nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_EXCL;
	req.n.nlmsg_type = RTM_NEWLINK;
	req.i.ifi_family = AF_UNSPEC;

	struct rtattr *linkinfo = NLMSG_TAIL(&req.n);
	addattr_l(&req.n, sizeof(req), IFLA_LINKINFO, NULL, 0);
	addattr_l(&req.n, sizeof(req), IFLA_INFO_KIND, type, strlen(type));
	
	struct rtattr *data = NLMSG_TAIL(&req.n);
	addattr_l(&req.n, sizeof(req), IFLA_INFO_DATA, NULL, 0);

	addattr32(&req.n, 1024, IFLA_MACVLAN_MODE, mode);

	data->rta_len = (void *)NLMSG_TAIL(&req.n) - (void *)data;
	linkinfo->rta_len = (void *)NLMSG_TAIL(&req.n) - (void *)linkinfo;

	addattr_l(&req.n, sizeof(req), IFLA_LINK, &ifindex, 4);

	addattr_l(&req.n, sizeof(req), IFLA_IFNAME, name, len);

