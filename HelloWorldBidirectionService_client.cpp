#include <iostream>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "HelloWorldBidirectionService.h"

#include <boost/thread.hpp>


using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

//using namespace tutorial;
using boost::shared_ptr;
//using namespace boost;
using boost::thread;

#define BOOST_DATE_TIME_SOURCE
#define BOOST_THREAD_NO_LIB


class HelloWorldBidirectionFace :public HelloWorldBidirectionServiceIf
{
public:
	void SayHello(const std::string& msg) {
		// Your implementation goes here
		/*printf("%s\n", msg);
		printf("%s\n", &msg);*/

		cout << msg << endl;
		//cout << &msg << endl;
	}
};

static void Run(boost::shared_ptr<TTransport> sock)
{
	boost::shared_ptr<HelloWorldBidirectionFace> handler(new HelloWorldBidirectionFace());
	boost::shared_ptr<TProcessor> processor(new HelloWorldBidirectionServiceProcessor(handler));

	//boost::shared_ptr<HelloWorldBidirectionServiceProcessor> processor(new HelloWorldBidirectionFace());
	try
	{
		boost::shared_ptr<TProtocol> inProtocol(new TBinaryProtocol(sock));
		boost::shared_ptr<TProtocol> outProtocol(new TBinaryProtocol(sock));
		while (processor->process(inProtocol, outProtocol, "proc"))
		{
			printf("wait next msg\n");
		}
	}
	catch (TException& tx)
	{
		printf("connect close\n");
		cout << "ERROR: " << tx.what() << endl;
	}
}

static void RecFromConsole(boost::shared_ptr<HelloWorldBidirectionServiceClient> client)
{
	string str;
	cout << "input exit stop" << endl;
	cin >> str;
	while (str.compare("exit"))
	{
		client->SayHello(str);

		cin >> str;
	}
}


int main()
{
	boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
	boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	boost::shared_ptr<HelloWorldBidirectionServiceClient> client(new HelloWorldBidirectionServiceClient(protocol));

	try {
		transport->open();

		boost::thread mythread(Run, socket);

		boost::thread mythread2(RecFromConsole, client);
		mythread2.join();

		transport->close();
	}
	catch (TException& tx) {
		cout << "ERROR: " << tx.what() << endl;
	}
}