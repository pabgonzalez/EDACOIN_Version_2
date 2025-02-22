#include <boost/filesystem.hpp>
#include <iostream>
#include <vector>
#include "BlockchainModel.h"
#include "BlockViewerController.h"
#include "JsonController.h"
#include "NodeController.h"
#include "BlockViewer.h"
#include "ImGuiObserverManager.h"
#include "FullNode.h"
#include "SPVNode.h"
#include "LocalNodes.h"
#include <cstdlib>
#include <ctime>
#include <time.h>

using namespace std;
using namespace boost::filesystem;

int main() {
	//srand(time(NULL));
	ImGuiObserverManager obsManager;
	if (obsManager.getError() == false)
	{
		//BlockchainModel model;
		//BlockViewerController blockViewerControl(model);
		//JsonController jsonControl(model);
		//BlockViewer view;
		//model.attach(view);
		//model.attach(blockViewerControl);
		//model.attach(jsonControl);
		//obsManager.addObserver(&blockViewerControl);
		//obsManager.addObserver(&view);

		LocalNodes localNodes("manifest.json");
		localNodes.addFullNode({"127.0.0.1", 1000}, "Jorge");
		localNodes.addFullNode({"127.0.0.1", 1001 }, "Ricardo");
		localNodes.addFullNode({"127.0.0.1", 1002 }, "Ernesto");
		NodeController nodeControl(localNodes);
		obsManager.addObserver(&nodeControl);

		while (obsManager.getExit() == false) {
			obsManager.cycle();
			//jsonControl.cycle();
		}
	}
	else
		char c = getchar();
}

//int main()
//{
//	SocketType s = {"IP", 22};
//	Node n(s, "myNode");
//	MerkleNode n1,n2,n3,n4,n5,n6,n7,n8,n9,n10,n11,n12,n13;
//	n1.level = n2.level = n3.level = n4.level = n5.level = n6.level = 3;
//	n7.level = n8.level = n9.level = n10.level = 2;
//	n11.level = n12.level = 1;
//	n13.level = 0;
//	n1.newIDstr = "a";
//	n2.newIDstr = "b";
//	n3.newIDstr = "c";
//	n4.newIDstr = "d";
//	n5.newIDstr = "e";
//	n6.newIDstr = "f";
//	n7.newIDstr = "g";
//	n8.newIDstr = "h";
//	n9.newIDstr = "i";
//	n10.newIDstr = "j";
//	n11.newIDstr = "k";
//	n12.newIDstr = "l";
//	n13.newIDstr = "m";
//	vector<MerkleNode> v = { n1,n2,n3,n4,n5,n6,n7,n8,n9,n10,n11,n12,n13 };
//	vector<string> out = n.recursiveMerkleBlock(v, 2);
//	for (int i = 0; i < out.size(); i++)
//	{
//		cout << out[i] << endl;
//	}
//}

//int main() {
//	srand(time(NULL));
//	map<string, SocketType> m = { {"id1", {"1", 1}}, {"id2", {"2", 2}}, {"id3", {"3", 3}}, {"id4", {"4", 4}},
//	{"id5", {"5", 5}}, {"id6", {"6", 6}}, {"id7", {"7", 7}}, {"id8", {"8", 8}}, {"id9", {"9", 9}},
//	{ "id10", {"10", 10} }, { "id11", {"11", 11} }, { "id12", {"12", 12} }, { "id13", {"13", 13} }, { "id14", {"14", 14} },
//	{ "id15", {"15", 15} }, { "id16", {"16", 16} }, { "id17", {"17", 17} }, { "id18", {"18", 18} }, { "id19", {"19", 19} } };
//	FullNode fn({"0", 0}, "id0", m);
//	vector<vector<bool>> v = fn.p2pAlgorithm(m);
//}