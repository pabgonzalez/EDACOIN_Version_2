#include "SPVNode.h"

void SPVNode::sendFilter(string nodeid) {
	if (isNeighbour(nodeid)) {
		json j;
		j["Id"] = ID;
		string aux = j;
		httpPost(nodeid, "/eda_coin/send_filter/", j);
	}
}

void SPVNode::getBlockHeader(string nodeid, string blockid) {
	if (isNeighbour(nodeid)) {
		httpGet(nodeid, "/eda_coin/get_block_header/", "block_id:" + blockid);
	}
}

json SPVNode::generateFilter() {
	json j;
	j["Id"] = ID;
	return j;
}