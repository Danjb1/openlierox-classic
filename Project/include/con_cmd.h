/////////////////////////////////////////
//
//   Auxiliary Software class library
//
/////////////////////////////////////////


// Command/variable parsing header
// Created 9/4/02
// Jason Boettcher


#ifndef __CON_COMMAND_H__
#define __CON_COMMAND_H__


#define		MAX_ARGS		32
#define		MAX_ARGLENGTH	128


// Command structure
typedef struct command_s {

	char				*strName;
	void				(*func) ( void );

	struct command_s	*Next;

} command_t;


// Arguements
int		Cmd_GetNumArgs(void);
void	Cmd_AddArg(char *text);
char	*Cmd_GetArg(int a);



// Command routines
command_t	*Cmd_GetCommand(char *strName);
void	Cmd_ParseLine(char *text);
int		Cmd_AutoComplete(char *strVar, int *iLength);
int		Cmd_AddCommand(char *strName, void (*func) ( void ));
void	Cmd_FreeCommands(void);
void	Cmd_Free(void);


// User commands
void    Cmd_Kick(void);


#endif  //  __CON_COMMAND_H__