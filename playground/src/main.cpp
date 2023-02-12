#include "app.hpp"

#include <spdlog/spdlog.h>
#include <iostream>
#include <string>

#include <dependencies/angelscript/include/angelscript.h>
#include <dependencies/angelscript/add_on/scriptstdstring/scriptstdstring.h>
#include <dependencies/angelscript/add_on/scriptbuilder/scriptbuilder.h>

void message_callback(const asSMessageInfo *msg, void *param) {
	switch (msg->type) {
	case asMSGTYPE_ERROR: {
		spdlog::error("%s at %d, %d: %s", msg->section, msg->row, msg->col, msg->message);
		break;
	}
	case asMSGTYPE_WARNING: {
		spdlog::warn("%s at %d, %d: %s", msg->section, msg->row, msg->col, msg->message);
		break;
	}
	case asMSGTYPE_INFORMATION: {
		spdlog::info(msg->message);
		break;
	}
	}
}

int compile_script(asIScriptEngine *engine, const std::string &filename) {
	FILE *f = fopen(filename.c_str(), "rb");
	if(f == 0) {
		spdlog::error("Failed to open the script file {}", filename);
		return 1;
	}

	fseek(f, 0, SEEK_END);
	int len = ftell(f);
	fseek(f, 0, SEEK_SET);
	std::string script;
	script.resize(len);
	size_t c = fread(&script[0], len, 1, f);
	fclose(f);

	if(c == 0) {
		spdlog::error("Failed to load script file {}", filename);
		return 1;
	}


	asIScriptModule *mod = engine->GetModule(0, asGM_ALWAYS_CREATE);
	int r = mod->AddScriptSection("script", &script[0], len);
	if(r < 0) {
		spdlog::error("AddScriptSection() failed");
		return 1;
	}

	// Compile the script. If script has been changed, it will be needed to
	// recompile to reflect these changes
	r = mod->Build();
	if(r < 0) {
		spdlog::error("Build() failed");
		return 1;
	}

	return 0;
}

// Function implementation with native calling convention
void print_string(std::string &str) {
	printf("%s", str.c_str());
}

// Function implementation with generic script interface
void print_string_generic(asIScriptGeneric *gen) {
	std::string *str = static_cast<std::string*>(gen->GetArgAddress(0));
	printf("%s", str->c_str());
}

void configure_engine(asIScriptEngine *engine)
{
	// Register the script string type
	RegisterStdString(engine);

	int r;
	if(!strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY")) {
		r = engine->RegisterGlobalFunction(
			"void print(string &in)",
			asFUNCTION(print_string),
			asCALL_CDECL
		);

		assert(r >= 0);
	}
	else {
		r = engine->RegisterGlobalFunction(
			"void print(string &in)",
			asFUNCTION(print_string_generic),
			asCALL_GENERIC
		);
		assert(r >= 0);
	}
}


int run_app() {
	// Create the script engine
	asIScriptEngine *engine = asCreateScriptEngine();
	if( engine == 0 )
	{
		spdlog::error("Failed to create script engine.");
		return -1;
	}

	// All compiler messages will be sent there
	engine->SetMessageCallback(
		asFUNCTION(message_callback),
		0,
		asCALL_CDECL
	);

	configure_engine(engine);

	// Compile the script code
	int r = compile_script(engine, "script.as");
	if(r < 0) {
		engine->Release();
		return -1;
	}

	// Create a context that will execute the script
	asIScriptContext *ctx = engine->CreateContext();
	if(ctx == 0) {
		spdlog::error("Failed to create the context.");
		engine->Release();
		return -1;
	}

	// Find the function for the function we want to execute
	asIScriptFunction *func = engine->GetModule(0)->GetFunctionByDecl("void main()");
	if(func == 0) {
		spdlog::error("The function 'void main()' was not found.");
		ctx->Release();
		engine->Release();
		return -1;
	}

	// Prepare the script context with the function we wish to execute. Prepare()
	// must be called on the context before each new script function that will be
	// executed. Note, that if you intend to execute the same function several
	// times, it might be a good idea to store the function returned by
	// GetFunctionByDecl(), so that this relatively slow call can be skipped
	r = ctx->Prepare(func);
	if(r < 0) {
		spdlog::error("Failed to prepare the context.");
		ctx->Release();
		engine->Release();
		return -1;
	}

	// Execute the function
	spdlog::info("Executing the script:\n");
	r = ctx->Execute();
	if(r != asEXECUTION_FINISHED) {
		// something went wrong
		if(r == asEXECUTION_ABORTED) {
			spdlog::error("The script was aborted before it could finish");
		}
		else if(r == asEXECUTION_EXCEPTION) {
			asIScriptFunction *func = ctx->GetExceptionFunction();
			spdlog::error(
				"The script ended with an exception: {} {} {} {} {}",
				func->GetDeclaration(),
				func->GetModuleName(),
				func->GetScriptSectionName(),
				ctx->GetExceptionLineNumber(),
				ctx->GetExceptionString()
			);
		}
		else {
			spdlog::error("The script ended for some unforeseen reason: {}", r);
		}
	}

	// We should manually release unused context
	ctx->Release();

	engine->ShutDownAndRelease();

	return 0;
}

int main(int argc, char **argv)
{
	run_app();

	return 0;
}

