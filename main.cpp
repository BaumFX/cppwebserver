#include "web_server.h"

void main()
{
	web_server yes("127.0.0.1", 80);
	if (yes.init() != 0)
		return;

	yes.run();
	system("pause");
}