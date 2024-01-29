ECHO OFF
xcopy c:\git\Template /s /e
attrib .neogit +h
attrib .neogit\*.* +h /s /d
attrib .neogit\*.* -h
ECHO ON

