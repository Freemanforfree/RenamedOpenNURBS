# Renamed OpenNURBS
## Description
When OpenNURBS and PCL are in the same project, there will be definition conflicts. Due to the lack of namespaces in OpenNURBS, it is difficult to resolve this problem through namespaces. In order to fully use the OpenNURBS library and PCL library, this project replaces all "ON" prefixes with "MYON" through regular expressions, and the test passes.



## Usage

```bash
mkdir build && cd build
cmake ..
make
```

And you can get **libopennurbsStatic.a** \ **libzlib.a** \ **libopennurbs_public_freetype.a** \ **libandroid_uuid.a** in ./build .



## Note

When OpenNURBS 8.x is compiled on the linux platform, the library itself needs to be linked with android_uuid, freetype263 and zlib to run normally, and after adding the **MY_ZCALLOC** compilation option, **OpenNURBS** and **zlib** are interdependent. Sequential linking of **libopennurbsStatic.a** **libzlib.a** may result in undefined symbols "zcfree" \ "zcalloc". 

Try adding the following linking options in your CMakeLists.txt:

```cmake
target_link_libraries (${YOUR_PROJECT_NAME} 
	-Wl,--start-group
	libopennurbsStatic.a
	libzlib.a
	libopennurbs_public_freetype.a
	libandroid_uuid.a
	-Wl,--end-group
	)
```

