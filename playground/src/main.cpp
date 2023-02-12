#include "app.hpp"

#include <spdlog/spdlog.h>
#include <iostream>
#include <string>

// #include <dependencies/angelscript/angelscript.h>
#include <dependencies/angelscript/include/angelscript.h>
#include <dependencies/angelscript/add_on/scriptstdstring/scriptstdstring.h>
// #include <dependencies/angelscript/add_on/scriptstdstring/scriptstdstring.h>
// #include <dependencies/angelscript/add_on/scriptstdstring/scriptstdstring.h>
#include <dependencies/angelscript/add_on/scriptbuilder/scriptbuilder.h>
// #include <scriptstdstring/scriptstdstring.h>
// #include <scriptbuilder/scriptbuilder.h>

void MessageCallback(const asSMessageInfo *msg, void *param)
{
	const char *type = "ERR ";
	if( msg->type == asMSGTYPE_WARNING )
		type = "WARN";
	else if( msg->type == asMSGTYPE_INFORMATION )
		type = "INFO";

	printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

int CompileScript(asIScriptEngine *engine)
{
	int r;

	// We will load the script from a file on the disk.
	FILE *f = fopen("script.as", "rb");
	if( f == 0 )
	{
		std::cout << "Failed to open the script file 'script.as'." << std::endl;
		return -1;
	}

	// Determine the size of the file
	fseek(f, 0, SEEK_END);
	int len = ftell(f);
	fseek(f, 0, SEEK_SET);

	// On Win32 it is possible to do the following instead
	// int len = _filelength(_fileno(f));

	// Read the entire file
	std::string script;
	script.resize(len);
	size_t c = fread(&script[0], len, 1, f);
	fclose(f);

	if( c == 0 )
	{
		std::cout << "Failed to load script file." << std::endl;
		return -1;
	}

	// Add the script sections that will be compiled into executable code.
	// If we want to combine more than one file into the same script, then
	// we can call AddScriptSection() several times for the same module and
	// the script engine will treat them all as if they were one. The script
	// section name, will allow us to localize any errors in the script code.
	asIScriptModule *mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
	r = mod->AddScriptSection("script", &script[0], len);
	if( r < 0 )
	{
		std::cout << "AddScriptSection() failed" << std::endl;
		return -1;
	}

	// Compile the script. If there are any compiler messages they will
	// be written to the message stream that we set right after creating the
	// script engine. If there are no errors, and no warnings, nothing will
	// be written to the stream.
	r = mod->Build();
	if( r < 0 )
	{
		std::cout << "Build() failed" << std::endl;
		return -1;
	}

	// The engine doesn't keep a copy of the script sections after Build() has
	// returned. So if the script needs to be recompiled, then all the script
	// sections must be added again.

	// If we want to have several scripts executing at different times but
	// that have no direct relation with each other, then we can compile them
	// into separate script modules. Each module use their own namespace and
	// scope, so function names, and global variables will not conflict with
	// each other.

	return 0;
}

// Function implementation with native calling convention
void PrintString(std::string &str)
{
	std::cout << str;
}

// Function implementation with generic script interface
void PrintString_Generic(asIScriptGeneric *gen)
{
	std::string *str = (std::string*)gen->GetArgAddress(0);
	std::cout << *str;
}

void ConfigureEngine(asIScriptEngine *engine)
{
	int r;

	// Register the script string type
	// Look at the implementation for this function for more information
	// on how to register a custom string type, and other object types.
	RegisterStdString(engine);

	if( !strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY") )
	{
		// Register the functions that the scripts will be allowed to use.
		// Note how the return code is validated with an assert(). This helps
		// us discover where a problem occurs, and doesn't pollute the code
		// with a lot of if's. If an error occurs in release mode it will
		// be caught when a script is being built, so it is not necessary
		// to do the verification here as well.
		r = engine->RegisterGlobalFunction("void Print(string &in)", asFUNCTION(PrintString), asCALL_CDECL); assert( r >= 0 );
	}
	else
	{
		// Notice how the registration is almost identical to the above.
		r = engine->RegisterGlobalFunction("void Print(string &in)", asFUNCTION(PrintString_Generic), asCALL_GENERIC); assert( r >= 0 );
	}


	// It is possible to register the functions, properties, and types in
	// configuration groups as well. When compiling the scripts it then
	// be defined which configuration groups should be available for that
	// script. If necessary a configuration group can also be removed from
	// the engine, so that the engine configuration could be changed
	// without having to recompile all the scripts.
}



int RunApplication()
{
	int r;

	// Create the script engine
	asIScriptEngine *engine = asCreateScriptEngine();
	if( engine == 0 )
	{
		std::cout << "Failed to create script engine." << std::endl;
		return -1;
	}

	// The script compiler will write any compiler messages to the callback.
	engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

	// Configure the script engine with all the functions,
	// and variables that the script should be able to use.
	ConfigureEngine(engine);

	// Compile the script code
	r = CompileScript(engine);
	if( r < 0 )
	{
		engine->Release();
		return -1;
	}

	// Create a context that will execute the script.
	asIScriptContext *ctx = engine->CreateContext();
	if( ctx == 0 )
	{
		std::cout << "Failed to create the context." << std::endl;
		engine->Release();
		return -1;
	}

	// Find the function for the function we want to execute.
	asIScriptFunction *func = engine->GetModule(0)->GetFunctionByDecl("float calc(float, float)");
	if( func == 0 )
	{
		std::cout << "The function 'float calc(float, float)' was not found." << std::endl;
		ctx->Release();
		engine->Release();
		return -1;
	}

	// Prepare the script context with the function we wish to execute. Prepare()
	// must be called on the context before each new script function that will be
	// executed. Note, that if you intend to execute the same function several
	// times, it might be a good idea to store the function returned by
	// GetFunctionByDecl(), so that this relatively slow call can be skipped.
	r = ctx->Prepare(func);
	if( r < 0 )
	{
		std::cout << "Failed to prepare the context." << std::endl;
		ctx->Release();
		engine->Release();
		return -1;
	}

	// Now we need to pass the parameters to the script function.
	ctx->SetArgFloat(0, 3.14159265359f);
	ctx->SetArgFloat(1, 2.71828182846f);

	// Execute the function
	std::cout << "Executing the script." << std::endl;
	std::cout << "---" << std::endl;
	r = ctx->Execute();
	std::cout << "---" << std::endl;
	if( r != asEXECUTION_FINISHED )
	{
		// The execution didn't finish as we had planned. Determine why.
		if( r == asEXECUTION_ABORTED )
			std::cout << "The script was aborted before it could finish. Probably it timed out." << std::endl;
		else if( r == asEXECUTION_EXCEPTION )
		{
			std::cout << "The script ended with an exception." << std::endl;

			// Write some information about the script exception
			asIScriptFunction *func = ctx->GetExceptionFunction();
			std::cout << "func: " << func->GetDeclaration() << std::endl;
			std::cout << "modl: " << func->GetModuleName() << std::endl;
			std::cout << "sect: " << func->GetScriptSectionName() << std::endl;
			std::cout << "line: " << ctx->GetExceptionLineNumber() << std::endl;
			std::cout << "desc: " << ctx->GetExceptionString() << std::endl;
		}
		else
			std::cout << "The script ended for some unforeseen reason (" << r << ")." << std::endl;
	}
	else
	{
		// Retrieve the return value from the context
		float returnValue = ctx->GetReturnFloat();
		std::cout << "The script function returned: " << returnValue << std::endl;
	}

	// We must release the contexts when no longer using them
	ctx->Release();

	// Shut down the engine
	engine->ShutDownAndRelease();

	return 0;
}





// int main(int argc, char* const* argv) {
//     // Processing launch arguments.
//     // For now there is just one - to toggle on debug messages.
//     // if (argc > 1) {
//     //     for (int i = 1; i < argc; i++) {
//     //         if (std::strcmp(argv[i], "--debug") == 0) {
//     //             spdlog::set_level(spdlog::level::debug);
//     //         }
//     //     }
//     // }

//     // App app;
//     // app.run();

//     asIScriptEngine *engine = asCreateScriptEngine();

//     // Set the message callback to receive information on errors in human readable form.
//     int r = engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL); assert( r >= 0 );

//     // AngelScript doesn't have a built-in string type, as there is no definite standard
//     // string type for C++ applications. Every developer is free to register its own string type.
//     // The SDK do however provide a standard add-on for registering a string type, so it's not
//     // necessary to implement the registration yourself if you don't want to.
//     RegisterStdString(engine);

//     // Register the function that we want the scripts to call
//     r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL); assert( r >= 0 );

//     CScriptBuilder builder;
//     int r = builder.StartNewModule(engine, "MyModule");
//     if( r < 0 ) {
//         // If the code fails here it is usually because there
//         // is no more memory to allocate the module
//         printf("Unrecoverable error while starting a new module.\n");
//         return;
//     }
//     r = builder.AddSectionFromFile("test.as");
//     if( r < 0 ) {
//         // The builder wasn't able to load the file. Maybe the file
//         // has been removed, or the wrong name was given, or some
//         // preprocessing commands are incorrectly written.
//         printf("Please correct the errors in the script and try again.\n");
//         return;
//     }
//     r = builder.BuildModule();
//     if( r < 0 ) {
//         // An error occurred. Instruct the script writer to fix the
//         // compilation errors that were listed in the output stream.
//         printf("Please correct the errors in the script and try again.\n");
//         return;
//     }

//     return 0;
// }


int main(int argc, char **argv)
{
	RunApplication();

	return 0;
}

