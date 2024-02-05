#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>

char * Commands[12] = {"init" , "config" , "add" , "reset" , "commit" , "set" , "replace" , "remove" , "status" , "branch" , "log" , "checkout"};

struct User {
	char name_mode[2];
	char Username[256];
	char email_mode[2];
	char Email[256];
};

struct Commit{
	char Path[256];
	time_t ModifiedTime;	
};

struct Commit_Log{
	char CommitID[20];
	time_t CommitTime;
	char CommitMessage[75];
	char CommitUser[100];
	char CommitBranch[100];
	int CommitFileCount;
};

struct User CurUser;
char cwd[256];
char GitRoot[256];
char CopyArgv[20][100];
char ActiveBranch[100];
char AliasString[1000];
char CheckoutMode[10];

// A function to check if a directory exists
int dir_exists(char * path)
{
	struct stat stats;
    stat(path, &stats);
    if (S_ISDIR(stats.st_mode))
        return 1;
    return 0;
}

// A function to check if a file exists
int file_exists(char * Path)
{
	FILE * fp = fopen(Path, "r");
	if (!fp) {
		return 0;
	}
	fclose(fp);
	return 1;
}

// A function to create a directory
void create_dir(char *path) {
  	if (mkdir(path) != 0) {
    	printf("Failed to create directory");
    	exit(1);
  	}
}

// A function to create a file
void create_file(char *path) {
  	FILE *fp = fopen(path, "w");
  	if (fp == NULL) {
    	printf("Failed to create file");
    	exit(1);
  	}
  	fclose(fp);
}

// A function to set Username & Email
void save_configuration(int Mode)
{
	FILE * fp = fopen(".neogit\\config", "w");
	if (!fp)
	{
		printf("Failed to open config file");
		return;
	}
	fprintf(fp, "%s\n", CurUser.name_mode);
	fprintf(fp, "%s\n", CurUser.Username);
	fprintf(fp, "%s\n", CurUser.email_mode);
	fprintf(fp, "%s\n", CurUser.Email);
	fprintf(fp, "%s\n", ActiveBranch);
	fprintf(fp, "%s\n", CheckoutMode);
	fclose(fp);

	if (Mode)
	{	
		fp = fopen(".neogit\\branches", "w");
		if (!fp)
		{
			printf("Failed to open branches file");
			return;
		}
		fprintf(fp, "%s\n", ActiveBranch);
		fclose(fp);
		
		fp = fopen(".neogit\\last_commit", "w");
		if (!fp)
		{
			printf("Failed to open last commit file");
			return;
		}
		fprintf(fp, "%s\n", "None");
		fclose(fp);		
	}
}

void Extract_CWD()
{
	if (getcwd(cwd, sizeof(cwd)) == NULL) {
    	printf("Failed to get current working directory");
    	return;
  	}
}
void Extract_Root()
{
	char WDirAdd[256];
  	char Temp[256];
   	strcpy(WDirAdd, cwd);
	strcat(WDirAdd, "\\.neogit");
	if (dir_exists(WDirAdd))
	{
		strcpy(GitRoot, cwd);
	   	return;
	}
	
  	strcpy(WDirAdd, cwd);
  	int Pos = strlen(WDirAdd) - 1;
  	while(Pos >= 0)
  	{
  		if (WDirAdd[Pos] != '\\')
		{
			WDirAdd[Pos] = '\0';
			Pos--;
		}
		else
		{
			strcpy(Temp, WDirAdd);
			strcat(Temp, ".neogit");
			if (dir_exists(Temp))
			{
				WDirAdd[Pos] = '\0';
				strcpy(GitRoot, WDirAdd);
		       	return;	
			}
			Pos--;
		}
	}
	strcpy(GitRoot, "None");
	return;
}

// A function to handle "init" command
void init(int argc)
{
	// check parameters
	if(argc > 2)
	{
		printf("Too many parameters...");
		return;
	}
	
  	// check existance of .neogit folder
  	if (strcmp(GitRoot, "None"))
  	{
  		printf("neogit repository already exists in %s\n", GitRoot);
  		return;
	}
	
	// Create the neogit folder and its subfolders and files
  	system("c:\\git\\init.bat");
  	printf("Initialized empty neogit repository in %s\n", cwd);
  	save_configuration(1);
}

// A function to handle "config" command
void config(int argc, char * argv[])
{
	int isglobal = 0;
	char isGlobalS[2];
	strcpy(isGlobalS, "0");
	FILE * fp;
	// check parameters
	if(argc > 5)
	{
		printf("Too many parameters...");
		return;
	}
	// check global switch
	if (!strcmp(argv[2], "-global"))
	{
		isglobal = 1;
		strcpy(isGlobalS, "1");
	}
	// check parameters
	if (strstr(argv[2 + isglobal], "alias."))
	{
		if ((argc + isglobal) < 4 + isglobal)
		{
			printf("Too few Parameters...\n");
			return;
		}
		char aliasStr[100];
		int i, Pos = 0;
		for(i = 6; CopyArgv[2 + isglobal][i]; i++)
		{
			aliasStr[Pos++] = CopyArgv[2 + isglobal][i];
		}
		aliasStr[Pos] = '\0';
		char CommandStr[1000];
		strcpy(CommandStr, "ExtractArgs ");
		strcat(CommandStr, argv[3 + isglobal]);
		int NewArgc = system(CommandStr);
		char NewArgv[NewArgc][100];
		fp = fopen("c:\\git\\Tempargs", "r");
		if (!fp)
		{
			printf("Temporary args file cannot open...\n");
			return;
		}
		Pos = 0;
		fscanf(fp, "%[^\n]s", NewArgv[Pos++]);
		while(!feof(fp))
		{
			fscanf(fp, "\n%[^\n]s", NewArgv[Pos++]);
		}
		fclose(fp);
		remove("c:\\git\\Tempargs");

		if (strcmp(NewArgv[1], "neogit"))
		{
			printf("Wrong command, command should start with neogit...\n");
			return;
		}
		int Sw = 0;
		for(i = 0; i < 12; i++)
		{
			if (!strcmp(NewArgv[2], Commands[i]))
			{
				Sw = 1;
			}
		}
		if (!Sw)
		{
			printf("Wrong command, command is not in legal commands (such as init, config, add ...)\n");
			return;
		}
		if (isglobal == 1)
		{
			fp = fopen("c:\\git\\neogit_alias", "a");
			if (!fp)
			{
				printf("Global alias file connot open...\n");
				return;
			}
			fprintf(fp, "%s\n", aliasStr);
			fprintf(fp, "%s\n", CopyArgv[4]);
			fclose(fp);
		}
		else
		{
			char WFileAdd[1000];
			strcpy(WFileAdd, GitRoot);
			strcat(WFileAdd, "\\.neogit\\alias");
			fp = fopen(WFileAdd, "a");
			if (!fp)
			{
				printf("Local alias file cannot open...\n");
				return;
			}
			fprintf(fp, "%s\n", aliasStr);
			fprintf(fp, "%s\n", CopyArgv[3]);
			fclose(fp);
		}		
		return;
	}
	
	if (!strcmp(argv[2+isglobal], "user.name"))
	{
		strcpy(CurUser.Username, CopyArgv[3+isglobal]);
		strcpy(CurUser.name_mode, isGlobalS);
	}
	if (!strcmp(argv[2+isglobal], "user.email"))
	{
		strcpy(CurUser.Email, CopyArgv[3+isglobal]);
		strcpy(CurUser.email_mode, isGlobalS);
	}
	
	if (strcmp(argv[2+isglobal], "user.name") && strcmp(argv[2+isglobal], "user.email") && !strstr(argv[2 + isglobal], "alias."))
	{
	   	printf("Wrong parameters\n");
		return;
	}
	// set new configuration for all projects
	if (isglobal)
	{
		fp = fopen("C:\\git\\neogit_config", "w");
		if (!fp)
		{
			printf("Global config file not found...\n");
			return;
		}	
		fprintf(fp, "%s\n", CurUser.Username);
		fprintf(fp, "%s\n", CurUser.Email);
		printf("Global configuration changed...\n");
		fclose(fp);
	}
	// set new configuration for local project
	char TempDir[256];
	strcpy(TempDir, GitRoot);
	strcat(TempDir, "\\.neogit");
	if (dir_exists(TempDir))
	{
		strcat(TempDir, "\\config");
		fp = fopen(TempDir, "w");
		if (!fp)
		{
			printf("Local config file not found...\n");
			return;
		}	
		fprintf(fp, "%s\n", CurUser.name_mode);
		fprintf(fp, "%s\n", CurUser.Username);
		fprintf(fp, "%s\n", CurUser.email_mode);
		fprintf(fp, "%s\n", CurUser.Email);
		fprintf(fp, "%s\n", ActiveBranch);
		fprintf(fp, "%s\n", CheckoutMode);
		printf("Local configuration changed...\n");
		fclose(fp);
	}
}

void extract_path(char * Path, char * DiffPath, char * FileName)
{
	int i;
	int Pos = 0;
	for(i = strlen(GitRoot) + 1; Path[i]; i++)
	{
		DiffPath[Pos++] = Path[i];
	}
	DiffPath[Pos] = '\0';
	char TempName[100];
	Pos = 0;
	for(i = strlen(DiffPath) - 1; i >= 0; i--)
	{
		if (DiffPath[i] != '\\')
		{
			TempName[Pos++] = DiffPath[i];
			DiffPath[i] = '\0';
		}
		else 
		{
			TempName[Pos] = '\0';
			DiffPath[i] = '\0';
			break;	
		}
	}
	if (i == -1) 
	{
		TempName[Pos] = '\0';
	}
	Pos = 0;
	for(i = strlen(TempName) - 1; i >= 0; i--)
	{
		FileName[Pos++] = TempName[i];
	}
	FileName[Pos] = '\0';
}

int isStaged(char * Path, char Staged[][256], int StagedCounter, int * Pos)
{
	int i;
	if (StagedCounter == 0) return 0;
	for (i = 0; i < StagedCounter; i++)
		if (!strcmp(Path, Staged[i]))
		{
			*Pos = i;
			return 1;
		}
	return 0;
}

int isModified(char * Path, struct Commit Commited[], int CommitedCounter)
{
	struct stat St;
	stat(Path, &St);
	int i;
	if (CommitedCounter == 0) return 2;
	for(i = 0; i < CommitedCounter; i++)
		if (!strcmp(Path, Commited[i].Path))
		{
			if (St.st_mtime == Commited[i].ModifiedTime)
				return 0;
			else 
				return 1;
		}
	return 2;
}

void add_file(char * Path, char Staged[][256], int StagedCounter)
{
	char WFileAdd[256];
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\staged");
	FILE * fp = fopen(WFileAdd, "a");
	if (!fp) {
		printf("Stage file doesn't exist...\n");
		return;
	}
	int Pos;
	if (!isStaged(Path, Staged, StagedCounter, &Pos))
	{
		fprintf(fp, "%s\n", Path);
	}
	fclose(fp);
}

void add_dir_file(char * Path, char Staged[][256], int StagedCounter, struct Commit Commited[], int CommitedCounter)
{
	char WFileAdd[256];
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\staged");
	FILE * fp = fopen(WFileAdd, "a");
	if (!fp) {
		printf("stage file doesn't exist...\n");
		return;
	}
	int Pos;
	if (!isStaged(Path, Staged, StagedCounter, &Pos) && isModified(Path, Commited, CommitedCounter))
	{
		fprintf(fp, "%s\n", Path);
	}
	fclose(fp);
}

void add_directory(char * Path, char Staged[][256], int StagedCounter, struct Commit Commited[], int CommitedCounter)
{
	DIR *dir = opendir(Path);
	if (dir == NULL) {
    	printf("Cannot open directory %s\n", Path);
    	return;
	}
	struct dirent *entry;
	char Temp[256];
	while ((entry = readdir(dir)) != NULL)
	{
    	if (!strcmp((*entry).d_name, ".") || !strcmp((*entry).d_name, "..")) 
		{
      		continue;
    	}
    	strcpy(Temp, Path);
    	strcat(Temp, "\\");
    	strcat(Temp, (*entry).d_name);
    	if (file_exists(Temp))
		{
			add_dir_file(Temp, Staged, StagedCounter, Commited, CommitedCounter);
		}
		if (dir_exists(Temp))
		{
			add_directory(Temp, Staged, StagedCounter, Commited, CommitedCounter);
		}
	}
	closedir(dir);
}

void add_wildcard(char* Path, char Staged[][256], int StagedCounter, struct Commit Commited[], int CommitedCounter)
{
	char Command[1000];
	strcpy(Command, "dir ");
	strcat(Command, Path);
	strcat(Command, " /b > c:\\git\\temp");
	system(Command);
	
	FILE * fp = fopen("c:\\git\\temp", "r");
	if (!fp)
	{
		printf("Temporary dir file not found...\n");
		return;
	}
	
	char WFileAdd[1000];
	char Name[100];
	fscanf(fp, "%[^\n]s", Name);
	while(!feof(fp))
	{
		strcpy(WFileAdd, cwd);
		strcat(WFileAdd, "\\");
		strcat(WFileAdd, Name);	
		if (file_exists(WFileAdd))
		{
			add_file(WFileAdd, Staged, StagedCounter);
		}
		if (dir_exists(WFileAdd))
		{
			add_directory(WFileAdd, Staged, StagedCounter, Commited, CommitedCounter);
		}	
		fscanf(fp, "\n%[^\n]s", Name);	
	}
	fclose(fp);
	remove("c:\\git\\temp");
	return;
}

void add_multiple_filedir(int argc, char *argv[], char Staged[][256], int StagedCounter, struct Commit Commited[], int CommitedCounter)
{
	int i;
	char WFileAdd[256];
	for (i = 3; i < argc; i++)
	{
		strcpy(WFileAdd, cwd);
		strcat(WFileAdd, "\\");
		strcat(WFileAdd, argv[i]);
		if (!file_exists(WFileAdd) && !dir_exists(WFileAdd))
			printf("%s file or folder not found...\n", WFileAdd);
	
		if (file_exists(WFileAdd))
			add_file(WFileAdd, Staged, StagedCounter);
		
		if (dir_exists(WFileAdd))
			add_directory(WFileAdd, Staged, StagedCounter, Commited, CommitedCounter);
	}
}

int load_staged(char Staged[][256], int * StagedCounter)
{
	char WFileAdd[256];
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\staged");
	FILE * fp = fopen(WFileAdd, "r");
	if (!fp) {
		printf("Stage file doesn't exist...\n");
		return 0;
	}
	
	fscanf(fp, "%s", Staged[*StagedCounter]);
	while(!feof(fp))
	{
		(*StagedCounter)++;
		fscanf(fp, "%s", Staged[*StagedCounter]);
	}
	fclose(fp);
	return 1;
}

int load_last_commited(struct Commit Commited[], int * CommitedCounter)
{
	char WFileAdd[256];
	char LastCommit[100];
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\last_commit");
	FILE * fp = fopen(WFileAdd, "r");
	if (!fp) {
		printf("Commited file doesn't exist...\n");
		return 0;
	}
	fscanf(fp, "%s", LastCommit);
	fclose(fp);

	if (!strcmp(LastCommit, "None"))
	{
		return 0;
	}
	
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\repository\\");
	strcat(WFileAdd, LastCommit);
	strcat(WFileAdd, "_files");
	fp = fopen(WFileAdd, "r");
	if (!fp) {
		printf("Last commited file doesn't exist...\n");
		return;
	}
	fscanf(fp, "%s", Commited[*CommitedCounter].Path);
	while(!feof(fp))
	{
		(*CommitedCounter)++;
		fscanf(fp, "%s", Commited[*CommitedCounter].Path);
	}
	fclose(fp);
	return 1;
}

void extract_commited_info(struct Commit Commited[], int CommitedCounter)
{
	int i;
	char DiffPath[100];
	char FileName[100];
	char TempPath[100];
	char CommitPath[1000];
	char WFileAdd[256];
	char LastCommit[20];
	char LastCommitBranch[100];
	
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\last_commit");
	FILE * fp = fopen(WFileAdd, "r");
	if (!fp) {
		printf("Commited file doesn't exist...\n");
		return;
	}
	fscanf(fp, "%s", LastCommit);
	fscanf(fp, "%s", LastCommitBranch);
	fclose(fp);
	
	strcpy(CommitPath, GitRoot);
	strcat(CommitPath, "\\.neogit\\repository\\");
	strcat(CommitPath, LastCommitBranch);
	strcat(CommitPath, "\\");
	strcat(CommitPath, LastCommit);
	
	struct stat St;
	for(i = 0; i < CommitedCounter; i++)
	{
		extract_path(Commited[i].Path, DiffPath, FileName);
		
		strcpy(TempPath, CommitPath);
		strcat(TempPath, "\\");
		if (strlen(DiffPath) > 0)
		{
			strcat(TempPath, DiffPath);
			strcat(TempPath, "\\");			
		}
		strcat(TempPath, FileName);
		stat(TempPath, &St);
		Commited[i].ModifiedTime = St.st_mtime;
	}
}

void add(int argc, char * argv[])
{
	char Staged[100][256];
	struct Commit Commited[100];
	char WFileAdd[256];
	int StagedCounter = 0;
	int CommitedCounter = 0;

	load_staged(Staged, &StagedCounter);
	int res = load_last_commited(Commited, &CommitedCounter);
	if (res) extract_commited_info(Commited, CommitedCounter);
	
	if (!strcmp(argv[2], "-f"))
	{
		add_multiple_filedir(argc, argv, Staged, StagedCounter, Commited, CommitedCounter);
		return;
	}
	
	strcpy(WFileAdd, cwd);
	strcat(WFileAdd, "\\");
	strcat(WFileAdd, CopyArgv[2]);
	
	if (strstr(CopyArgv[2], "*") || strstr(CopyArgv[2], "?"))
	{
		add_wildcard(WFileAdd, Staged, StagedCounter, Commited, CommitedCounter);
		return;
	}
		
	if (!file_exists(WFileAdd) && !dir_exists(WFileAdd))
	{
		printf("%s file or folder not found...\n", WFileAdd);
		return;
	}
	if (file_exists(WFileAdd))
	{
		add_file(WFileAdd, Staged, StagedCounter);
	}
	if (dir_exists(WFileAdd))
	{
		add_directory(WFileAdd, Staged, StagedCounter, Commited, CommitedCounter);
	}
}

void reset_file(char * Path, char Staged[][256], int StagedCounter, char Reseted[])
{
	int Pos;
	if (isStaged(Path, Staged, StagedCounter, &Pos))
	{
		Reseted[Pos] = '1';
	}
}

void reset_directory(char * Path, char Staged[][256], int StagedCounter, char Reseted[])
{
	DIR *dir = opendir(Path);
	if (dir == NULL) {
    	printf("Cannot open directory %s\n", Path);
    	return;
	}
	struct dirent *entry;
	char Temp[256];
	while ((entry = readdir(dir)) != NULL)
	{
    	if (!strcmp((*entry).d_name, ".") || !strcmp((*entry).d_name, "..")) 
		{
      		continue;
    	}
    	strcpy(Temp, Path);
    	strcat(Temp, "\\");
    	strcat(Temp, (*entry).d_name);
    	if (file_exists(Temp))
		{
			reset_file(Temp, Staged, StagedCounter, Reseted);
		}
		if (dir_exists(Temp))
		{
			reset_directory(Temp, Staged, StagedCounter, Reseted);
		}
	}
}

void reset_multiple_filedir(int argc, char *argv[], char Staged[][256], int StagedCounter, char Reseted[])
{
	int i;
	char WFileAdd[256];
	for (i = 3; i < argc; i++)
	{
		strcpy(WFileAdd, cwd);
		strcat(WFileAdd, "\\");
		strcat(WFileAdd, argv[i]);
		if (!file_exists(WFileAdd) && !dir_exists(WFileAdd))
			printf("%s file or folder not found...\n", WFileAdd);
	
		if (file_exists(WFileAdd))
			reset_file(WFileAdd, Staged, StagedCounter, Reseted);
		
		if (dir_exists(WFileAdd))
			reset_directory(WFileAdd, Staged, StagedCounter, Reseted);
	}
}

void reset(int argc, char * argv[])
{
	char Staged[100][256];
	char Reseted[100];
	char WFileAdd[256];
	int StagedCounter = 0;
	FILE * fp;
	
	load_staged(Staged, &StagedCounter);
	
	int i;
	for(i = 0; i < StagedCounter; i++)
		Reseted[i] = '0';
		
	if (!strcmp(argv[2], "-f"))
	{
		reset_multiple_filedir(argc, argv, Staged, StagedCounter, Reseted);
		strcpy(WFileAdd, GitRoot);
		strcat(WFileAdd, "\\.neogit\\staged");
		fp = fopen(WFileAdd, "w");
		for(i = 0; i < StagedCounter; i++)
		{
			if (Reseted[i] == '0')
				fprintf(fp, "%s\n", Staged[i]);
		}
		fclose(fp);
		return;
	}
	
	strcpy(WFileAdd, cwd);
	strcat(WFileAdd, "\\");
	strcat(WFileAdd, argv[2]);
	if (!file_exists(WFileAdd) && !dir_exists(WFileAdd))
	{
		printf("%s file or folder not found...\n", WFileAdd);
		return;
	}
	
	if (file_exists(WFileAdd))
	{
		reset_file(WFileAdd, Staged, StagedCounter, Reseted);
	}
		
	if (dir_exists(WFileAdd))
	{
		reset_directory(WFileAdd, Staged, StagedCounter, Reseted);
	}
	
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\staged");
	fp = fopen(WFileAdd, "w");
	for(i = 0; i < StagedCounter; i++)
	{
		if (Reseted[i] == '0')
			fprintf(fp, "%s\n", Staged[i]);
	}
	fclose(fp);
}

void status()
{
	char Staged[100][256];
	struct Commit Commited[100];
	char WFileAdd[256];
	char WDirAdd[256];
	char Temp[256];
	int Pos;
	int StagedCounter = 0;
	int CommitedCounter = 0;

	load_staged(Staged, &StagedCounter);
	int res = load_last_commited(Commited, &CommitedCounter);
	if (res) extract_commited_info(Commited, CommitedCounter);
		
	DIR *dir = opendir(cwd);
	if (dir == NULL) {
    	printf("Cannot open directory %s\n", cwd);
    	return;
	}
	struct dirent *entry;
	char StagedSign;
	char ModifiedSign;
	int ModifiedMode;
	int Sw = 0;
	while ((entry = readdir(dir)) != NULL)
	{
    	if (!strcmp((*entry).d_name, ".") || !strcmp((*entry).d_name, "..")) 
		{
      		continue;
    	}
    	strcpy(Temp, cwd);
    	strcat(Temp, "\\");
    	strcat(Temp, (*entry).d_name);
    	StagedSign = '-';
    	ModifiedSign = ' ';
    	if (file_exists(Temp))
    	{
			if (isStaged(Temp, Staged, StagedCounter, &Pos))
    			StagedSign = '+';
    		ModifiedMode = isModified(Temp, Commited, CommitedCounter);
    		if (ModifiedMode == 1)
    			ModifiedSign = 'M';
    		if (ModifiedMode == 2)
    			ModifiedSign = 'A';
    		if ((StagedSign == '+') || (ModifiedSign == 'M') || (ModifiedSign == 'A'))
    		{
    			printf("%c %c %s\n", StagedSign, ModifiedSign, Temp);
				Sw = 1;
			}
		}
	}	
	if (!Sw)
	{
		printf("No Change To Show...\n");
		return;
	}
}

void Load_SCMess(char SCName[100][75], char SCMess[100][75], int * SCMessCounter)
{
	char WFileAdd[256];
	
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\SCMess");
	FILE * fp = fopen(WFileAdd, "r");
	if (!fp) {
		printf("Shortcut messages file doesn't exist...\n");
		return;
	}
	
	*SCMessCounter = 0;
	fscanf(fp, "%[^\n]s", SCName[*SCMessCounter]);
	while(!feof(fp))
	{
		fscanf(fp, "\n%[^\n]s", SCMess[*SCMessCounter]);
		(*SCMessCounter)++;
		fscanf(fp, "\n%[^\n]s", SCName[*SCMessCounter]);
	}
	fclose(fp);
}

void Save_SCMess(char SCName[100][75], char SCMess[100][75], int SCMessCounter)
{
	char WFileAdd[256];
	
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\SCMess");
	FILE * fp = fopen(WFileAdd, "w");
	if (!fp) {
		printf("Shortcut messages file doesn't exist...\n");
		return;
	}
	int i;
	for(i = 0; i < SCMessCounter; i++)
	{
		fprintf(fp, "%s\n", SCName[i]);
		fprintf(fp, "%s\n", SCMess[i]);
	}
	fclose(fp);
}

void setSM(int argc, char * argv[])
{
	if (strcmp(argv[2], "-m") || strcmp(argv[4], "-s"))
	{
		printf("Wrong parameters...\n");
		return;
	}
	if (strlen(argv[3]) > 72)
	{
		printf("Message should be less than 73 char...\n");
		return;
	}
	int SCMessCounter = 0;
	char SCName[100][75];
	char SCMess[100][75];
	Load_SCMess(SCName, SCMess, &SCMessCounter);
	int i;
	for(i = 0; i < SCMessCounter; i++)
	{
		if (!strcmp(CopyArgv[5], SCName[i]))
		{
			printf("Message shortcut already exists...\n");
			return;
		}
	}
	
	char WFileAdd[256];
	
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\SCMess");
	FILE * fp = fopen(WFileAdd, "a");
	if (!fp) {
		printf("Shortcut messages file doesn't exist...\n");
		return;
	}
	fprintf(fp, "%s\n", CopyArgv[5]);
	fprintf(fp, "%s\n", CopyArgv[3]);
	fclose(fp);
}

void replaceSM(int argc, char * argv[])
{
	if (strcmp(argv[2], "-m") || strcmp(argv[4], "-s"))
	{
		printf("Wrong parameters...\n");
		return;
	}
	if (strlen(argv[3]) > 72)
	{
		printf("Message should be less than 73 chars...\n");
		return;
	}
	int SCMessCounter = 0;
	char SCName[100][75];
	char SCMess[100][75];
	Load_SCMess(SCName, SCMess, &SCMessCounter);
	int i, Sw = 0;
	for(i = 0; i < SCMessCounter; i++)
	{
		if (!strcmp(CopyArgv[5], SCName[i]))
		{
			strcpy(SCMess[i], CopyArgv[3]);
			Sw = 1;
		}
	}
	if (!Sw)
	{
		printf("Message shortcut not found...\n");
		return;
	}
	Save_SCMess(SCName, SCMess, SCMessCounter);
}

void removeSM(int argc, char * argv[])
{
	if (strcmp(argv[2], "-s"))
	{
		printf("Wrong parameters...\n");
		return;
	}
	
	int SCMessCounter = 0;
	char SCName[100][75];
	char SCMess[100][75];
	Load_SCMess(SCName, SCMess, &SCMessCounter);
	int i, Sw = 0;
	for(i = 0; i < SCMessCounter; i++)
	{
		if (!strcmp(CopyArgv[3], SCName[i]))
		{
			strcpy(SCName[i], SCName[SCMessCounter - 1]);
			strcpy(SCMess[i], SCMess[SCMessCounter - 1]);
			Sw = 1;
			break;
		}
	}
	if (!Sw)
	{
		printf("Message shortcut not found...\n");
		return;
	}
	SCMessCounter--;
	Save_SCMess(SCName, SCMess, SCMessCounter);
}

void commit(int argc, char * argv[])
{
	if (!strcmp(CheckoutMode, "OLD"))
	{
		printf("You are in an old commit checkout, commit command is impossible. Checkout HEAD first");
		return;
	}
	if (argc != 4)
	{
		printf("Few parameters...\n");
		return;
	}
	if (strcmp(argv[2], "-m") && strcmp(argv[2], "-s"))
	{
		printf("Wrong parameters...\n");
		return;
	}
	if (strlen(argv[3]) > 72)
	{
		printf("Message should be less than 73 chars...\n");
		return;
	}
	
	char Message[75];
	strcpy(Message, CopyArgv[3]);
	if (strcmp(argv[2], "-s") == 0)
	{
		int SCMessCounter = 0;
		char SCName[100][75];
		char SCMess[100][75];
		Load_SCMess(SCName, SCMess, &SCMessCounter);
		int i, sw = 0;
		for(i = 0; i < SCMessCounter; i++)
		{
			if (!strcmp(CopyArgv[3], SCName[i]))
			{
				strcpy(Message, SCMess[i]);
				sw = 1;
				break;
			}
		}
		if (!sw)
		{
			printf("Shortcut message not found...\n");
			return;
		}
	}
	
	char Staged[100][256];
	int StagedCounter;
	
	load_staged(Staged, &StagedCounter);
	if (StagedCounter == 0)
	{
		printf("No file in staged mode...\n");
		return;
	}
	
	time_t CommitTime;
	CommitTime = time(NULL);

	char CommitTimeS[20];
	sprintf(CommitTimeS, "%ld", CommitTime);
	char CommitPath[1000];
	strcpy(CommitPath, GitRoot);
	strcat(CommitPath, "\\.neogit\\repository\\");
	strcat(CommitPath, ActiveBranch);
	strcat(CommitPath, "\\");
	strcat(CommitPath, CommitTimeS);
	create_dir(CommitPath);
	
	char CommitInfoPath[1000];
	strcpy(CommitInfoPath, GitRoot);
	strcat(CommitInfoPath, "\\.neogit\\repository\\");
	strcat(CommitInfoPath, CommitTimeS);
	strcat(CommitInfoPath, "_info");
	FILE * CommitInfo = fopen(CommitInfoPath, "w");
	fprintf(CommitInfo, "%s\n%s\n%s\n%d", Message, CurUser.Username, ActiveBranch, StagedCounter);
	fclose(CommitInfo);
	
	char CommitFilesPath[1000];
	strcpy(CommitFilesPath, GitRoot);
	strcat(CommitFilesPath, "\\.neogit\\repository\\");
	strcat(CommitFilesPath, CommitTimeS);
	strcat(CommitFilesPath, "_files");
	FILE * CommitFiles = fopen(CommitFilesPath, "w");
	int i;
	for(i = 0; i < StagedCounter; i++)
	{
		fprintf(CommitFiles, "%s\n", Staged[i]);
	}
	fclose(CommitFiles);

	char TempBatPath[1000];
	strcpy(TempBatPath, cwd);
	strcat(TempBatPath, "\\TempBat.bat");
	FILE * TempBat = fopen(TempBatPath, "w");

	char DiffPath[100];
	char FileName[100];
	char TempPath[100];
	
	fprintf(TempBat, "echo OFF\n");
	for(i = 0; i < StagedCounter; i++)
	{
		extract_path(Staged[i], DiffPath, FileName);
		strcpy(TempPath, CommitPath);
		strcat(TempPath, "\\");
		strcat(TempPath, DiffPath);
		if (!dir_exists(TempPath))
			create_dir(TempPath);
		strcat(TempPath, "\\");
		strcat(TempPath, FileName);
		fprintf(TempBat, "copy %s %s\n", Staged[i], TempPath);
	}
	fprintf(TempBat, "echo ON\n");
	
	fclose(TempBat);
	system("TempBat.bat");
	remove("TempBat.bat");
	printf("%d Files commited.\n", StagedCounter);
	printf("Commit ID : %s\n", CommitTimeS);
	printf("Commit Message : %s\n", Message);
	printf("Commit Time : %s", ctime(&CommitTime));
	
	char CommitsFilePath[1000];
	strcpy(CommitsFilePath, GitRoot);
	strcat(CommitsFilePath, "\\.neogit\\commits");
	FILE * fp = fopen(CommitsFilePath, "a");
	fprintf(fp, "%s\n", CommitTimeS);
	fclose(fp);
	
	char LastCommitFilePath[1000];
	strcpy(LastCommitFilePath, GitRoot);
	strcat(LastCommitFilePath, "\\.neogit\\last_commit");
	fp = fopen(LastCommitFilePath, "w");
	fprintf(fp, "%s\n", CommitTimeS);
	fprintf(fp, "%s\n", ActiveBranch);
	fclose(fp);
	
	char StagedFilePath[1000];
	strcpy(StagedFilePath, GitRoot);
	strcat(StagedFilePath, "\\.neogit\\staged");
	fp = fopen(StagedFilePath, "w");
	fclose(fp);
}

void Load_Commits_Info(struct Commit_Log Commit_Logs[], int * CommitCounter)
{
	char Path[1000];
	char Temp[1000];
	char FileName[30];
	char CommitID[20];
	FILE * fp;
	int i;
	
	strcpy(Path, GitRoot);
	strcat(Path, "\\.neogit\\repository\\");
	DIR *dir = opendir(Path);
	if (dir == NULL) {
    	printf("Cannot open repository directory\n");
    	return;
	}
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
    	if (!strcmp((*entry).d_name, ".") || !strcmp((*entry).d_name, "..")) 
		{
      		continue;
    	}
    	if (strstr((*entry).d_name, "_info"))
    	{
			strcpy(FileName, (*entry).d_name);
			for(i = 0; i < strlen(FileName) - 5; i++)
			{
				CommitID[i] = FileName[i];
			}
			CommitID[i] = '\0';
			strcpy(Commit_Logs[*CommitCounter].CommitID, CommitID);
			Commit_Logs[*CommitCounter].CommitTime = strtol(CommitID, NULL, 10);
			strcpy(Temp, Path);
			strcat(Temp, "\\");
			strcat(Temp, FileName);
			fp = fopen(Temp, "r");
			if (!fp)
			{
				printf("Cannot open commit info file for %s\n", CommitID);
				return;
			}
			fscanf(fp, "%[^\n]s", &Commit_Logs[*CommitCounter].CommitMessage);
			fscanf(fp, "\n%[^\n]s", &Commit_Logs[*CommitCounter].CommitUser);
			fscanf(fp, "\n%s", &Commit_Logs[*CommitCounter].CommitBranch);
			fscanf(fp, "%d", &Commit_Logs[*CommitCounter].CommitFileCount);
			
			(*CommitCounter)++;
		}
	}
	closedir(dir);
}

void log_git(int argc, char * argv[])
{
	if (argc < 2 || argc > 4)
	{
		printf("Too many parameters...\n");
		return;
	}
	int i, n, Sw = 0;
	struct Commit_Log Commit_Logs[1000];
	int CommitCounter = 0;
	Load_Commits_Info(Commit_Logs, &CommitCounter);

	// log
	if (argc == 2)
	{
		for(i = CommitCounter - 1; i >= 0; i--)
		{
			printf("Commit ID : %s\n", Commit_Logs[i].CommitID);
			printf("Commit Time : %s", ctime(&Commit_Logs[i].CommitTime));
			printf("Commit Message : %s\n", Commit_Logs[i].CommitMessage);
			printf("Commit User : %s\n", Commit_Logs[i].CommitUser);
			printf("Commit Branch : %s\n", Commit_Logs[i].CommitBranch);
			printf("Commit File Count : %d\n", Commit_Logs[i].CommitFileCount);
			printf("==============================\n");
		}
		return;
	}

	// log -n
	n = CommitCounter;
	if (!strcmp(argv[2], "-n"))
	{
		if (atoi(argv[3]) < n)
		{
			n = atoi(argv[3]);
		}
		for(i = CommitCounter - 1; i >= CommitCounter - n; i--)
		{
			printf("Commit ID : %s\n", Commit_Logs[i].CommitID);
			printf("Commit Time : %s", ctime(&Commit_Logs[i].CommitTime));
			printf("Commit Message : %s\n", Commit_Logs[i].CommitMessage);
			printf("Commit User : %s\n", Commit_Logs[i].CommitUser);
			printf("Commit Branch : %s\n", Commit_Logs[i].CommitBranch);
			printf("Commit File Count : %d\n", Commit_Logs[i].CommitFileCount);
			printf("==============================\n");
		}
		return;
	}
		
	// log -branch
	if (!strcmp(argv[2], "-branch"))
	{
		for(i = CommitCounter - 1; i >= 0; i--)
		{
			if (!strcmp(CopyArgv[3], Commit_Logs[i].CommitBranch))
			{
				printf("Commit ID : %s\n", Commit_Logs[i].CommitID);
				printf("Commit Time : %s", ctime(&Commit_Logs[i].CommitTime));
				printf("Commit Message : %s\n", Commit_Logs[i].CommitMessage);
				printf("Commit User : %s\n", Commit_Logs[i].CommitUser);
				printf("Commit Branch : %s\n", Commit_Logs[i].CommitBranch);
				printf("Commit File Count : %d\n", Commit_Logs[i].CommitFileCount);
				printf("==============================\n");
				Sw = 1;
			}
		}
		if (!Sw)
		{
			printf("This branch is not valid...\n");
		}
		return;
	}
	
	// log -author
	if (!strcmp(argv[2], "-author"))
	{
		for(i = CommitCounter - 1; i >= 0; i--)
		{
			if (!strcmp(CopyArgv[3], Commit_Logs[i].CommitUser))
			{
				printf("Commit ID : %s\n", Commit_Logs[i].CommitID);
				printf("Commit Time : %s", ctime(&Commit_Logs[i].CommitTime));
				printf("Commit Message : %s\n", Commit_Logs[i].CommitMessage);
				printf("Commit User : %s\n", Commit_Logs[i].CommitUser);
				printf("Commit Branch : %s\n", Commit_Logs[i].CommitBranch);
				printf("Commit File Count : %d\n", Commit_Logs[i].CommitFileCount);
				printf("==============================\n");
				Sw = 1;
			}
		}
		if (!Sw)
		{
			printf("Commits by this author not found...\n");
		}
		return;		
	}
	
	// log -search
	if (!strcmp(argv[2], "-search"))
	{
		for(i = CommitCounter - 1; i >= 0; i--)
		{
			if (strstr(Commit_Logs[i].CommitMessage, CopyArgv[3]))
			{
				printf("Commit ID : %s\n", Commit_Logs[i].CommitID);
				printf("Commit Time : %s", ctime(&Commit_Logs[i].CommitTime));
				printf("Commit Message : %s\n", Commit_Logs[i].CommitMessage);
				printf("Commit User : %s\n", Commit_Logs[i].CommitUser);
				printf("Commit Branch : %s\n", Commit_Logs[i].CommitBranch);
				printf("Commit File Count : %d\n", Commit_Logs[i].CommitFileCount);
				printf("==============================\n");
				Sw = 1;
			}
		}
		if (!Sw)
		{
			printf("Commits containing this message not found...\n");
		}		
		return;		
	}
	
	time_t TempTime;
	int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0 ;  
	if (sscanf(argv[3], "%4d.%2d.%2d %2d:%2d:%2d", &year, &month, &day, &hour, &min, &sec) == 6) {
		struct tm breakdown = {0};
       	breakdown.tm_year = year - 1900; /* years since 1900 */
       	breakdown.tm_mon = month - 1;
       	breakdown.tm_mday = day;
       	breakdown.tm_hour = hour;
       	breakdown.tm_min = min;
       	breakdown.tm_sec = sec;
    	if ((TempTime = mktime(&breakdown)) == (time_t)-1) {
        	printf("Could not convert time input to time_t...\n");
        	return;
       	}
   	}
   	else 
	{
    	printf("The input was not a valid time format...\n");
      	return;
   	}
   	
	// log -Since
	if (!strcmp(argv[2], "-since"))
	{
		for(i = CommitCounter - 1; i >= 0; i--)
		{
			if (TempTime < Commit_Logs[i].CommitTime)
			{
				printf("Commit ID : %s\n", Commit_Logs[i].CommitID);
				printf("Commit Time : %s", ctime(&Commit_Logs[i].CommitTime));
				printf("Commit Message : %s\n", Commit_Logs[i].CommitMessage);
				printf("Commit User : %s\n", Commit_Logs[i].CommitUser);
				printf("Commit Branch : %s\n", Commit_Logs[i].CommitBranch);
				printf("Commit File Count : %d\n", Commit_Logs[i].CommitFileCount);
				printf("==============================\n");
				Sw = 1;
			}
		}
		if (!Sw)
		{
			printf("Commits after this time not found...\n");
		}		
		return;		
	}
	
	// log -Before
	if (!strcmp(argv[2], "-before"))
	{
		for(i = CommitCounter - 1; i >= 0; i--)
		{
			if (TempTime > Commit_Logs[i].CommitTime)
			{
				printf("Commit ID : %s\n", Commit_Logs[i].CommitID);
				printf("Commit Time : %s", ctime(&Commit_Logs[i].CommitTime));
				printf("Commit Message : %s\n", Commit_Logs[i].CommitMessage);
				printf("Commit User : %s\n", Commit_Logs[i].CommitUser);
				printf("Commit Branch : %s\n", Commit_Logs[i].CommitBranch);
				printf("Commit File Count : %d\n", Commit_Logs[i].CommitFileCount);
				printf("==============================\n");
				Sw = 1;
			}
		}
		if (!Sw)
		{
			printf("Commits before this time not found...\n");
		}		
		return;				
	}
	printf("Wrong Parameters...\n");
}

void load_branches(char Branches[][100], int * BranchCounter)
{
	char WFileAdd[1000];
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\branches");
	FILE * fp = fopen(WFileAdd, "r");
	if (!fp) {
		printf("Branch file doesn't exist...\n");
		return;
	}
	fscanf(fp, "%s", Branches[*BranchCounter]);
	while(!feof(fp))
	{
		(*BranchCounter)++;
		fscanf(fp, "%s", Branches[*BranchCounter]);
	}
	fclose(fp);
}

void branch(int argc, char * argv[])
{
	if (argc > 3)
	{
		printf("Too many parameters...\n");
		return;
	}
	
	char Branches[100][100];
	int BranchCounter = 0;
	load_branches(Branches, &BranchCounter);
	if (argc == 2)
	{
		int i;
		for(i = 0; i < BranchCounter; i++)
			printf("%s\n", Branches[i]);
		return;
	}
	
	int i;
	for(i = 0; i < BranchCounter; i++)
	{
		if (!strcmp(Branches[i], CopyArgv[2]))
		{
			printf("This branch is already made...\n");
			return;
		}
	}
	char WFileAdd[1000];
	char LCID[20];
	
	// add new branch
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\branches");
	FILE * fp = fopen(WFileAdd, "a");
	fprintf(fp, "%s\n", CopyArgv[2]);
	fclose(fp);
	
	// retrieve last commit id
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\last_commit");
	fp = fopen(WFileAdd, "r");
	fscanf(fp, "%s\n", LCID);
	fclose(fp);
	
	// make new commit id
	time_t CommitTime;
	CommitTime = time(NULL);
	char CommitTimeS[20];
	sprintf(CommitTimeS, "%ld", CommitTime);
	
	// make proper directories
	char NewCommitPath[1000];
	strcpy(NewCommitPath, GitRoot);
	strcat(NewCommitPath, "\\.neogit\\repository\\");
	strcat(NewCommitPath, CopyArgv[2]);
	create_dir(NewCommitPath);
	strcat(NewCommitPath, "\\");
	strcat(NewCommitPath, CommitTimeS);
	create_dir(NewCommitPath);
	
	// retrieve last commit info
	char Message[100];
	char User[100];
	char Branch[100];
	int Number;
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\repository\\");
	strcat(WFileAdd, LCID);
	strcat(WFileAdd, "_info");
	fp = fopen(WFileAdd, "r");
	if (!fp)
	{
		printf("Cannot open last commit file info\n");
		return;
	}
	fscanf(fp, "%[^\n]s", Message);
	fscanf(fp, "\n%[^\n]s", User);
	fscanf(fp, "\n%[^\n]s", Branch);
	fscanf(fp, "%d", &Number);
	fclose(fp);
	
	// make source address
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\repository\\");
	strcat(WFileAdd, Branch);
	strcat(WFileAdd, "\\");
	strcat(WFileAdd, LCID);
	
	// make BAT file to copy files
	fp = fopen("Temp.bat", "w");
	if (!fp)
	{
		printf("Cannot create bat file\n");
		return;
	}
	fprintf(fp, "echo OFF\n");
	fprintf(fp, "xcopy %s %s /s\n", WFileAdd, NewCommitPath);
	fprintf(fp, "echo ON\n");
	fclose(fp);
	system("Temp.bat");
	remove("Temp.bat");
	
	// change & make commit files
	char WFileDest[1000];
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\commits");
	fp = fopen(WFileAdd, "a");
	if (!fp)
	{
		printf("Cannot open commits file\n");
		return;
	}
	fprintf(fp, "%s\n", CommitTimeS);
	fclose(fp);
	
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\last_commit");
	fp = fopen(WFileAdd, "w");
	if (!fp)
	{
		printf("Cannot open last commit file \n");
		return;
	}
	fprintf(fp, "%s\n", CommitTimeS);
	fprintf(fp, "%s\n", CopyArgv[2]);
	fclose(fp);
	
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\repository\\");
	strcat(WFileAdd, CommitTimeS);
	strcat(WFileAdd, "_info");
	fp = fopen(WFileAdd, "w");
	fprintf(fp, "%s\n%s\n%s\n%d", Message, User, CopyArgv[2], Number);
	fclose(fp);
	
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\repository\\");
	strcat(WFileAdd, CommitTimeS);
	strcat(WFileAdd, "_files");
	FILE * Dest = fopen(WFileAdd, "w");
	
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\repository\\");
	strcat(WFileAdd, LCID);
	strcat(WFileAdd, "_files");
	fp = fopen(WFileAdd, "r");
	
	char c = fgetc(fp); 
    while (c != EOF) 
    { 
        fputc(c, Dest); 
        c = fgetc(fp); 
    } 
    fclose(fp); 
    fclose(Dest); 
}

void load_Commits(char Commits[][100], int * CommitsCounter)
{
	char WFileAdd[1000];
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\commits");
	FILE * fp = fopen(WFileAdd, "r");
	if (!fp)
	{
		printf("Commits file not found...\n");
		return;
	}
	fscanf(fp, "%s", Commits[*CommitsCounter]);
	while(!feof(fp))
	{
		(*CommitsCounter)++;
		fscanf(fp, "%s", Commits[*CommitsCounter]);
	}
	fclose(fp);
	return;
}

int check_change()
{
	char LC[20];
	time_t LCT;
	char Path[1000];
	strcpy(Path, GitRoot);
	strcat(Path, "\\.neogit\\last_commit");
	FILE * fp = fopen(Path, "r");
	if (!fp)
	{
		printf("Last commit file cannot be opened...\n");
		return 0;
	}
	fscanf(fp, "%s", LC);
	fclose(fp);
	if (!strcmp(LC, "None"))
	{
		return 1;
	}
	LCT = strtol(LC, NULL, 10);
	char Command[1000];
	strcpy(Command, "dir ");
	strcat(Command, GitRoot);
	strcat(Command, "\\*.* /A-D /S /B > c:\\git\\temp");
	system(Command);
	struct stat St;
	fp = fopen("c:\\git\\temp", "r");
	if (!fp)
	{
		printf("Temp list file is not accessible...\n");
		return;
	}
	fscanf(fp, "%[^\n]s", Path);
	while(!feof(fp))
	{
		if (strstr(Path, "\\.neogit\\"))
		{
			fscanf(fp, "\n%[^\n]s", Path);
			continue;
		}
		stat(Path, &St);
		if (St.st_mtime > LCT)
		{
			return 1;
		}
		fscanf(fp, "\n%[^\n]s", Path);
	}
	fclose(fp);
	remove("c:\\git\\temp");
	return 0;
}

int checkout_branch(char * BranchName)
{
	char Branches[100][100];
	int BranchCounter = 0;
	char Branch[100];
	char CommitID[20];
	load_branches(Branches, &BranchCounter);
	int i, Sw = 0;
	for(i = 0; i < BranchCounter; i++)
	{
		if (!strcmp(BranchName, Branches[i]))
		{
			strcpy(Branch, Branches[i]);
			Sw = 1;
			break;
		}
	}
	if (!Sw)
	{
		return 0;
	}
	struct Commit_Log Commit_Logs[1000];
	int CommitCounter = 0;
	Load_Commits_Info(Commit_Logs, &CommitCounter);
	for(i = CommitCounter - 1; i >= 0; i--)
	{
		if (!strcmp(Commit_Logs[i].CommitBranch, Branch))
		{
			strcpy(CommitID, Commit_Logs[i].CommitID);
			break;
		}
	}
	
	char SourcePath[1000];
	
	strcpy(SourcePath, GitRoot);
	strcat(SourcePath, "\\.neogit\\repository\\");
	strcat(SourcePath, Branch);
	strcat(SourcePath, "\\");
	strcat(SourcePath, CommitID);
	
	char Command[1000];
	strcpy(Command, "xcopy \"");
	strcat(Command, SourcePath);
	strcat(Command, "\" \"");
	strcat(Command, GitRoot);
	strcat(Command, "\" /Y /S /Q");
	system(Command);
	strcpy(ActiveBranch, Branch);
	strcpy(CheckoutMode, "HEAD");
	save_configuration(0);
	printf("Branch %s checkedout...\n", Branch);
	return 1;
}

int checkout_id(char * InputID)
{
	char Commits[100][100];
	int CommitsCounter = 0;
	char Branch[100];
	char CommitID[20];
	load_Commits(Commits, &CommitsCounter);
	int i, Sw = 0;
	for(i = 0; i < CommitsCounter; i++)
	{
		if (!strcmp(InputID, Commits[i]))
		{
			strcpy(CommitID, Commits[i]);
			Sw = 1;
			break;
		}
	}
	if (!Sw)
	{
		return 0;
	}
	struct Commit_Log Commit_Logs[1000];
	int CommitCounter = 0;
	Load_Commits_Info(Commit_Logs, &CommitCounter);
	for(i = CommitCounter - 1; i >= 0; i--)
	{
		if (!strcmp(Commit_Logs[i].CommitID, CommitID))
		{
			strcpy(Branch, Commit_Logs[i].CommitBranch);
			break;
		}
	}
	char SourcePath[1000];
	
	strcpy(SourcePath, GitRoot);
	strcat(SourcePath, "\\.neogit\\repository\\");
	strcat(SourcePath, Branch);
	strcat(SourcePath, "\\");
	strcat(SourcePath, CommitID);
	
	char Command[1000];
	strcpy(Command, "xcopy \"");
	strcat(Command, SourcePath);
	strcat(Command, "\" \"");
	strcat(Command, GitRoot);
	strcat(Command, "\" /Y /S /Q");
	system(Command);
	strcpy(CheckoutMode, "OLD");
	save_configuration(0);
	printf("Commit ID %s checkedout...\n", CommitID);
	return 1;	
}

void checkout(int argc, char * argv[])
{
	if (argc > 3)
	{
		printf("Too many parameters...\n");
		return;
	}
	if (!strcmp(CopyArgv[2], "HEAD"))
	{
		checkout_branch(ActiveBranch);
		return;
	}	
	if (check_change())
	{
		printf("There are files changed since last commit, cannot checkout...\n");
		return;	
	}
	int res = checkout_branch(CopyArgv[2]);
	if (res == 0)
	{
		res = checkout_id(CopyArgv[2]);
	}
	if (!res)
	{
		printf("The parameter is neither a branch name nor a commit ID...\n");
		return;
	}
}

void load_alias(char aliasNames[100][100], char aliasCommands[100][1000], int * aliasCounter)
{
	FILE * fp = fopen("c:\\git\\neogit_alias", "r");
	if (!fp)
	{
		printf("Global alias file not found...\n");
		return;
	}
	fscanf(fp, "%[^\n]s", aliasNames[*aliasCounter]);
	while(!feof(fp))
	{
		fscanf(fp, "\n%[^\n]s", aliasCommands[*aliasCounter]);
		(*aliasCounter)++;
		fscanf(fp, "\n%[^\n]s", aliasNames[*aliasCounter]);
	}
	fclose(fp);
	
	char WFileAdd[1000];
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\alias");
	fp = fopen(WFileAdd, "r");
	if (!fp)
	{
		printf("Local alias file not found...\n");
		return;
	}
	fscanf(fp, "%[^\n]s", aliasNames[*aliasCounter]);
	while(!feof(fp))
	{
		fscanf(fp, "\n%[^\n]s", aliasCommands[*aliasCounter]);
		(*aliasCounter)++;
		fscanf(fp, "\n%[^\n]s", aliasNames[*aliasCounter]);
	}
	fclose(fp);
}

int check_alias(int argc, char * argv[], char aliasNames[100][100], char aliasCommands[100][1000], int aliasCounter)
{
	int i;
	for(i = 0; i < aliasCounter; i++)
	{
		if (!strcmp(CopyArgv[1], aliasNames[i]))
		{
			strcpy(AliasString, aliasCommands[i]);
			return i;
		}
	}
	return -1;
}

int load_and_check_alias(int argc, char * argv[])
{
	char aliasNames[100][100];
	char aliasCommands[100][1000];
	int aliasCounter = 0;
	load_alias(aliasNames, aliasCommands, &aliasCounter);
	if (check_alias(argc, argv, aliasNames, aliasCommands, aliasCounter) != -1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void ParseMainArg(int argc, char * argv[])
{
	if (!strcmp(argv[1], "init"))
	{
		init(argc);
		return;
	}
	if (!strcmp(argv[1], "config"))
	{
		config(argc, argv);
		return;
	}
	if (!strcmp(GitRoot, "None"))
	{
		printf("There is no initialized repository...\n");
		return;
	}
	if (!strcmp(argv[1], "add"))
	{
		add(argc, argv);
		return;
	}
	if (!strcmp(argv[1], "reset"))
	{
		reset(argc, argv);
		return;
	}
	if (!strcmp(argv[1], "status"))
	{
		status();
		return;
	}
	if (!strcmp(argv[1], "set"))
	{
		setSM(argc, argv);
		return;
	}
	if (!strcmp(argv[1], "replace"))
	{
		replaceSM(argc, argv);
		return;
	}
	if (!strcmp(argv[1], "remove"))
	{
		removeSM(argc, argv);
		return;
	}
	if (!strcmp(argv[1], "commit"))
	{
		commit(argc, argv);
		return;
	}	
	if (!strcmp(argv[1], "log"))
	{
		log_git(argc, argv);
		return;
	}
	if (!strcmp(argv[1], "branch"))
	{
		branch(argc, argv);
		return;
	}
	if (!strcmp(argv[1], "checkout"))
	{
		checkout(argc, argv);
		return;
	}
	if (load_and_check_alias(argc, argv))
	{
		system(AliasString);
		return;
	}
	printf("Wrong Command...\n");
	return;
}

// a function to load Global Configuration
void LoadGlobalConfig()
{
	FILE * fp = fopen("c:\\git\\neogit_config", "r");
	if (!fp)
	{
		printf("Global config file not found...\n");
		return;
	}
	strcpy(CurUser.name_mode, "1");
	strcpy(CurUser.email_mode, "1");
	fscanf(fp, "%[^\n]s", &CurUser.Username);
	fscanf(fp, "\n%[^\n]s", &CurUser.Email);
	strcpy(ActiveBranch, "master");
	strcpy(CheckoutMode, "HEAD");
	fclose(fp);
}

// a function to load Local Configuration
void LoadLocalConfig()
{
  	char WFileAdd[256];
  	char TempData[256];
  	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit");
	if (dir_exists(WFileAdd))
	{
		strcat(WFileAdd, "\\config");
		FILE * fp = fopen(WFileAdd, "r");
		if (!fp)
		{
			printf("Local config file not found...\n");
			return;
		}
		fscanf(fp, "%[^\n]s", &CurUser.name_mode);
		if (!strcmp(CurUser.name_mode, "0"))
		{
			fscanf(fp, "\n%[^\n]s", &CurUser.Username);
		}
		else
		{
			fscanf(fp, "\n%[^\n]s", TempData);
		}
		fscanf(fp, "\n%[^\n]s", &CurUser.email_mode);
		if (!strcmp(CurUser.name_mode, "0"))
		{
			fscanf(fp, "\n%[^\n]s", &CurUser.Email);
		}
		else
		{
			fscanf(fp, "\n%[^\n]s", TempData);
		}
		fscanf(fp, "\n%[^\n]s", ActiveBranch);
		fscanf(fp, "\n%[^\n]s", CheckoutMode);
		fclose(fp);
	}
}

void toLower(int argc, char *argv[])
{
	int i,j;
	for(i = 0; i < argc; i++)
	{
		strcpy(CopyArgv[i], argv[i]);
	}
	for(i = 1; i < argc; i++)
	{
		for(j = 0; j < strlen(argv[i]); j++)
		{
			if (argv[i][j] >= 'A' && argv[i][j] <= 'Z')
				argv[i][j] = tolower(argv[i][j]);
		}
	}
}

int main(int argc, char * argv[])
{
	Extract_CWD();
	Extract_Root();
	LoadGlobalConfig();
	LoadLocalConfig();
	toLower(argc, argv);
	ParseMainArg(argc, argv);
	return 0;
}
