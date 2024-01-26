//محمد شریفی طهرانی 402106123
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

struct User {
	char mode[2];
	char Username[256];
	char Email[256];
};

struct Commit{
	char Path[256];
	long int Size;
	time_t ModifiedTime;	
};

struct User CurUser;
char cwd[256];
char GitRoot[256];
char ** CopyArgv;
char ActiveBranch[100];

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
    	printf("failed to create directory");
    	exit(1);
  	}
}

// A function to create a file
void create_file(char *path) {
  	FILE *fp = fopen(path, "w");
  	if (fp == NULL) {
    	printf("failed to create file");
    	exit(1);
  	}
  	fclose(fp);
}

// A function to set UserName & Email
void setconfiguration()
{
	FILE * fp = fopen(".neogit\\config", "w");
	if (!fp)
	{
		printf("failed to open config file");
		exit(1);
	}
	fprintf(fp, "%s\n", CurUser.mode);
	fprintf(fp, "%s\n", CurUser.Username);
	fprintf(fp, "%s\n", CurUser.Email);
	fprintf(fp, "%s\n", ActiveBranch);
	fclose(fp);
	
	fp = fopen(".neogit\\branches", "w");
	if (!fp)
	{
		printf("failed to open branches file");
		exit(1);
	}
	fprintf(fp, "%s\n", ActiveBranch);
	fclose(fp);
	
	fp = fopen(".neogit\\last_commit", "w");
	if (!fp)
	{
		printf("failed to open branches file");
		exit(1);
	}
	fprintf(fp, "%s\n", "None");
	fclose(fp);
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
  	char WDirAdd[256];
  	char Temp[256];
   	strcpy(WDirAdd, cwd);
	strcat(WDirAdd, "\\.neogit");
	if (dir_exists(WDirAdd))
	{
		printf("neogit repository already exists in current directory\n");
	   	exit(1);	
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
			strcat(WDirAdd, ".neogit");
			if (dir_exists(WDirAdd))
			{
				printf("neogit repository already exists in %s\n", Temp);
		       	exit(1);	
			}
			Pos--;
		}
	}
  	
	// Create the neogit folder and its subfolders and files
  	system("c:\\git\\init.bat");
  	printf("Initialized empty neogit repository in %s\n", cwd);
  	setconfiguration(cwd);
}

// A function to handle "config" command
void config(int argc, char * argv[])
{
	int isglobal = 0;
	// check parameters
	if(argc > 5)
	{
		printf("Too many parameters...");
		return;
	}
	// check global switch
	if (!strcmp(argv[2], "-global"))
		isglobal = 1;
	
	// check parameters
	if (!strcmp(argv[2+isglobal], "user.name"))
		strcpy(CurUser.Username, CopyArgv[3+isglobal]);
	if (!strcmp(argv[2+isglobal], "user.email"))
		strcpy(CurUser.Email, CopyArgv[3+isglobal]);
	if (strcmp(argv[2+isglobal], "user.name") && strcmp(argv[2+isglobal], "user.email"))
	{
	   	printf("wrong parameters\n");
		exit(1);
	}
	
	// set new configuration for local project
	char TempDir[256];
	strcpy(TempDir, GitRoot);
	strcat(TempDir, "\\.neogit\\config");
	FILE * fp = fopen(TempDir, "w");
	if (!fp)
	{
		printf("Local Config File Not Found...\n");
		return;
	}	
	if (isglobal)
		fprintf(fp, "%s\n", "1");
	else
		fprintf(fp, "%s\n", "0");
	fprintf(fp, "%s\n", CurUser.Username);
	fprintf(fp, "%s\n", CurUser.Email);
	printf("Local Configuration Changed...\n");
	fclose(fp);
	
	// set new configuration for all projects
	if (isglobal)
	{
		fp = fopen("C:\\git\\neogit_config", "w");
		if (!fp)
		{
			printf("Local Config File Not Found...\n");
			return;
		}	
		fprintf(fp, "%s\n", CurUser.Username);
		fprintf(fp, "%s\n", CurUser.Email);
		printf("Global Configuration Changed...\n");
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
			if (St.st_size == Commited[i].Size && St.st_mtime == Commited[i].ModifiedTime)
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
		printf("stage file not exists...\n");
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
		printf("stage file not exists...\n");
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
    	printf("cannot open directory %s\n", Path);
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
		printf("Stage file not exists...\n");
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
		printf("Commited file not exists...\n");
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
		printf("last Commited file not exists...\n");
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
	
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\last_commit");
	FILE * fp = fopen(WFileAdd, "r");
	if (!fp) {
		printf("Commited file not exists...\n");
		return;
	}
	fscanf(fp, "%s", LastCommit);
	fclose(fp);
	
	strcpy(CommitPath, GitRoot);
	strcat(CommitPath, "\\.neogit\\repository\\");
	strcat(CommitPath, ActiveBranch);
	strcat(CommitPath, "\\");
	strcat(CommitPath, LastCommit);
	
	struct stat St;
	for(i = 0; i < CommitedCounter; i++)
	{
		extract_path(Commited[i].Path, DiffPath, FileName);
		
		strcpy(TempPath, CommitPath);
		strcat(TempPath, "\\");
		strcat(TempPath, DiffPath);
		strcat(TempPath, "\\");
		strcat(TempPath, FileName);
		stat(TempPath, &St);

		Commited[i].Size = St.st_size;
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
	strcat(WFileAdd, argv[2]);
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
    	printf("cannot open directory %s\n", Path);
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

	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\staged");
	FILE * fp = fopen(WFileAdd, "r");
	if (!fp) {
		printf("Stage file not exists...\n");
		return;
	}
	
	fscanf(fp, "%s", Staged[StagedCounter]);
	while(!feof(fp))
	{
		StagedCounter++;
		fscanf(fp, "%s", Staged[StagedCounter]);
	}
	fclose(fp);
	
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
	
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\staged");
	FILE * fp = fopen(WFileAdd, "r");
	if (!fp) {
		printf("Stage file not exists...\n");
		return;
	}
	
	int StagedCounter = 0;
	fscanf(fp, "%s", Staged[StagedCounter]);
	while(!feof(fp))
	{
		StagedCounter++;
		fscanf(fp, "%s", Staged[StagedCounter]);
	}
	fclose(fp);

	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\commits");
	fp = fopen(WFileAdd, "r");
	if (!fp) {
		printf("Commited file not exists...\n");
		return;
	}
	int CommitedCounter = 0;
	fscanf(fp, "%s %ld %ld", Commited[CommitedCounter].Path, Commited[CommitedCounter].Size, Commited[CommitedCounter].ModifiedTime);
	while(!feof(fp))
	{
		CommitedCounter++;
		fscanf(fp, "%s %ld %ld", Commited[CommitedCounter].Path, Commited[CommitedCounter].Size, Commited[CommitedCounter].ModifiedTime);
	}
	fclose(fp);

	DIR *dir = opendir(cwd);
	if (dir == NULL) {
    	printf("cannot open directory %s\n", cwd);
    	return;
	}
	struct dirent *entry;
	char StagedSign;
	char ModifiedSign;
	int ModifiedMode;
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
    		printf("%c %c %s\n", StagedSign, ModifiedSign, Temp);
		}
	}	
}

void Load_SCMess(char SCName[100][75], char SCMess[100][75], int * SCMessCounter)
{
	char WFileAdd[256];
	
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\SCMess");
	FILE * fp = fopen(WFileAdd, "r");
	if (!fp) {
		printf("shortcut meesages file not exists...\n");
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
		printf("shortcut meesages file not exists...\n");
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
		printf("wrong parameters...\n");
		return;
	}
	if (strlen(argv[3]) > 72)
	{
		printf("message should be less than 73 char...\n");
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
			printf("message shortcut already exists...\n");
			return;
		}
	}
	
	char WFileAdd[256];
	
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\SCMess");
	FILE * fp = fopen(WFileAdd, "a");
	if (!fp) {
		printf("shortcut meesages file not exists...\n");
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
		printf("wrong parameters...\n");
		return;
	}
	if (strlen(argv[3]) > 72)
	{
		printf("message should be less than 73 char...\n");
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
		printf("message shortcut not found...\n");
		return;
	}
	Save_SCMess(SCName, SCMess, SCMessCounter);
}

void removeSM(int argc, char * argv[])
{
	if (strcmp(argv[2], "-s"))
	{
		printf("wrong parameters...\n");
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
		printf("message shortcut not found...\n");
		return;
	}
	SCMessCounter--;
	Save_SCMess(SCName, SCMess, SCMessCounter);
}

void commit(int argc, char * argv[])
{
	if (argc != 4)
	{
		printf("few parameters...\n");
		return;
	}
	if (strcmp(argv[2], "-m") && strcmp(argv[2], "-s"))
	{
		printf("wrong parameters...\n");
		return;
	}
	if (strlen(argv[3]) > 72)
	{
		printf("message should be less than 73 char...\n");
		return;
	}
	
	char Message[75];
	strcpy(Message, argv[3]);
	if (strcmp(argv[2], "-s") == 0)
	{
		int SCMessCounter = 0;
		char SCName[100][75];
		char SCMess[100][75];
		Load_SCMess(SCName, SCMess, &SCMessCounter);
		int i;
		for(i = 0; i < SCMessCounter; i++)
		{
			if (!strcmp(CopyArgv[3], SCName[i]))
			{
				strcpy(Message, SCMess[i]);
				break;
			}
		}
		printf("shortcut message not found...\n");
		return;
	}
	
	char Staged[100][256];
	int StagedCounter;
	
	load_staged(Staged, &StagedCounter);
	if (StagedCounter == 0)
	{
		printf("no file in stage mode...\n");
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
	printf("%d files comitted.\n", StagedCounter);
	printf("stage id : %s\n", CommitTimeS);
	printf("comitt message : %s\n", Message);
	printf("commit time : %s", ctime(&CommitTime));
	
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
	fclose(fp);
	
	char StagedFilePath[1000];
	strcpy(StagedFilePath, GitRoot);
	strcat(StagedFilePath, "\\.neogit\\staged");
	fp = fopen(StagedFilePath, "w");
	fclose(fp);
}

void log_git(int argc, char * argv[])
{
	
}

void load_branches(char Branches[][100], int * BranchCounter)
{
	char WFileAdd[1000];
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\branches");
	FILE * fp = fopen(WFileAdd, "r");
	if (!fp) {
		printf("branch file not exists...\n");
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
		printf("too many parameters...\n");
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
		if (!strcmp(Branches[i], argv[2]))
		{
			printf("thin branch already existed...\n");
			return;
		}
	}
	char WFileAdd[1000];
	char LCID[20];
	
	// add new branch
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\branches");
	FILE * fp = fopen(WFileAdd, "a");
	fprintf(fp, "%s\n", argv[2]);
	fclose(fp);
	
	// retrive last commit id
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
	strcat(NewCommitPath, argv[2]);
	create_dir(NewCommitPath);
	strcat(NewCommitPath, "\\");
	strcat(NewCommitPath, CommitTimeS);
	create_dir(NewCommitPath);
	
	// retrive last commit info
	char Message[100];
	char User[100];
	char Branch[100];
	char Number[10];
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\repository\\");
	strcat(WFileAdd, LCID);
	strcat(WFileAdd, "_info");
	fp = fopen(WFileAdd, "r");
	fscanf(fp, "%s", Message);
	fscanf(fp, "%s", User);
	fscanf(fp, "%s", Branch);
	fscanf(fp, "%s", Number);
	fclose(fp);
	
	// make source address
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\repository\\");
	strcat(WFileAdd, Branch);
	strcat(WFileAdd, "\\");
	strcat(WFileAdd, LCID);
	
	// make BAT file to copy files
	fp = fopen("Temp.bat", "w");
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
	fprintf(fp, "%s\n", CommitTimeS);
	fclose(fp);
	
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\last_commit");
	fp = fopen(WFileAdd, "w");
	fprintf(fp, "%s\n", CommitTimeS);
	fclose(fp);
	
	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit\\repository\\");
	strcat(WFileAdd, CommitTimeS);
	strcat(WFileAdd, "_info");
	fp = fopen(WFileAdd, "w");
	fprintf(fp, "%s\n%s\n%s\n%d", Message, User, argv[2], Number);
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

void chechout()
{
	
}

void ParseMainArg(int argc, char * argv[])
{
	if (!strcmp(argv[1], "init"))
	{
		init(argc);
		return;
	}
	if (!strcmp(GitRoot, "None"))
	{
		printf("there is no intialized repository...\n");
		return;
	}
	if (!strcmp(argv[1], "config"))
	{
		config(argc, argv);
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
		
	}
}

// a function to load Global Configuration
void LoadGlobalConfig()
{
	FILE * fp = fopen("c:\\git\\neogit_config", "r");
	if (!fp)
	{
		printf("Global Config File Not Found...\n");
		return;
	}
	strcpy(CurUser.mode, "1");
	fscanf(fp, "%s", &CurUser.Username);
	fscanf(fp, "%s", &CurUser.Email);
	strcpy(ActiveBranch, "master");
	fclose(fp);
}

// a function to load Local Configuration
void LoadLocalConfig()
{
  	char WFileAdd[256];
  	struct User Temp;
  	strcpy(WFileAdd, GitRoot);
	strcat(WFileAdd, "\\.neogit");
	if (dir_exists(WFileAdd))
	{
		strcat(WFileAdd, "\\config");
		FILE * fp = fopen(WFileAdd, "r");
		if (!fp)
		{
			printf("Local Config File Not Found...\n");
			return;
		}
		fscanf(fp, "%s", &CurUser.mode);
		fscanf(fp, "%s", &Temp.Username);
		fscanf(fp, "%s", &Temp.Username);
		if (!strcmp(CurUser.mode, "0"))
		{
			strcpy(CurUser.Username, Temp.Username);
			strcpy(CurUser.Email, Temp.Email);
		}
		fscanf(fp, "%s", ActiveBranch);
		fclose(fp);
	}
}

void toLower(int argc, char *argv[])
{
	int i,j;
	CopyArgv = (char **) malloc(argc * sizeof(char*));
	for(i = 0; i < argc; i++)
	{
		CopyArgv[i] = malloc(sizeof(argv[i]));
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

void Extract_Root()
{
	if (getcwd(cwd, sizeof(cwd)) == NULL) {
    	printf("failed to get current working directory");
    	exit(1);
  	}
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
			strcat(WDirAdd, ".neogit");
			if (dir_exists(WDirAdd))
			{
				strcpy(GitRoot, Temp);
		       	return;	
			}
			Pos--;
		}
	}
	strcpy(GitRoot, "None");
	return;
}

int main(int argc, char * argv[])
{
	Extract_Root();
	LoadGlobalConfig();
	LoadLocalConfig();
	toLower(argc, argv);
	ParseMainArg(argc, argv);
	return 1;
}
