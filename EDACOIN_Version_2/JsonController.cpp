#include "JsonController.h"
#include "json.hpp"

using json = nlohmann::json;

JsonController::JsonController(BlockchainModel& modelo) : Controller(modelo) {
}

JsonController::~JsonController()
{
}

void JsonController::update(void* objeto) {
	static int prevIndex = -1;

	if (m.getFileSelected()) {
		if (m.getFileIndex() != prevIndex) {
			prevIndex = m.getFileIndex();
			json j;
			std::ifstream i(m.getFilePath(m.getFileIndex()).string());
			i >> j;
			m.clearBlockChain();
			saveBlocksFromJson(j);
		}
	}
}

void JsonController::cycle() {
	dispatch();
}

void JsonController::getEvent() {
}

void JsonController::dispatch() {
}

void JsonController::saveBlocksFromJson(json j) {
	if (!j.empty() && j.is_array())

	{
		vector<string> vTx;
		for (auto& element : j)
		{
			auto height = element["height"];
			auto nonce = element["nonce"];
			auto blockid = element["blockid"];
			auto previousblockid = element["previousblockid"];
			auto merkleroot = element["merkleroot"];
			auto nTx = element["nTx"];
			m.appendBlock(createTxArray(element["tx"]), height, nonce, blockid, previousblockid, merkleroot, nTx);
		}
	}
}

vector<string> JsonController::createTxArray(json j)
{
	vector<string> vTx;
	vector<string> strSum;
	for (auto& element : j)
	{
		string concatenatedIds = "";
		for (int i = 0; i < element["nTxin"]; i++) {
			string aux = ((element["vin"])[i])["txid"];
			concatenatedIds += aux;
		}
		vTx.push_back(concatenatedIds);
	}
	return vTx;
}