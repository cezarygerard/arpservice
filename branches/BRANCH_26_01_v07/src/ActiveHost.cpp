/**
* @author Cezary Zawadka 
* @date 2010.01.17
* @version 0.9
*
*	
*/

#include "ActiveHost.hpp"

ActiveHost::ActiveHost() : ttl(MAX_TTL), start("0000-00-00-00-00-00"), stop("0000-00-00-00-00-00")

{
	ip;
	mac;
	netaddr;
	ip.ip[0] = 0;
	ip.ip[1] = 0;
	ip.ip[2] = 0;
	ip.ip[3] = 0;
	
	mac.m[0] = 0;
	mac.m[1] = 0;
	mac.m[2] = 0;
	mac.m[3] = 0;
	mac.m[4] = 0;
	mac.m[5] = 0;
	
	netaddr.ip[0] = 0;
	netaddr.ip[1] = 0;
	netaddr.ip[2] = 0;
	netaddr.ip[3] = 0; 
}

ActiveHost::~ActiveHost()
{

}
