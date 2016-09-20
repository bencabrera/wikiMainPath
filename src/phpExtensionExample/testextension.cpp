#include <phpcpp.h>

#include <iostream>
#include <string>

extern "C" {

	void testfunc()
	{ 
		Php::out << "Hallo Welt!" << std::endl;
	}

	PHPCPP_EXPORT void *get_module() 
	{
		static Php::Extension extension("yourextension", "1.0");
		extension.add<testfunc>("testfunc");

		return extension;
	}
}
