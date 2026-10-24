
#ifndef command_h
#define command_h

struct SimpleCommand
{
	int _numberOfAvailableArguments;
	int _numberOfArguments;
	char **_arguments;

	SimpleCommand();
	void insertArgument(char *argument);
};

struct Command
{
	int _numberOfAvailableSimpleCommands = 1;
	int _numberOfSimpleCommands =0;
	SimpleCommand **_simpleCommands = (SimpleCommand **)malloc(_numberOfSimpleCommands * sizeof(SimpleCommand *));
	char *_outFile;
	char *_inputFile;
	char *_errFile;
	int  _out_error =0;
	int _background =0;
	int _append =0;

	void prompt();
	void print();
	void execute();
	void clear();

	Command();
	void insertSimpleCommand(SimpleCommand *simpleCommand);

	static Command _currentCommand;
	static SimpleCommand *_currentSimpleCommand;
};

#endif
