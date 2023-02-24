#include <launcher.h>
#include <sys/env.h>
#include <sys/dbg.h>
#include <stdio.h>

list<running_program_t*>* running_programs = nullptr;

extern char* root_fs;


void launcher_init() {
	running_programs = new list<running_program_t*>(10);
	debugf("launcher_init()");
}

void launcher_run(char* program) {
	debugf("launcher_run(%s)", program);

	running_program_t* running_program = new running_program_t();
	memset(running_program, 0, sizeof(running_program_t));

	//TODO: allow support for fexec programs
	
	char program_path[512] = { 0 };
	sprintf(program_path, "%s/BIN/%s.elf", root_fs, program);
	debugf("program_path: %s", program_path);

	char** argv = new char*[2];
	argv[0] = (char*) malloc(strlen(program) + 1);
	memset(argv[0], 0, strlen(program) + 1);
	strcpy(argv[0], program);

	argv[1] = nullptr;

	running_program->task = spawn(program_path, (const char**) argv, (const char**) env(ENV_ENVP), ELF_EXECUTABLE, true);
	running_program->on_exit = false;
	running_program->argv = argv;

	running_program->task->on_exit = &running_program->on_exit;

	running_programs->add(running_program);
}

void launcher_tick() {
	running_programs->foreach<void*>([](void* _, list<running_program_t*>::node* n) {
		if (n->data->on_exit) {
			debugf("program %s exited", n->data->argv[0]);
			running_programs->remove(n);
			free(n->data->argv[0]);
			delete n->data->argv;
			delete n->data;
		}
	}, nullptr);
}