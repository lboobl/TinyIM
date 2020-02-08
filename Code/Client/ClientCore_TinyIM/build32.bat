mkdir Build32Bat
cd Build32Bat
cmake ..
MSBuild.exe ClientCore_TinyIM.sln -property:Configuration=Debug
pause
cd ..