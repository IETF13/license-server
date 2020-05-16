#include "client.h"
using namespace std;

int main(int argc, char** argv) {
	if (argc != 4)
		printf("Arg numbers wrong!\n");
	else if (strlen(argv[3]) != PASSWORDLEN)
		printf("Password length wrong!");
	else {
		client Client(argv[1], atoi(argv[2]), argv[3]);
		Client.running();
	}
}

