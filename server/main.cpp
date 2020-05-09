#include "server.h"
using namespace std;

int main(int argc, char** argv) {
	if (argc != 3)
		printf("arg numbers wrong!\n");
	else {
		server Server(argv[1], atoi(argv[2]));
		Server.running();
	}
}

