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
			m.appendBlock(createBlock(element));
		}
	}
}

Block createBlock(json j) {
	if (!j.empty() && j.is_object())
	{
		vector<string> vTx;
		auto height = j["height"];
		auto nonce = j["nonce"];
		auto blockid = j["blockid"];
		auto previousblockid = j["previousblockid"];
		auto merkleroot = j["merkleroot"];
		auto nTx = j["nTx"];
		return Block(createTxVector(j["tx"]), height, nonce, blockid, previousblockid, merkleroot, nTx);
	}
	return Block();
}

vector<Transaction> createTxVector(json j) {
	vector<Transaction> vTx;
	if (!j.empty()) {
		for (auto& element : j) {
			vTx.push_back(createTx(element));
		}
		return vTx;
	}
	else
		vTx = {};
		return vTx;
}

Transaction createTx(json j) {
	Transaction tx;
	if (!j.empty() && j.is_object()) {
		tx.txid = j["txid"];
		tx.nTxin = j["nTxin"];
		for (int n = 0; n < j["nTxin"]; n++) {
			vinType vin;
			vin.blockid = ((j["vin"])[n])["blockid"];
			vin.txid = ((j["vin"])[n])["txid"];
			tx.appendVin(vin);
		}
		tx.nTxout = j["nTxout"];
		for (int n = 0; n < j["nTxout"]; n++) {
			voutType vout;
			vout.publicid = ((j["vout"])[n])["publicid"];
			vout.amount = ((j["vout"])[n])["amount"];
			tx.appendVout(vout);
		}
	}
	return tx;
}

Filter_ createFilter(json j) {
	Filter_ f = {"", "", 0};
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