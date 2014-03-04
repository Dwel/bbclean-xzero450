// Configuration
static char foopath[MAX_PATH];
static char titleFormat[128];
static char playlistHeader[128];
static char BBITitleName[128];
static char BBISeekName[128];
static char BBIVolumeName[128];
static char BBIAlbumArtName[128];
static char BBIAAImgNames[128];
static char BBIAAImgTypes[128];
static char BBIAANoImage[MAX_PATH];
static char ImgNames[128];
static char ImgTypes[128];
char *imname[32], *imtype[32];
int imnamec=0, imtypec=0;
int BBIAlbumArtHeight;
int BBIAlbumArtWidth;
bool setBBITitle;
bool setBBISeek;
bool setBBIVolume;
bool setBBIAlbumArt;
bool useBBInterface;

// Predefinition
void WriteRCSettings();
void ReadRCSettings();
void SetDefaultRCSettings();
void ReadSettings();
void EmunAlbumArtImg();


void ReadRCSettings()
{
	char temp[MAX_LINE_LENGTH], path[MAX_LINE_LENGTH], defaultpath[MAX_LINE_LENGTH];
	int nLen;
		
	// First we look for the config file in the same folder as the plugin...
	GetModuleFileName(hInstance, rcpath, sizeof(rcpath));
	nLen = strlen(rcpath)-1;
	while (nLen>0 && rcpath[nLen] != '\\') nLen--;
	rcpath[nLen+1] = 0;
	strcpy(temp, rcpath);
	strcpy(path, rcpath);
	strcat(temp, "bbfooman.rc");
	strcat(path, "bbfoomanrc");
	// ...checking the two possible filenames bbfooman.rc and bbfoomanrc ...
	if (FileExists(temp)) strcpy(rcpath, temp);
	else if (FileExists(path)) strcpy(rcpath, path);
	// ...if not found, we try the Blackbox directory...
	else
	{
		// ...but first we save the default path (bbfooman.rc in the same
		// folder as the plugin) just in case we need it later (see below)...
		strcpy(defaultpath, temp);
		GetBlackboxPath(rcpath, sizeof(rcpath));
		strcpy(temp, rcpath);
		strcpy(path, rcpath);
		strcat(temp, "bbfooman.rc");
		strcat(path, "bbfoomanrc");
		if (FileExists(temp)) strcpy(rcpath, temp);
		else if (FileExists(path)) strcpy(rcpath, path);
		else // If no config file was found, we use the default path and settings, and return
		{
			strcpy(rcpath, defaultpath);
			SetDefaultRCSettings();
			WriteRCSettings();
			return;
		}
	}
	// If a config file was found we read the plugin settings from the file...
	// Always checking non-bool values to make sure they are the right format
	ReadSettings();
}


void WriteRCSettings()
{
	WriteString(rcpath, "bbfooman.titleFormat:", titleFormat);
	WriteString(rcpath, "bbfooman.playlistHeader:", playlistHeader);
	WriteString(rcpath, "bbfooman.BBITitleControlName:", BBITitleName);
	WriteString(rcpath, "bbfooman.BBISeekControlName:", BBISeekName);
	WriteString(rcpath, "bbfooman.BBIVolumeControlName:", BBIVolumeName);
	WriteString(rcpath, "bbfooman.BBIAlbumArtControlName:", BBIAlbumArtName);
	WriteString(rcpath, "bbfooman.BBIAAImgNames:", BBIAAImgNames);
	WriteString(rcpath, "bbfooman.BBIAAImgTypes:", BBIAAImgTypes);
	WriteString(rcpath, "bbfooman.BBIAANoImage:", BBIAANoImage);
	WriteString(rcpath, "bbfooman.FoobarPath:", foopath);
	WriteBool(rcpath, "bbfooman.useBBInterface:", useBBInterface);
	WriteBool(rcpath, "bbfooman.setBBITitle:", setBBITitle);
	WriteBool(rcpath, "bbfooman.setBBISeek:", setBBISeek);
	WriteBool(rcpath, "bbfooman.setBBIVolume:", setBBIVolume);
	WriteBool(rcpath, "bbfooman.setBBIAlbumArt:", setBBIAlbumArt);
	WriteInt(rcpath, "bbfooman.BBIAlbumArtHeight:", BBIAlbumArtHeight);
	WriteInt(rcpath, "bbfooman.BBIAlbumArtWidth:", BBIAlbumArtWidth);
}


void ReadSettings()
{
	strcpy(titleFormat, ReadString(rcpath, "bbfooman.titleFormat:", "$if(%title%,$if2(%album artist%,%artist%) - %title%,%_filename_ext%)"));
	strcpy(playlistHeader, ReadString(rcpath, "bbfooman.playlistHeader:", "$if2(%album artist%,%artist%) '['%album%']'"));
	strcpy(BBITitleName, ReadString(rcpath, "bbfooman.BBITitleControlName:", "Title"));
	strcpy(BBISeekName, ReadString(rcpath, "bbfooman.BBISeekControlName:", "SeekBar"));
	strcpy(BBIVolumeName, ReadString(rcpath, "bbfooman.BBIVolumeControlName:", "Volume"));
	strcpy(BBIAlbumArtName, ReadString(rcpath, "bbfooman.BBIAlbumArtControlName:", "AlbumArt"));
	strcpy(BBIAAImgNames, ReadString(rcpath, "bbfooman.BBIAAImgNames:", "folder;cover;coverart;front;album;albumart;"));
	strcpy(BBIAAImgTypes, ReadString(rcpath, "bbfooman.BBIAAImgTypes:", "jpg;gif;png;bmp;tif;"));
	EmunAlbumArtImg();
	strcpy(BBIAANoImage, ReadString(rcpath, "bbfooman.BBIAANoImage:", "NoImage.jpg"));
	strcpy(foopath, ReadString(rcpath, "bbfooman.FoobarPath:", "C:\\Program Files\\Foobar2000\\Foobar2000.exe"));
	BBIAlbumArtHeight = ReadInt(rcpath, "bbfooman.BBIAlbumArtHeight:", 200);
	BBIAlbumArtWidth = ReadInt(rcpath, "bbfooman.BBIAlbumArtWidth:", 200);
	setBBITitle = ReadBool(rcpath, "bbfooman.setBBITitle:", false);
	setBBISeek = ReadBool(rcpath, "bbfooman.setBBISeek:", false);
	setBBIVolume = ReadBool(rcpath, "bbfooman.setBBIVolume:", false);
	setBBIAlbumArt = ReadBool(rcpath, "bbfooman.setBBIAlbumArt:", false);
	useBBInterface = ReadBool(rcpath, "bbfooman.useBBInterface:", false);
}


void SetDefaultRCSettings()
{
	strcpy(foopath, "C:\\Program Files\\Foobar2000\\Foobar2000.exe");
	strcpy(titleFormat, "$if(%title%,$if2(%album artist%,%artist%) - %title%,%_filename_ext%)");
	strcpy(playlistHeader, "$if2(%album artist%,%artist%) '['%album%']'");
	strcpy(BBITitleName, "Title");
	strcpy(BBISeekName, "SeekBar");
	strcpy(BBIVolumeName, "Volume");
	strcpy(BBIAlbumArtName, "AlbumArt");
	strcpy(BBIAAImgNames, "folder;cover;coverart;front;album;albumart;");
	strcpy(BBIAAImgTypes, "jpg;gif;png;bmp;tif;");
	strcpy(BBIAANoImage, "NoImage.jpg");
	BBIAlbumArtHeight = 200;
	BBIAlbumArtWidth = 200;
	setBBITitle = true;
	setBBISeek = true;
	setBBIVolume = true;
	setBBIAlbumArt = true;
	useBBInterface = true;
}

void EmunAlbumArtImg()
{
	char *schar, *list;

	strcpy(ImgNames, BBIAAImgNames);
	list = ImgNames;
	while(schar = strchr(list, ';')) {schar[0]=0; imname[imnamec++]=list; list=schar+1;}

	strcpy(ImgTypes, BBIAAImgTypes);
	list = ImgTypes;
	while(schar = strchr(list, ';')) {schar[0]=0; imtype[imtypec++]=list; list=schar+1;}
}
