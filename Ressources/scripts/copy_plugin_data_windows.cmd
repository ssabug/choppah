@echo off
cls
set PLUGIN_DIR=%HomePath%\ssabug\choppah
SET SCRIPT_DIR=%~dp0
 ::%~dp0:~0,-1%
set SOURCE_DIR=%~dp0..\..\

echo /////// Creating folders in %PLUGIN_DIR%
md %HomePath%\ssabug
md %PLUGIN_DIR%
md %PLUGIN_DIR%\config

echo /////// Copying files from  %SOURCE_DIR%\Ressources\ ...
Xcopy %SOURCE_DIR%Ressources\config_files\config.xml %PLUGIN_DIR%\config\config.xml
Xcopy /E /I %SOURCE_DIR%Ressources\skins %PLUGIN_DIR%\skins
Xcopy /E /I %SOURCE_DIR%Ressources\presets %PLUGIN_DIR%\presets

echo /////// Finished