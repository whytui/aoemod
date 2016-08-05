
*******
ROR_API
*******

ROR_API is an unofficial extension for Age of Empires: Rise of Rome.
It allows you to add any custom plug-in to Rise of Rome without having to modify the game itself.

ROR_API requires CustomizeAOE >=2.0.1.2 but please use latest version (http://aoe.heavengames.com/dl-php/showfile.php?fileid=2464)


***********
* INSTALL *
***********

Run CustomizeAOE, click File/Install CustomROR menu entry. Then follow the indications.


To install only ROR_API (without customROR):
- Run CustomizeAOE, select empiresx.exe file and click File/Edit game file menu entry.
- Then select ROR_API entry in treeview, check the "enable feature" box and click Save.
- Put ROR_API.dll in your Age of Empires: Rise of Rome installation directory.
- Put (or create) ROR_API.conf in your Age of Empires: Rise of Rome installation directory.
...[Please note that this only has to be done once, now you can plug/unplug any DLL just by changing configuration]...
- Open ROR_API.conf in a text editor and list the DLL plug-ins you want to add.
- Write 1 DLL file name per line without spaces or any extra character. Invalid lines are ignored (see provided example).
- Subdirectories are supported (example: CustomROR\customROR.dll)


***********
* EXAMPLE *
***********
ROR_API is compatible with windowed mode.
To use windowed mode in RoR:
- Get wndmode.dll (for example: http://aoe.heavengames.com/dl-php/showfile.php?fileid=2527)
- Add a line "wndmode.dll" to ROR_API.conf (no other character on the line, not even spaces)
- Use CustomizeAOE to enable the "windowed mode" options (this will fix several mouse issues in windowed mode)


**********************
* CUSTOM PLUGIN DEMO *
**********************

- Requires to install CustomROR with CustomizeAOE
- Add "customROR.dll" to ROR_API.conf if it is not already there.
- Run a game, you should see a chat message from CustomROR.
See the CustomROR_README.txt file for details about features.


***************
* FOR MODDERS *
***************

Wanna create your own compatible plug-in ?
Compile your own DLL using the provided file ROR_API_pub.h and create a method DispatchToCustomCode.
See ROR_API_pub.h for more details.

How do I call my plug-in DLL from RoR ?
Just add a "CALL 0x419574" instruction where you want in EXE file.
The return address will be useful to write your DispatchToCustomCode method.


***************
* LIMITATIONS *
***************

- Calling ROR_API does NOT preserve flags (be careful with JE, etc)
- Calling ROR_API does NOT preserve ST registers (for float calculations)
- You can't change ESP value in ROR_API, stack position must be preserved: it is not possible to PUSH dwords in ROR_API (or POP)
... However you can PUSH arguments, make a call yourself, then POP arguments (if needed)

***********
* WARNING *
***********

FPU (float register stack in assembler) works as a stack. When a FLD (float load) is done, there must be a matching operation to pop the value.
For example, each FLD should have a FSTP (float store and pop). Other operations "pop" current float value, refer to x86 documentation.
If you modify game code to add a code to ROR_API, make sure you never remove a FLD, FSTP or any other "float" operation.
It is NOT recommended to call ROR_API in the middle of float operations.
