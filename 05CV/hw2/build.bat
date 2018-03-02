mkdir build  
cd build  
cmake -G "Visual Studio 14 Win64" ..  
MSBuild ALL_BUILD.vcxproj /p:Platform=x64 /p:Configuration=Debug  
MSBuild ALL_BUILD.vcxproj /p:Platform=x64 /p:Configuration=Release 
