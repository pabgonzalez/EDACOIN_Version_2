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
			m.appendBlock(createTxVector(element["tx"]), height, nonce, blockid, previousblockid, merkleroot, nTx);
		}
	}
}

vector<Transaction> JsonController::createTxVector(json j) {
	vector<Transaction> vTx = { "", 0, vinType {"", ""}, 0, voutType {"", 0} };
	if (!j.empty()) {
		for (auto& element : j) {
			for (int m = 0; m < element["nTx"]; m++) {
				vTx[m].nTxin = element["nTxin"];
				for (int n = 0; n < element["nTxin"]; n++) {
					vinType vin;
					vin.blockid = ((element["vin"])[n])["blockid"];
					vin.txid = ((element["vin"])[n])["txid"];
					vTx[m].appendVin(vin);
				}
				vTx[m].nTxout = element["nTxin"];
				for (int n = 0; n < element["nTxout"]; n++) {
					voutType vout;
					vout.publicid = ((element["vout"])[n])["publicid"];
					vout.amount = ((element["vout"])[n])["amount"];
					vTx[m].appendVout(vout);
				}
			}
		}
		return vTx;
	}
	else
		return vTx;
}

Filter JsonController::createFilter(json j) {
	Filter f = {"", "", 0};
	if (!j.empty()) {
		for (auto& element : j) {
			f.publicid = element["publicid"];
			f.ip = element["ip"];
			f.port = element["port"];
		}
		return f;
	}
	else
		return f;
}