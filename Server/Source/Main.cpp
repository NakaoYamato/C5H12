#include "ServerAssignment.h"

int main()
{
	bool loop = true;
	do
	{
		std::cout << "00FENLServer Sample" << std::endl;
		int input = 0;
		std::cin >> input;
		switch (input)
		{
		case 0:
		{
			ServerAssignment server = ServerAssignment();
			server.Execute();
			break;
		}
		default:
		{
			loop = false;
			break;
		}
		}
	} while (loop);
}